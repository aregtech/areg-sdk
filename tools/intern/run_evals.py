#!/usr/bin/env python3
# ===========================================================================
# Grades an application an agent built for one of the tasks in
# tools/intern/evals/tasks.json.
#
#   python3 tools/intern/run_evals.py --list
#   python3 tools/intern/run_evals.py --task 04-timer --dir ~/work/tick
#   python3 tools/intern/run_evals.py --self-check --sdk-root .
#
# The task bank is prompts and pass criteria; nothing here runs an agent. An
# agent is given a prompt, writes a project into a directory, and this decides
# whether the result works: it configures, builds, runs and checks the output.
#
# --self-check grades the reference recipes instead, which is how the harness
# itself is verified. The framework is built once: the SDK's own build directory
# is brought up to date and installed into a prefix under the system temp folder,
# which later runs reuse while framework/ and conf/cmake/ are unchanged. Every
# recipe is built once against it, all at the same time, through its own
# find_package(areg), and every task of that recipe is graded on the one build. --from-source builds the framework inside
# each recipe instead, through its FetchContent block: the path a project without
# an installed SDK takes, for a change to the framework's CMake.
#
# Report a run with --tokens and --hops to record what it cost; the numbers are
# printed back with the verdict and are not otherwise used.
#
# Exit code 0 when every graded task passed, 1 otherwise.
# ===========================================================================
import argparse
import glob
import hashlib
import json
import os
import re
import shutil
import subprocess
import sys
import tempfile
import time
from concurrent.futures import ThreadPoolExecutor

sys.path.insert(0, os.path.join(os.path.dirname(os.path.dirname(os.path.abspath(__file__))),
                                'agent'))
import build_project  # noqa: E402
import run_scenarios  # noqa: E402
import service_ports  # noqa: E402

SDK = os.path.dirname(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
BANK = os.path.join(SDK, 'tools', 'intern', 'evals', 'tasks.json')
RECIPES = os.path.join(SDK, 'docs', 'agent', 'recipes')


def load_tasks():
    with open(BANK, encoding='utf-8') as handle:
        return json.load(handle)['tasks']


def run(command, cwd=None, timeout=900):
    try:
        return subprocess.run(command, cwd=cwd, capture_output=True, text=True,
                              timeout=timeout)
    except subprocess.TimeoutExpired:
        return None


def build_config(build_dir, config):
    """Returns the --config arguments a multi-config generator needs, else nothing.

    A multi-config generator, Visual Studio among them, ignores CMAKE_BUILD_TYPE and
    builds Debug unless the build step names the configuration.
    """
    cache = os.path.join(build_dir, 'CMakeCache.txt')
    try:
        with open(cache, 'r', encoding='utf-8', errors='replace') as handle:
            for line in handle:
                if line.startswith('CMAKE_CONFIGURATION_TYPES:'):
                    _, _, value = line.partition('=')
                    if value.strip():
                        return ['--config', config]
                    return []
    except OSError:
        pass
    return []


JOBS = os.cpu_count() or build_project.DEFAULT_JOBS

# The targets an installed SDK carries: the libraries and the services a run needs.
SDK_TARGETS = ('areg', 'aregextend', 'areglogger', 'mtrouter', 'logcollector',
               'logobserver')

# Where an installed SDK keeps the services, beside a project's own build/bin.
SERVICE_DIRS = []


def fingerprint(sdk_root):
    """What the installed framework is made of: every file under framework/ and
    conf/cmake/, and the top CMake files, by name, time and size."""
    digest = hashlib.sha1()
    paths = [os.path.join(sdk_root, name) for name in ('CMakeLists.txt', 'areg.cmake')]
    for top in ('framework', os.path.join('conf', 'cmake')):
        for root, dirs, names in os.walk(os.path.join(sdk_root, top)):
            dirs.sort()
            paths += [os.path.join(root, name) for name in sorted(names)]
    for path in paths:
        try:
            status = os.stat(path)
        except OSError:
            continue
        digest.update('{} {} {}\n'.format(os.path.relpath(path, sdk_root),
                                          status.st_mtime_ns, status.st_size).encode())
    return digest.hexdigest()


def installed_sdk(sdk_root, build_dir, jobs):
    """The framework built from sdk_root and installed once, reused while it is unchanged.

    Returns (prefix, how) or (None, why). The SDK's own build directory is brought
    up to date and installed; with none, one without tests and examples is kept
    beside the installation.
    """
    sdk_root = os.path.realpath(sdk_root)
    holder = os.path.join(tempfile.gettempdir(), 'areg-eval-sdk',
                          hashlib.sha1(sdk_root.encode()).hexdigest()[:12])
    prefix, stamp = os.path.join(holder, 'install'), os.path.join(holder, 'stamp')
    wanted = fingerprint(sdk_root)
    try:
        with open(stamp, encoding='utf-8') as handle:
            if handle.read().strip() == wanted:
                return prefix, 'the framework installed at {}, unchanged'.format(prefix)
    except OSError:
        pass

    build_dir = os.path.abspath(os.path.join(sdk_root, build_dir))
    cache = os.path.join(build_dir, 'CMakeCache.txt')
    if not os.path.isfile(cache):
        build_dir = os.path.join(holder, 'build')
        cache = os.path.join(build_dir, 'CMakeCache.txt')
        result = run(['cmake', '-S', sdk_root, '-B', build_dir, '-DCMAKE_BUILD_TYPE=Release',
                      '-DAREG_TESTS=OFF', '-DAREG_EXAMPLES=OFF'], timeout=1800)
        if result is None or result.returncode != 0:
            return None, 'configuring the SDK in {} failed: {}'.format(
                build_dir, '' if result is None else (result.stderr or result.stdout)[-300:])
    home = None
    with open(cache, encoding='utf-8', errors='replace') as handle:
        for line in handle:
            if line.startswith('CMAKE_HOME_DIRECTORY:'):
                home = line.partition('=')[2].strip()
    if home is None or os.path.realpath(home) != sdk_root:
        return None, '{} is a build of {}, not of {}'.format(build_dir, home, sdk_root)
    config = build_config(build_dir, 'Release')
    result = run(['cmake', '--build', build_dir, '-j', str(jobs), '--target']
                 + list(SDK_TARGETS) + config, timeout=3600)
    if result is None or result.returncode != 0:
        return None, 'the SDK build in {} failed: {}'.format(
            build_dir, '' if result is None else (result.stderr or result.stdout)[-300:])
    shutil.rmtree(prefix, ignore_errors=True)
    result = run(['cmake', '--install', build_dir, '--prefix', prefix] + config)
    if result is None or result.returncode != 0:
        return None, 'installing {} failed: {}'.format(
            build_dir, '' if result is None else (result.stderr or result.stdout)[-300:])
    with open(stamp, 'w', encoding='utf-8') as handle:
        handle.write(wanted + '\n')
    return prefix, 'the framework built from {} and installed at {}'.format(build_dir, prefix)


def build(project, sdk_root, jobs=JOBS, prefix=None):
    """Configures and builds the project the agent produced.

    With prefix, the project's find_package(areg) finds the SDK installed there and
    the framework is not compiled again. Without it, FETCHCONTENT_SOURCE_DIR_AREG
    points the project's own FetchContent block at the local SDK, which it compiles:
    no network, and no edit to the project either way.
    """
    configure = ['cmake', '-B', 'build', '-DCMAKE_BUILD_TYPE=Release']
    if prefix:
        configure.append('-DCMAKE_PREFIX_PATH=' + prefix)
    elif sdk_root:
        configure.append('-DFETCHCONTENT_SOURCE_DIR_AREG=' + os.path.abspath(sdk_root))
    result = run(configure, cwd=project)
    if result is None:
        return False, 'configure timed out'
    if result.returncode != 0:
        return False, 'configure failed: ' + (result.stderr or result.stdout)[-400:]

    command = ['cmake', '--build', 'build', '-j', str(jobs)]
    command += build_config(os.path.join(project, 'build'), 'Release')
    result = run(command, cwd=project)
    if result is None:
        return False, 'build timed out'
    if result.returncode != 0:
        return False, 'build failed: ' + (result.stderr or result.stdout)[-400:]
    return True, 'built'


SERVICES = ('mtrouter', 'logcollector', 'logobserver')
NOT_PROGRAMS = ('.so', '.dll', '.dylib', '.a', '.lib', '.json', '.init', '.txt')


def binaries_of(project):
    """The application executables in build/bin.

    Shared libraries carry the executable bit too, so the extension decides, and
    the framework's own services are never the thing under test.
    """
    found = []
    for path in sorted(glob.glob(os.path.join(project, 'build', 'bin', '*'))):
        name = os.path.basename(path)
        if os.path.isdir(path) or not os.access(path, os.X_OK):
            continue
        if name.split('.')[0] in SERVICES:
            continue
        if any(ext in name for ext in NOT_PROGRAMS):
            continue
        found.append(path)
    return found


ROUTER_PORT = 8181


def service_binary(project, name):
    """A framework service built beside the project, or None.

    Suffix selection, console mode on Windows, readiness and reaping are
    run_scenarios.py's: two copies of a process lifecycle is how one gets fixed
    and the other does not.
    """
    return run_scenarios.find_service(name, [os.path.join(project, 'build', 'bin')]
                                      + SERVICE_DIRS)


def start_collector(project, database):
    """The log collector, writing to this database. Returns (handle, why-not)."""
    binary = service_binary(project, 'logcollector')
    if binary is None:
        return None, 'logcollector was not built beside the project'
    # --log=db overrides the collector's own configuration, so the database lands
    # where this check reads it.
    return run_scenarios.start_service(binary, service_ports.COLLECTOR_PORT,
                                       args=['--log=db', database], cwd=project,
                                       timeout=20.0)


def assert_collected(database, wanted):
    """The submitted application's own logs, read back off the database.

    The rows are read with the recipe's query_sqlog.py, which is the script the
    corpus hands an agent: a second reader here would be the one that goes stale.
    """
    if not os.path.isfile(database):
        return False, 'the collector wrote no {}'.format(os.path.basename(database))
    sys.path.insert(0, os.path.join(SDK, 'docs', 'agent', 'recipes',
                                    '08-observability'))
    import query_sqlog
    modules, messages = query_sqlog.modules_and_messages(database)
    named = sorted(set(modules))
    if len(named) < wanted:
        return False, ('the log database holds rows from {} process(es) {}, and the '
                       'task needs {}'.format(len(named), named, wanted))
    if not messages:
        return False, 'the log database holds no message of its own'
    return True, ''


SOURCE_SUFFIXES = ('.cpp', '.hpp', '.h', '.init', '.txt')


def declared(project, patterns):
    """Which of these patterns no submitted source carries."""
    text = []
    for root, dirs, names in os.walk(project):
        dirs[:] = [d for d in dirs if d not in ('build', '.git')]
        for name in names:
            if name.endswith(SOURCE_SUFFIXES):
                with open(os.path.join(root, name), encoding='utf-8',
                          errors='replace') as handle:
                    text.append(handle.read())
    joined = '\n'.join(text)
    return [p for p in patterns if re.search(p, joined) is None]


def produced(project, patterns):
    """Which of these globs matched no file the run left behind."""
    return [p for p in patterns
            if not glob.glob(os.path.join(project, p), recursive=True)]


def assert_artefacts(task, project):
    """The requirements a task states that its output lines do not carry."""
    missing = declared(project, task.get('declares') or [])
    if missing:
        return False, 'no source declares: ' + '; '.join(missing)
    absent = produced(project, task.get('produces') or [])
    if absent:
        return False, 'the run wrote no file matching: ' + '; '.join(absent)
    return True, ''


def run_processes(task, project, found):
    """Runs a task whose processes reach each other through the router.

    The shape is the task's own "run" block: which executable leads, which run in
    the background, and whether a router is needed. The lead is the one whose
    output is checked.
    """
    spec = task['run']

    def named(role):
        """The executable playing this role.

        A recipe compiled straight from src/ is called consumer; the same recipe
        built through CMake is called hello_consumer, because the project names its
        targets. Both answer to the role the task names.
        """
        for path in found:
            stem = os.path.basename(path).split('.')[0]
            if stem == role or stem.endswith('_' + role):
                return path
        return None

    lead = named(spec['lead'])
    if lead is None:
        return False, 'no executable called {} was built'.format(spec['lead'])

    collector, database = None, os.path.join(project, 'collected.sqlog')
    if spec.get('collects'):
        collector, why = start_collector(project, database)
        if collector is None:
            return False, why

    router = None
    if spec.get('router'):
        router_bin = service_binary(project, 'mtrouter')
        if router_bin is None:
            return False, 'mtrouter was not built beside the project'
        router, why = run_scenarios.start_service(router_bin, ROUTER_PORT,
                                                  cwd=project, timeout=20.0)
        if router is None:
            if collector is not None:
                service_ports.stop([collector], [service_ports.COLLECTOR_PORT])
            return False, why

    handles = []
    try:
        for name in spec.get('background', []):
            path = named(name)
            if path is None:
                return False, 'no executable called {} was built'.format(name)
            handles.append(subprocess.Popen([path], cwd=project,
                                            stdout=subprocess.DEVNULL,
                                            stderr=subprocess.DEVNULL))
        time.sleep(1.0)
        result = run([lead] + list(task.get('args') or []), cwd=project, timeout=90)
        if result is None:
            return False, '{} did not finish'.format(spec['lead'])
        if result.returncode != 0:
            return False, '{} exited {}'.format(spec['lead'], result.returncode)
        output = result.stdout + result.stderr
        missing = [text for text in task.get('expect', []) if text not in output]
        if missing:
            return False, 'output did not contain: ' + '; '.join(missing)
        held, why = assert_artefacts(task, project)
        if not held:
            return False, why
        if spec.get('collects'):
            # The collector writes as it receives, so the last rows need their
            # moment before it is asked to stop.
            time.sleep(2.0)
            held, why = assert_collected(database, spec['collects'])
            if not held:
                return False, why
            return True, ('built, ran through {} processes, output matched, and '
                          'their logs reached the database'.format(1 + len(handles)))
        return True, 'built, ran through {} processes, output matched'.format(
            1 + len(handles))
    finally:
        run_scenarios.stop_services(handles)
        service_ports.stop([router], [ROUTER_PORT] if router is not None else [])
        if collector is not None:
            service_ports.stop([collector], [service_ports.COLLECTOR_PORT])


def criteria_key(task):
    """What makes two tasks the same graded build.

    Several tasks answer to one recipe -- every repair task shares the recipe it is
    the repair of -- so the same (recipe, criteria) pair is graded once.
    """
    return (task.get('reference'), tuple(task.get('expect') or []),
            task.get('binaries', 1), json.dumps(task.get('run'), sort_keys=True),
            tuple(task.get('args') or []), tuple(task.get('declares') or []),
            tuple(task.get('produces') or []))


def copy_project(source, work):
    """A copy of the project to build in, so nothing is built inside the original."""
    project = os.path.join(work, os.path.basename(os.path.abspath(source)))
    shutil.copytree(source, project, ignore=shutil.ignore_patterns('build', '.git'))
    return project


def grade(task, source, sdk_root, jobs=JOBS):
    """Builds a copy of the project and grades it."""
    if not os.path.isdir(source):
        return False, 'no such directory: ' + source
    with tempfile.TemporaryDirectory(prefix='areg-eval-') as work:
        project = copy_project(source, work)
        ok, note = prepare(project, sdk_root, jobs)
        return _grade(task, project) if ok else (False, note)


def prepare(project, sdk_root, jobs=JOBS, prefix=None):
    """Checks the project has what a build needs, then builds it."""
    if not os.path.isfile(os.path.join(project, 'CMakeLists.txt')):
        return False, 'no CMakeLists.txt in ' + project

    documents = (glob.glob(os.path.join(project, '**', '*.siml'), recursive=True)
                 + glob.glob(os.path.join(project, '**', '*.fsml'), recursive=True))
    documents = [d for d in documents if os.sep + 'build' + os.sep not in d]
    if not documents:
        return False, 'the project declares no service document'
    return build(project, sdk_root, jobs, prefix)


def files_of(project):
    found = set()
    for root, dirs, names in os.walk(project):
        found.update(os.path.join(root, name) for name in dirs + names)
    return found


def restore(project, kept):
    """Removes what a graded run left in the project, so the next task starts clean."""
    for path in sorted(files_of(project) - kept, key=len, reverse=True):
        if os.path.isdir(path) and not os.path.islink(path):
            shutil.rmtree(path, ignore_errors=True)
        elif os.path.lexists(path):
            os.remove(path)


def _grade(task, project):
    """Grades a project already built."""
    found = binaries_of(project)
    wanted = task.get('binaries', 1)
    if len(found) < wanted:
        return False, 'expected {} executable(s) in build/bin, found {}'.format(
            wanted, len(found))

    if task.get('run'):
        return run_processes(task, project, found)

    expect = task.get('expect') or []
    if not expect:
        return True, 'built {} executable(s); no output asserted'.format(len(found))

    result = run([found[0]] + list(task.get('args') or []), cwd=project, timeout=90)
    if result is None:
        return False, 'the application did not finish'
    if result.returncode != 0:
        return False, 'exit code {}'.format(result.returncode)

    output = result.stdout + result.stderr
    missing = [text for text in expect if text not in output]
    if missing:
        return False, 'output did not contain: ' + '; '.join(missing)
    held, why = assert_artefacts(task, project)
    if not held:
        return False, why
    return True, 'built, ran, output matched'


def self_check(tasks, args):
    """Grades every task that names a reference recipe. The framework is built once,
    every recipe is built once and all of them at the same time, and the tasks are
    then graded one after another, as they share the router's port."""
    groups = {}
    for task in tasks:
        if task.get('reference'):
            groups.setdefault(task['reference'], []).append(task)
        else:
            print('SKIP  {:<22} no reference recipe'.format(task['id']), flush=True)
    prefix, how = None, 'the framework compiled inside each recipe'
    if not args.from_source:
        prefix, how = installed_sdk(args.sdk_root, args.sdk_build, args.jobs)
        if prefix is None:
            print('note: {}. The framework is compiled inside each recipe instead.'
                  .format(how), flush=True)
            how = 'the framework compiled inside each recipe'
        else:
            SERVICE_DIRS[:] = [os.path.join(prefix, 'tools', 'areg'),
                               os.path.join(prefix, 'bin')]
            # A Windows application built against the installation loads its DLL from bin.
            os.environ['PATH'] = os.pathsep.join(
                [os.path.join(prefix, 'bin')]
                + [part for part in [os.environ.get('PATH')] if part])
    each = max(1, args.jobs // len(groups)) + 1 if groups else args.jobs
    print('grading {} task(s) on {} recipe(s), built together at -j {} each, {}.'.format(
        sum(len(group) for group in groups.values()), len(groups), each, how), flush=True)
    with tempfile.TemporaryDirectory(prefix='areg-eval-') as work:
        projects = dict((reference, copy_project(os.path.join(RECIPES, reference), work))
                        for reference in groups)
        with ThreadPoolExecutor(max_workers=max(1, len(groups))) as pool:
            built = dict(zip(groups, pool.map(
                lambda reference: prepare(projects[reference], args.sdk_root, each, prefix),
                groups)))
        failures = 0
        for reference, group in groups.items():
            project = projects[reference]
            kept = files_of(project)
            seen = {}
            for task in group:
                key = criteria_key(task)
                if not built[reference][0]:
                    ok, note = built[reference]
                elif key in seen:
                    ok, note = seen[key][0], seen[key][1] + ' (same criteria)'
                else:
                    ok, note = _grade(task, project)
                    seen[key] = (ok, note)
                    restore(project, kept)
                print('{}  {:<22} {}'.format('PASS' if ok else 'FAIL', task['id'], note),
                      flush=True)
                failures += 0 if ok else 1
        return 1 if failures else 0


def main():
    parser = argparse.ArgumentParser(
        description='Grade an application built for an agent evaluation task.')
    parser.add_argument('--list', action='store_true', help='show the task bank')
    parser.add_argument('--task', help='the task id to grade')
    parser.add_argument('--dir', help='the project the agent produced')
    parser.add_argument('--sdk-root', default=SDK,
                        help='local SDK to build against (default: this repository)')
    parser.add_argument('--self-check', action='store_true',
                        help='grade the reference recipes instead, to verify the harness')
    parser.add_argument('--jobs', type=int, default=JOBS,
                        help='parallel compile jobs (default: {}, the processors here)'
                             .format(JOBS))
    parser.add_argument('--sdk-build', default='build',
                        help='with --self-check: the SDK build directory, relative to '
                             '--sdk-root, that is brought up to date and installed once '
                             '(default: build)')
    parser.add_argument('--from-source', action='store_true',
                        help='with --self-check: compile the framework inside each recipe '
                             'through its FetchContent block, as a project without an '
                             'installed SDK does')
    parser.add_argument('--tokens', type=int, help='tokens the agent spent, for the report')
    parser.add_argument('--hops', type=int, help='documents the agent opened, for the report')
    args = parser.parse_args()

    tasks = load_tasks()

    if args.list:
        for task in tasks:
            print('{:<18} {}'.format(task['id'], task['teaches']))
        return 0

    if args.self_check:
        return self_check(tasks, args)

    if not args.task or not args.dir:
        parser.error('give --task and --dir, or --list, or --self-check')

    task = next((t for t in tasks if t['id'] == args.task), None)
    if task is None:
        print('no such task: {}. Try --list.'.format(args.task), file=sys.stderr)
        return 1

    ok, note = grade(task, args.dir, args.sdk_root, args.jobs)
    print('{}  {}  {}'.format('PASS' if ok else 'FAIL', task['id'], note))
    if args.tokens or args.hops:
        print('      cost: {} tokens, {} documents opened'.format(
            args.tokens if args.tokens else '-', args.hops if args.hops else '-'))
    return 0 if ok else 1


if __name__ == '__main__':
    sys.exit(main())
