#!/usr/bin/env python3
# Copyright (c) 2026 Aregtech
"""Execute the workflows' actual path filters with a controlled Git producer."""

import os
from pathlib import Path
import re
import subprocess
import tempfile
import unittest

from test_coverage_paths import find_bash


ROOT = Path(__file__).resolve().parents[2]
GIT = """#!/usr/bin/env bash
set -e
case "$1" in
  cat-file) exit 0 ;;
  diff)
    if [ "${DIFF_FAIL:-0}" = 1 ]; then
      echo 'fatal: fixture diff failed' >&2
      exit 128
    fi
    if [ "${RELEVANT:-1}" = 1 ]; then
      printf '%s\\n' framework/changed.cpp
    fi
    printf '%s\\n' docs/unrelated-{1..30000}.md
    ;;
  *) echo "unexpected git command: $*" >&2; exit 2 ;;
esac
"""


class PathFilterTest(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        cls.bash = find_bash()
        cls.plans = {}
        cls.cache_consumers = set()
        for path in (ROOT / '.github' / 'workflows').glob('*.yml'):
            text = path.read_text(encoding='utf-8')
            pattern = re.search(r"^\s+PATTERN: '([^']+)'$", text, re.MULTILINE)
            if not pattern:
                continue
            if 'uses: ./.github/actions/compiler-cache' in text:
                cls.cache_consumers.add(path.name)
            body = text.split('      - id: plan\n', 1)[1].split('        run: |\n', 1)[1]
            lines = []
            for line in body.splitlines():
                if not line.startswith('          '):
                    break
                lines.append(line[10:])
                if 'echo "run=${RUN}"' in line:
                    break
            script = re.sub(r'\$\{\{[^}]+\}\}', 'fixture-base', '\n'.join(lines))
            cls.plans[path.name] = (pattern.group(1), script)
        if not cls.plans:
            raise RuntimeError('No workflow path planners found')

    def run_plan(self, pattern, script, **variables):
        with tempfile.TemporaryDirectory(prefix='areg ci filter ') as temporary:
            root = Path(temporary)
            git = root / 'git'
            git.write_text(GIT, encoding='ascii', newline='\n')
            git.chmod(0o755)
            plan = root / 'plan.sh'
            plan.write_text(script + '\n', encoding='ascii', newline='\n')
            output = root / 'output'
            output.touch()
            env = dict(os.environ, EVENT='pull_request', DRAFT='false', PATTERN=pattern,
                       GITHUB_SHA='fixture-head', GITHUB_OUTPUT=output.as_posix())
            env.update(variables)
            command = ('directory="$1"; if command -v cygpath >/dev/null; then '
                       'directory="$(cygpath -u "$directory")"; fi; '
                       'export PATH="$directory:$PATH"; shift; exec bash -eo pipefail "$@"')
            result = subprocess.run(
                [self.bash, '--noprofile', '--norc', '-c', command, '--',
                 root.as_posix(), plan.as_posix()],
                env=env, cwd=ROOT, capture_output=True, text=True)
            return result, output.read_text(encoding='utf-8').strip()

    # Reproduces the defect: an early grep exit gave Git SIGPIPE and skipped relevant builds.
    def test_large_relevant_diff_runs_every_affected_workflow(self):
        for name, (pattern, script) in self.plans.items():
            with self.subTest(workflow=name):
                result, output = self.run_plan(pattern, script)
                self.assertEqual(result.returncode, 0, result.stderr)
                self.assertEqual(output, 'run=true')

    def test_unrelated_diff_skips_the_workload(self):
        for name, (pattern, script) in self.plans.items():
            with self.subTest(workflow=name):
                result, output = self.run_plan(pattern, script, RELEVANT='0')
                self.assertEqual(result.returncode, 0, result.stderr)
                self.assertEqual(output, 'run=false')

    def test_manual_runs_are_not_filtered(self):
        for name, (pattern, script) in self.plans.items():
            with self.subTest(workflow=name):
                result, output = self.run_plan(pattern, script, EVENT='workflow_dispatch',
                                               RELEVANT='0')
                self.assertEqual(result.returncode, 0, result.stderr)
                self.assertEqual(output, 'run=true')

    # Reproduces the defect: a failed diff was reported as a successful no-change decision.
    def test_failed_diff_fails_the_plan_without_a_skip_output(self):
        for name, (pattern, script) in self.plans.items():
            with self.subTest(workflow=name):
                result, output = self.run_plan(pattern, script, DIFF_FAIL='1')
                self.assertNotEqual(result.returncode, 0)
                self.assertIn('fatal: fixture diff failed', result.stderr)
                self.assertEqual(output, '')

    def test_invalid_expression_fails_the_plan(self):
        for name, (_, script) in self.plans.items():
            with self.subTest(workflow=name):
                result, output = self.run_plan('[', script)
                self.assertNotEqual(result.returncode, 0)
                self.assertTrue(result.stderr)
                self.assertEqual(output, '')

    # Reproduces the defect: root build and third-party edits did not exercise agent integrations.
    def test_agent_integrations_include_their_build_dependencies(self):
        pattern, _ = self.plans['agent-docs.yml']
        for path in ('CMakeLists.txt', 'thirdparty/sqlite3/sqlite3.c',
                     'conf/anything.cmake', 'docs/agent/README.md'):
            with self.subTest(path=path):
                self.assertIsNotNone(re.search(pattern, path))

    def test_shared_ci_changes_recheck_every_consumer(self):
        for name, (pattern, _) in self.plans.items():
            for path in ('tools/intern/ci-changes.sh', 'tests/units/test_ci_plans.py'):
                with self.subTest(workflow=name, path=path):
                    self.assertIsNotNone(re.search(pattern, path))

    def test_cache_action_changes_recheck_its_consumers(self):
        for name in self.cache_consumers:
            with self.subTest(workflow=name):
                self.assertIsNotNone(re.search(
                    self.plans[name][0], '.github/actions/compiler-cache/action.yml'))


class SanitizeScriptTest(unittest.TestCase):
    SCRIPT = ROOT / 'tools' / 'intern' / 'sanitize.sh'

    def run_script(self, *words):
        """Runs a copy of the script in a tree of its own, with the build tools stubbed."""
        with tempfile.TemporaryDirectory(prefix='areg sanitize ') as temporary:
            root = Path(temporary)
            tools = root / 'tools' / 'intern'
            stubs = root / 'stubs'
            binary = root / 'build-ubsan' / 'product' / 'bin' / 'areg-unit-tests'
            for directory in (tools, stubs, binary.parent):
                directory.mkdir(parents=True)
            copy = tools / 'sanitize.sh'
            copy.write_bytes(self.SCRIPT.read_bytes())
            record = '#!/usr/bin/env bash\nprintf \'[%s]\' "$#" "$@" >> "$RECORD"\n'
            for stub in (stubs / 'ctest', binary):
                stub.write_text(record, encoding='ascii', newline='\n')
                stub.chmod(0o755)
            for name in ('cmake', 'g++'):
                (stubs / name).write_text('#!/usr/bin/env bash\n', encoding='ascii', newline='\n')
                (stubs / name).chmod(0o755)
            recorded = root / 'recorded'
            recorded.touch()
            command = ('directory="$1"; if command -v cygpath >/dev/null; then '
                       'directory="$(cygpath -u "$directory")"; fi; '
                       'export PATH="$directory:$PATH"; shift; exec bash "$@"')
            result = subprocess.run(
                [find_bash(), '--noprofile', '--norc', '-c', command, '--',
                 stubs.as_posix(), copy.as_posix(), 'ubsan', '--keep', *words],
                env=dict(os.environ, RECORD=recorded.as_posix()),
                stdout=subprocess.PIPE, stderr=subprocess.STDOUT, text=True)
            self.assertEqual(result.returncode, 0, result.stdout)
            return recorded.read_text(encoding='ascii')

    def test_arrays_that_may_be_empty_are_expanded_for_bash_3_2(self):
        text = self.SCRIPT.read_text(encoding='utf-8')
        names = set(re.findall(r'\b(\w+)=\((?:\)|"\$\{@:)', text))
        self.assertIn('prefix', names)
        for name in names:
            with self.subTest(array=name):
                guarded = '${%s[@]+"${%s[@]}"}' % (name, name)
                self.assertNotIn('"${%s[@]' % name, text.replace(guarded, ''))

    def test_run_without_arguments_passes_none_to_the_binary(self):
        self.assertEqual(self.run_script('--run', 'auto'), '[0]')

    def test_arguments_reach_the_binary_unsplit(self):
        self.assertEqual(self.run_script('--run', 'auto', '--', 'one two', 'three'),
                         '[2][one two][three]')

    def test_ctest_gets_only_the_options_asked_for(self):
        self.assertEqual(self.run_script('--run', 'ctest'),
                         '[3][--output-on-failure][--timeout][600]')
        self.assertEqual(self.run_script('--repeat', '3', '--run', 'ctest:one'),
                         '[7][--output-on-failure][--timeout][600][-R][one]'
                         '[--repeat][until-fail:3]')


if __name__ == '__main__':
    unittest.main()
