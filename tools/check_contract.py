#!/usr/bin/env python3
"""Check an application's sources against the AREG agent contract.

The contract is `docs/agent/api.json`, the machine-readable form of the rules
stated in `AGENTS.md` section 6 and in `docs/agent/`. This tool reads that file
and reports the places where a source tree breaks one of them.

It is meant for code an agent has just written, before the first build. Most of
what it finds compiles cleanly and fails only at run time, which is exactly the
class of mistake a build does not catch.

    python3 tools/check_contract.py                 # check the current directory
    python3 tools/check_contract.py ~/myapp         # check a project
    python3 tools/check_contract.py --strict        # advisories fail too
    python3 tools/check_contract.py --list          # what is checked

Exit code 0 means no error was found. On Windows the interpreter is `python`.
"""

import argparse
import json
import os
import re
import sys
import xml.etree.ElementTree as ET

HERE = os.path.dirname(os.path.abspath(__file__))
ROOT = os.path.dirname(HERE)
DEFAULT_API = os.path.join(ROOT, 'docs', 'agent', 'api.json')

SOURCE_EXT = ('.cpp', '.cxx', '.cc', '.hpp', '.hxx', '.hh', '.h')
SKIP_DIRS = {'.git', '.svn', '.hg', 'build', 'out', 'generate', 'generated',
             'node_modules', '__pycache__', 'thirdparty'}

HANDLER_RE = re.compile(
    r'\b(request_\w+|response_\w+|broadcast_\w+|on_\w+_update|'
    r'request_\w+_failed|process_timer|service_connected)\s*\(')
LITERAL_RE = re.compile(r'"((?:[^"\\]|\\.)*)"')
OVERRIDE_RE = re.compile(
    r'\b(request_\w+|response_\w+|broadcast_\w+|on_\w+_update)\s*\('
    r'[^;]*\)\s*(?:const\s*)?(?:final|override)\b')
QUALIFIED_RE = re.compile(
    r'\b\w+::(request_\w+|response_\w+|broadcast_\w+|on_\w+_update|'
    r'process_timer)\s*\(')
GENERATED_RE = re.compile(
    r'^(request_\w+|response_\w+|broadcast_\w+|on_\w+_update)$')
ROLE_RE = re.compile(r'BEGIN_REGISTER_COMPONENT\s*\(\s*"([^"]+)"')
DEP_RE = re.compile(r'REGISTER_DEPENDENCY\s*\(\s*"([^"]+)"')
MODEL_BEGIN_RE = re.compile(r'\bBEGIN_MODEL\s*\(')
MODEL_END_RE = re.compile(r'\bEND_MODEL\s*\(')
CTOR_RE = re.compile(r'^\s*(?:explicit\s+)?(?:\w+::)?(\w+)\s*\('
                     r'[^;]*areg::ComponentEntry[^;]*\)')
BLOCKING_RE = re.compile(
    r'\b(std::this_thread::sleep_for|std::this_thread::sleep_until|'
    r'::?sleep|::?Sleep|usleep|nanosleep|\w+\.join|\w+->join|'
    r'lock_until|wait_quit)\s*\(|'
    r'\bwhile\s*\(\s*true\s*\)|\bfor\s*\(\s*;\s*;\s*\)')
NON_TERMINAL_RE = re.compile(r'\b(Disconnected|ConnectionLost|Failed)\b')
QUIT_RE = re.compile(r'\b(signal_quit|unload_model|release)\s*\(')
LEGACY_CONTAINER_RE = re.compile(r'\bTE(ArrayList|HashMap|LinkedList)\b')
# The framework names that carried an NE or TE prefix before areg was renamed,
# recovered from the rename commits themselves:
#   git log -M --diff-filter=R --name-status -- 'framework/*'
# The set is matched exactly rather than by prefix, because an application is
# free to use any coding style and may legitimately define its own NE* name.
LEGACY_NAMES = frozenset((
    'NEApplication', 'NEApplicationPosix', 'NEApplicationWin32', 'NEBasicShape',
    'NECommon', 'NEConnection', 'NEDebug', 'NEDebugPosix', 'NEDebugWin32',
    'NELogCollectorSettings', 'NELogConfig', 'NELoggerSettings', 'NELogging',
    'NELogObserverSettings', 'NELogOptions', 'NEMath', 'NEMemory',
    'NEMulticastRouterSettings', 'NEPersistence',
    'NERegistry', 'NERemoteService', 'NEService', 'NESocket', 'NESocketPosix',
    'NESocketWin32', 'NEString', 'NESynchTypesIX', 'NESyncTypesIX',
    'NESystemService', 'NETrace', 'NEUtilities', 'NEUtilitiesPosix',
    'NEUtilitiesWin32', 'TEArrayList', 'TEEvent', 'TEFixedArray', 'TEHashMap',
    'TELinkedList', 'TEMap', 'TEPair', 'TEProperty', 'TEResourceListMap',
    'TEResourceMap', 'TERingStack', 'TERuntimeResourceMap',
    'TESortedLinkedList', 'TEStack', 'TEString', 'TETemplateBase',
))
LEGACY_NAME_RE = re.compile(r'\b(?:NE|TE)[A-Z][A-Za-z0-9]*\b')
RANGE_FOR_RE = re.compile(r'\bfor\s*\(\s*[^;()]*\b(\w+)\s*\)\s*$')
AREG_CONTAINER_DECL_RE = re.compile(
    r'\bareg::(ArrayList|HashMap|LinkedList)\s*<[^;]*>\s*(?:&\s*)?(\w+)')

CHECKS = [
    ('P-02', 'error',   'a member that no service document declares'),
    ('P-03', 'error',   'REGISTER_DEPENDENCY naming no registered role'),
    ('P-04', 'error',   'a request or a subscription in a component constructor'),
    ('P-05', 'error',   'quitting on a state the framework recovers from'),
    ('P-06', 'error',   'blocking inside a handler'),
    ('P-07', 'error',   'including a header from a private/ folder'),
    ('P-08', 'error',   'throw, try or catch'),
    ('P-09', 'error',   'two components with one role name in one model'),
    ('B-02', 'advice',  'a range-for over an areg container'),
    ('B-04', 'advice',  'a container name or header with the obsolete TE prefix'),
    ('B-05', 'advice',  'a name with the obsolete NE or TE prefix'),
]


def snake(name):
    """Turn a document name into the framework's snake_case spelling."""
    out = re.sub(r'(.)([A-Z][a-z]+)', r'\1_\2', name)
    out = re.sub(r'([a-z0-9])([A-Z])', r'\1_\2', out)
    return out.replace('__', '_').lower()


class Finding(object):
    def __init__(self, rule, severity, path, line, text):
        self.rule = rule
        self.severity = severity
        self.path = path
        self.line = line
        self.text = text

    def render(self, base):
        rel = os.path.relpath(self.path, base).replace('\\', '/')
        return '%s:%d: %s %s: %s' % (rel, self.line, self.severity.upper(),
                                     self.rule, self.text)


def strip_noise(line):
    """Remove a line comment and the contents of string literals."""
    line = re.sub(r'"(?:[^"\\]|\\.)*"', '""', line)
    line = re.sub(r"'(?:[^'\\]|\\.)*'", "''", line)
    cut = line.find('//')
    return line[:cut] if cut >= 0 else line


def collect_sources(base):
    found = []
    for path, dirs, files in os.walk(base):
        dirs[:] = [d for d in dirs
                   if d not in SKIP_DIRS and not d.startswith('.')]
        for name in sorted(files):
            if name.endswith(SOURCE_EXT):
                found.append(os.path.join(path, name))
    return sorted(found)


def collect_documents(base):
    found = []
    for path, dirs, files in os.walk(base):
        dirs[:] = [d for d in dirs
                   if d not in SKIP_DIRS and not d.startswith('.')]
        for name in sorted(files):
            if name.endswith('.siml'):
                found.append(os.path.join(path, name))
    return sorted(found)


def expected_members(documents, problems):
    """Every member name the given service documents can produce."""
    names = set()
    for doc in documents:
        try:
            root = ET.parse(doc).getroot()
        except (ET.ParseError, OSError) as err:
            problems.append('%s: cannot be read: %s' % (doc, err))
            continue
        for attribute in root.iter('Attribute'):
            base = snake(attribute.get('Name', ''))
            if not base:
                continue
            names.update({
                'set_%s' % base, '%s' % base, 'is_%s_valid' % base,
                'invalidate_%s' % base, 'on_%s_update' % base,
                'notify_on_%s_update' % base,
            })
        for method in root.iter('Method'):
            base = snake(method.get('Name', ''))
            kind = method.get('MethodType', '')
            if not base:
                continue
            if kind == 'Request':
                names.update({'request_%s' % base, 'request_%s_failed' % base})
            elif kind == 'Response':
                names.update({'response_%s' % base,
                              'notify_on_response_%s' % base})
            elif kind == 'Broadcast':
                names.update({'broadcast_%s' % base,
                              'notify_on_broadcast_%s' % base})
    return names


def has_body(lines, start):
    """True when the declaration at `start` opens a brace block, not a `;`.

    A pure declaration inside a class body ends in a semicolon. Treating it as
    a definition would make the whole rest of the class look like its body.
    """
    for index in range(start, min(start + 12, len(lines))):
        clean = strip_noise(lines[index])
        if index == start:
            head = clean
            paren = head.find('(')
            clean = head[paren:] if paren >= 0 else head
        for char in clean:
            if char == '{':
                return True
            if char == ';':
                return False
    return False


def body_range(lines, start):
    """Index of the last line of the brace block opened at or after `start`."""
    depth = 0
    seen = False
    for index in range(start, len(lines)):
        clean = strip_noise(lines[index])
        for char in clean:
            if char == '{':
                depth += 1
                seen = True
            elif char == '}':
                depth -= 1
                if seen and depth <= 0:
                    return index
        if seen and depth <= 0:
            return index
    return len(lines) - 1


def check_file(path, lines, known, findings):
    in_model = False
    model_roles = {}
    handled = set()

    for number, raw in enumerate(lines):
        line = strip_noise(raw)
        stripped = line.strip()

        if MODEL_BEGIN_RE.search(line):
            in_model = True
            model_roles = {}
        elif MODEL_END_RE.search(line):
            in_model = False

        if in_model:
            for role in ROLE_RE.findall(raw):
                if role in model_roles:
                    findings.append(Finding(
                        'P-09', 'error', path, number + 1,
                        'role name "%s" is already registered in this model at '
                        'line %d; the role name is the routing identity'
                        % (role, model_roles[role])))
                else:
                    model_roles[role] = number + 1

        if stripped.startswith('#include') and '/private/' in raw:
            if 'DebugDefs.hpp' not in raw:
                findings.append(Finding(
                    'P-07', 'error', path, number + 1,
                    'a header under private/ is not API: %s' % raw.strip()))

        if re.search(r'\b(throw|try|catch)\b', line):
            findings.append(Finding(
                'P-08', 'error', path, number + 1,
                'AREG neither throws nor catches; return bool, std::optional '
                'or an error code'))

        if LEGACY_CONTAINER_RE.search(line):
            findings.append(Finding(
                'B-04', 'advice', path, number + 1,
                'the container is areg::ArrayList, areg::HashMap or '
                'areg::LinkedList, with no TE prefix'))
        else:
            # An #include keeps its quotes stripped by strip_noise, so the
            # header name is read from the raw line instead.
            subject = raw if stripped.startswith('#include') else line
            for name in LEGACY_NAME_RE.findall(subject):
                if name in LEGACY_NAMES:
                    findings.append(Finding(
                        'B-05', 'advice', path, number + 1,
                        '"%s" was removed when areg was renamed and no such '
                        'file or symbol exists. Names now live in namespace '
                        'areg, types are PascalCase and methods are '
                        'snake_case; grep for the replacement rather than '
                        'recalling it' % name))
                    break

        if known:
            match = OVERRIDE_RE.search(line) or QUALIFIED_RE.search(line)
            if match and GENERATED_RE.match(match.group(1)) \
                    and match.group(1) not in known:
                findings.append(Finding(
                    'P-02', 'error', path, number + 1,
                    '"%s" is not derivable from any .siml document in this '
                    'project; see docs/agent/20-service-interface.md'
                    % match.group(1)))

        if QUIT_RE.search(line):
            guard = [strip_noise(lines[i])
                     for i in range(max(0, number - 6), number)]
            guard = [g for g in guard
                     if 'ASSERT' not in g and 'OUTPUT_' not in g
                     and re.search(r'\b(if|else|case|switch)\b|==|!=', g)]
            window = ' '.join(guard)
            if NON_TERMINAL_RE.search(window) and 'Rejected' not in window \
                    and 'Shutdown' not in window:
                findings.append(Finding(
                    'P-05', 'error', path, number + 1,
                    'Disconnected, ConnectionLost and Failed are transient and '
                    'the framework reconnects; only Rejected and Shutdown are '
                    'terminal'))

        ctor = CTOR_RE.match(line)
        if ctor and has_body(lines, number):
            end = body_range(lines, number)
            for inner in range(number, end + 1):
                body = strip_noise(lines[inner])
                bad = re.search(r'\b(request_\w+|notify_on_\w+)\s*\(', body)
                if bad and 'ConsumerBase' not in body:
                    findings.append(Finding(
                        'P-04', 'error', path, inner + 1,
                        '"%s" runs before the service is connected; the first '
                        'legal moment is inside service_connected() once '
                        'areg::is_service_connected(status) is true'
                        % bad.group(1)))
            handled.update(range(number, end + 1))
            continue

        if number in handled:
            continue

        handler = HANDLER_RE.search(line)
        if handler and ('final' in line or 'override' in line
                        or QUALIFIED_RE.search(line)) \
                and has_body(lines, number):
            end = body_range(lines, number)
            for inner in range(number + 1, end + 1):
                body = strip_noise(lines[inner])
                hit = BLOCKING_RE.search(body)
                if hit:
                    findings.append(Finding(
                        'P-06', 'error', path, inner + 1,
                        'blocking inside "%s" stops every component on that '
                        'dispatcher thread; use a timer or another thread'
                        % handler.group(1)))
            handled.update(range(number, end + 1))

    containers = set()
    for raw in lines:
        for _, name in AREG_CONTAINER_DECL_RE.findall(strip_noise(raw)):
            containers.add(name)
    if containers:
        for number, raw in enumerate(lines):
            line = strip_noise(raw).rstrip()
            match = RANGE_FOR_RE.search(line)
            if match and ':' in line and match.group(1) in containers:
                findings.append(Finding(
                    'B-02', 'advice', path, number + 1,
                    'an areg container has no begin()/end(); iterate by index '
                    'with size() and operator[]'))


def check_roles(sources, findings, read):
    """P-03.

    A role can be registered through a constant instead of a literal, so a
    dependency is reported only when its exact text appears nowhere else in the
    project. That still catches the mistake this rule exists for, a misspelling,
    without inventing failures for code that names its roles once.
    """
    roles = set()
    literals = set()
    deps = []
    for path in sources:
        text = read(path)
        if text is None:
            continue
        roles.update(ROLE_RE.findall(text))
        for number, raw in enumerate(text.splitlines()):
            found = DEP_RE.findall(raw)
            for dep in found:
                deps.append((dep, path, number + 1))
            if not found:
                literals.update(LITERAL_RE.findall(raw))
    for dep, path, number in deps:
        if dep not in roles and dep not in literals:
            findings.append(Finding(
                'P-03', 'error', path, number,
                'no component is registered under the role name "%s"; the '
                'dependency string must equal a provider role name character '
                'for character' % dep))


def read_text(path, problems):
    for encoding in ('utf-8', 'latin-1'):
        try:
            with open(path, 'r', encoding=encoding) as handle:
                return handle.read()
        except UnicodeDecodeError:
            continue
        except OSError as err:
            problems.append('%s: cannot be read: %s' % (path, err))
            return None
    problems.append('%s: cannot be decoded' % path)
    return None


def main():
    parser = argparse.ArgumentParser(
        description='Check application sources against docs/agent/api.json.')
    parser.add_argument('path', nargs='?', default='.',
                        help='the project directory to check (default: .)')
    parser.add_argument('--api', default=DEFAULT_API,
                        help='path to api.json (default: the SDK copy)')
    parser.add_argument('--strict', action='store_true',
                        help='let an advisory fail as well')
    parser.add_argument('--list', action='store_true',
                        help='print what is checked and exit')
    args = parser.parse_args()

    if args.list:
        try:
            with open(args.api, 'r', encoding='utf-8') as handle:
                contract = json.load(handle)
        except (OSError, ValueError):
            contract = {}
        stated = {}
        for item in contract.get('prohibitions', []):
            stated[item.get('id')] = item.get('rule', '')
        for item in contract.get('base_api_notes', []):
            stated[item.get('id')] = item.get('rule', '')
        for rule, severity, summary in CHECKS:
            print('%-5s %-7s %s' % (rule, severity, summary))
            if stated.get(rule):
                print('%12s%s' % ('', stated[rule]))
        return 0

    base = os.path.abspath(args.path)
    if not os.path.isdir(base):
        print('not a directory: %s' % base, file=sys.stderr)
        return 2

    if not os.path.isfile(args.api):
        print('contract not found: %s' % args.api, file=sys.stderr)
        return 2

    problems = []
    documents = collect_documents(base)
    known = expected_members(documents, problems)
    sources = collect_sources(base)
    if not sources:
        print('no C++ source found under %s' % base, file=sys.stderr)
        return 2

    findings = []
    cache = {}

    def read(path):
        if path not in cache:
            cache[path] = read_text(path, problems)
        return cache[path]

    for path in sources:
        text = read(path)
        if text is None:
            continue
        check_file(path, text.splitlines(), known, findings)
    check_roles(sources, findings, read)

    for note in problems:
        print('note: %s' % note)

    findings.sort(key=lambda f: (f.path, f.line, f.rule))
    for finding in findings:
        print(finding.render(base))

    errors = [f for f in findings if f.severity == 'error']
    advice = [f for f in findings if f.severity == 'advice']
    print('%d file(s) and %d document(s) checked, %d error(s), %d advisory(ies)'
          % (len(sources), len(documents), len(errors), len(advice)))
    if not documents:
        print('note: no .siml document found, so P-02 was not checked')

    if errors or (args.strict and advice):
        return 1
    return 0


if __name__ == '__main__':
    sys.exit(main())
