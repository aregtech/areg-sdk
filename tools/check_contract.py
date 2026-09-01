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
# The interfaces that carried an IE prefix before areg was renamed, recovered the
# same way:
#   git log -M --diff-filter=R --name-status -- 'framework/*'
# Two IE names survive as private implementation detail and are deliberately not
# in this set: IEServiceEventConsumer and IETimerManagingEventConsumer. They live
# under component/private/, so no application may include them anyway -- that is
# P-07's job, and reporting them here as well would be telling an agent to rename
# something that still exists.
LEGACY_IE_NAMES = frozenset((
    'IEByteBuffer', 'IEConfigurationListener', 'IECursorPosition',
    'IEDatabaseEngine', 'IEEventConsumer', 'IEEventRouter', 'IEIOStream',
    'IELogDatabaseEngine', 'IEProxyListener', 'IEQueueListener',
    'IERemoteEventConsumer', 'IERemoteMessageHandler', 'IERemoteService',
    'IERemoteServiceConnection', 'IERemoteServiceConsumer',
    'IERemoteServiceHandler', 'IERemoteServiceMessageHandler',
    'IEServerConnectionHandler', 'IEServiceConnectionConsumer',
    'IEServiceConnectionHandler', 'IEServiceConnectionProvider',
    'IEServiceRegisterConsumer', 'IEServiceRegisterProvider', 'IESynchObject',
    'IESynchObjectBaseIX', 'IESyncObject', 'IESyncObjectBaseIX',
    'IEThreadConsumer', 'IETimerConsumer', 'IEWaitableBaseIX',
    'IEWorkerThreadConsumer',
))
IE_NAME_RE = re.compile(r'\bIE[A-Z][A-Za-z0-9]*\b')

# The camelCase accessors that became snake_case, recovered from the commit that
# performed the rename:
#   git show 94d3ab57 -- 'framework/areg/base/*.hpp' 'framework/areg/component/*.hpp'
# Only names attested by that diff, or stated in AGENTS.md, are listed. A name is
# reported only when the receiver is demonstrably an areg object (see B-07 below),
# because an application is free to call getName() on a type of its own and a
# false positive on the user's own style would make this tool untrustworthy.
LEGACY_ACCESSORS = {
    'getAddress': 'address', 'getAppName': 'app_name', 'getAt': 'at',
    'getBuffer': 'buffer', 'getCapacity': 'capacity', 'getChannel': 'channel',
    'getChecksum': 'checksum', 'getConsumer': 'consumer', 'getCookie': 'cookie',
    'getCount': 'count', 'getData': 'data', 'getDay': 'day',
    'getElements': 'elements', 'getExtension': 'extension',
    'getHandle': 'handle', 'getHours': 'hours', 'getId': 'id',
    'getInstance': 'instance', 'getKey': 'key', 'getLength': 'length',
    'getList': 'list', 'getMagic': 'magic', 'getMajor': 'major',
    'getMinor': 'minor', 'getMode': 'mode', 'getMonth': 'month',
    'getName': 'name', 'getNext': 'next', 'getNow': 'now',
    'getPatch': 'patch', 'getPath': 'path', 'getPosition': 'position',
    'getPrev': 'prev', 'getPrintable': 'printable', 'getPriority': 'priority',
    'getProxy': 'proxy', 'getResource': 'resource', 'getResult': 'result',
    'getSequence': 'sequence', 'getService': 'service', 'getSize': 'size',
    'getSource': 'source', 'getSpace': 'space', 'getStart': 'start',
    'getStop': 'stop', 'getTarget': 'target', 'getThread': 'thread',
    'getTime': 'to_time', 'getTimeout': 'timeout', 'getTimer': 'timer',
    'getType': 'type', 'getValue': 'value', 'getValues': 'values',
    'getWord': 'word', 'getYear': 'year', 'isValid': 'is_valid',
}
LEGACY_CALL_RE = re.compile(r'(\w+)\s*(?:\.|->)\s*([A-Za-z]\w*)\s*\(')
AREG_STATIC_CALL_RE = re.compile(r'\bareg::\w+::([A-Za-z]\w*)\s*\(')
# A variable whose declared type is an areg one. Reused by B-01, B-03 and B-07 so
# that all three fire only on framework objects.
AREG_DECL_RE = re.compile(
    r'\bareg::(\w+)\s*(?:<[^;{}]*>)?\s*(?:const\s*)?[&*]?\s*(\w+)\s*(?=[;={,)])')

RANGE_FOR_RE = re.compile(r'\bfor\s*\(\s*[^;()]*\b(\w+)\s*\)\s*$')
AREG_CONTAINER_DECL_RE = re.compile(
    r'\bareg::(ArrayList|HashMap|LinkedList)\s*<[^;]*>\s*(?:&\s*)?(\w+)')

# P-01. The generator stamps every file it writes. A stamped file inside the
# application's own sources means the generate target has been copied out of, and
# the copy is what gets edited.
GENERATED_BANNER = 'Created by Areg SDK code generator tool'

# B-01. The printf style log macros take a C string; areg::String is a class and
# passing it for %s is undefined behaviour that prints rubbish rather than failing.
LOG_MACRO_RE = re.compile(r'\b(?:LOG|TRACE)_(?:DBG|INFO|WARN|ERR|FATAL|SCOPE)\s*\(')

# B-03. Emptiness has its own predicate; comparing the count is slower to read and
# is the spelling an agent reaches for out of habit.
SIZE_EMPTY_RE = re.compile(r'(\w+)\s*(?:\.|->)\s*size\s*\(\s*\)\s*(==|!=|>)\s*0\b')

# P-10. A watchdog timeout is registered per thread; it does nothing unless the
# watchdog manager is running, and the manager is off by default.
WATCHDOG_MACRO_RE = re.compile(
    r'\b(BEGIN_REGISTER_THREAD_EX2?|REGISTER_WORKER_THREAD_EX2?)\s*\(')
WATCHDOG_TIMEOUT_ARG = {'BEGIN_REGISTER_THREAD_EX': 1,
                        'BEGIN_REGISTER_THREAD_EX2': 1,
                        'REGISTER_WORKER_THREAD_EX': 2,
                        'REGISTER_WORKER_THREAD_EX2': 2}
WATCHDOG_ON_RE = re.compile(r'\bstart_watchdog_manager\s*\(')
SETUP_RE = re.compile(r'\bApplication\s*::\s*setup\s*\(')

# P-11. The consumer name in the model is matched against the string the component
# answers to in worker_thread_consumer(). A mismatch is silent: the worker thread
# starts and never runs a consumer.
WORKER_MACRO_RE = re.compile(r'\b(REGISTER_WORKER_THREAD(?:_EX2?)?)\s*\(')

# B8. Any finding can be silenced on the offending line or the line above. A
# checker that cannot be quieted on a correct edge case gets switched off wholesale.
IGNORE_RE = re.compile(r'//\s*areg-check\s*:\s*ignore(?:\s+([A-Z]-\d\d(?:\s*,\s*[A-Z]-\d\d)*))?')


CHECKS = [
    ('P-01', 'advice',  'a generated file copied into the application sources'),
    ('P-02', 'error',   'a member that no service document declares'),
    ('P-03', 'error',   'REGISTER_DEPENDENCY naming no registered role'),
    ('P-04', 'error',   'a request or a subscription in a component constructor'),
    ('P-05', 'error',   'quitting on a state the framework recovers from'),
    ('P-06', 'error',   'blocking inside a handler'),
    ('P-07', 'error',   'including a header from a private/ folder'),
    ('P-08', 'error',   'throw, try or catch'),
    ('P-09', 'error',   'two components with one role name in one model'),
    ('P-10', 'advice',  'a watchdog timeout registered while the watchdog is off'),
    ('P-11', 'advice',  'a worker thread consumer name nothing answers to'),
    ('B-01', 'advice',  'an areg::String passed to a printf style log macro'),
    ('B-02', 'advice',  'a range-for over an areg container'),
    ('B-03', 'advice',  'size() == 0 instead of is_empty()'),
    ('B-04', 'advice',  'a container name or header with the obsolete TE prefix'),
    ('B-05', 'advice',  'a name with the obsolete NE or TE prefix'),
    ('B-06', 'advice',  'a name with the obsolete IE prefix'),
    ('B-07', 'advice',  'a camelCase accessor on an areg object'),
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


def split_args(text, open_paren):
    """The top level arguments of a call whose '(' is at open_paren.

    Returns a list of stripped argument strings, or None when the parentheses do
    not close on this text -- a macro split across lines is skipped rather than
    guessed at.
    """
    depth = 0
    current = []
    args = []
    for index in range(open_paren, len(text)):
        char = text[index]
        if char in '([{':
            depth += 1
            if depth == 1:
                continue
        elif char in ')]}':
            depth -= 1
            if depth == 0:
                args.append(''.join(current).strip())
                return args
        if depth == 1 and char == ',':
            args.append(''.join(current).strip())
            current = []
        else:
            current.append(char)
    return None


def suppressed(lines, number, rule):
    """B8. True when the finding is silenced on its own line or the one above.

    `// areg-check: ignore` silences every rule on the line; naming rules after it
    silences only those. The comment on the line above applies to the line below,
    which is where a macro or a long call usually needs it.
    """
    for index in (number - 1, number - 2):
        if index < 0 or index >= len(lines):
            continue
        match = IGNORE_RE.search(lines[index])
        if match is None:
            continue
        named = match.group(1)
        if named is None:
            return True
        if rule in [part.strip() for part in named.split(',')]:
            return True
    return False


def areg_variables(lines):
    """Variables in this file whose declared type belongs to areg.

    B-01, B-03 and B-07 all report a call only on one of these, so that an
    application calling getName() or size() on a type of its own is never touched.
    """
    names = {}
    for raw in lines:
        for kind, var in AREG_DECL_RE.findall(strip_noise(raw)):
            if var not in ('return', 'const', 'if', 'while', 'for'):
                names[var] = kind
    return names



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
    areg_vars = areg_variables(lines)

    for number, raw in enumerate(lines[:12]):
        if GENERATED_BANNER in raw:
            findings.append(Finding(
                'P-01', 'advice', path, number + 1,
                'this file carries the generator banner, so it is a copy of a '
                'file under the generate target. The next build rewrites the '
                'original and the copy silently stops matching it; change the '
                'service document instead'))
            break

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
                        'recalling it. One NE namespace does survive, '
                        'NEMultitargetRouterSettings in '
                        'framework/mtrouter/app/MTRouterNames.hpp, and it is '
                        'the only one' % name))
                    break

        subject = raw if stripped.startswith('#include') else line
        for name in IE_NAME_RE.findall(subject):
            if name in LEGACY_IE_NAMES:
                findings.append(Finding(
                    'B-06', 'advice', path, number + 1,
                    '"%s" lost its IE prefix when areg was renamed; the '
                    'interface is now areg::%s. The prefix is gone from the '
                    'public API, not from the framework: two IE names survive '
                    'under component/private/, which no application may include'
                    % (name, name[2:])))
                break

        for var, method in LEGACY_CALL_RE.findall(line):
            if method in LEGACY_ACCESSORS and var in areg_vars:
                findings.append(Finding(
                    'B-07', 'advice', path, number + 1,
                    '"%s" is an areg::%s; every framework method is snake_case, '
                    'so %s() is now %s()'
                    % (var, areg_vars[var], method, LEGACY_ACCESSORS[method])))
                break
        else:
            for method in AREG_STATIC_CALL_RE.findall(line):
                if method in LEGACY_ACCESSORS:
                    findings.append(Finding(
                        'B-07', 'advice', path, number + 1,
                        'every framework method is snake_case, so %s() is now '
                        '%s()' % (method, LEGACY_ACCESSORS[method])))
                    break

        if LOG_MACRO_RE.search(line) and '%s' in raw:
            for var, kind in areg_vars.items():
                if kind != 'String':
                    continue
                # Only a bare argument is wrong. A cast or a conversion call
                # spells the intent out and is how the examples pass a String.
                if re.search(r',\s*%s\s*[,)]' % re.escape(var), line):
                    findings.append(Finding(
                        'B-01', 'advice', path, number + 1,
                        '"%s" is an areg::String, not a C string; a printf style '
                        'log macro needs %s.as_string() for %%s' % (var, var)))
                    break

        for match in SIZE_EMPTY_RE.finditer(line):
            var, operator = match.group(1), match.group(2)
            if var in areg_vars:
                findings.append(Finding(
                    'B-03', 'advice', path, number + 1,
                    'emptiness has its own predicate: %s.is_empty() reads as what '
                    'it tests, where %s.size() %s 0 does not'
                    % (var, var, operator)))
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


def check_threads(sources, findings, read):
    """P-10 and P-11, both of which need the whole project to decide.

    P-10. A watchdog timeout is a per thread number in the model. It is enforced
    only while the watchdog manager runs, and the manager is off unless asked for:
    Application::setup() takes startWatchdog as its fifth argument and defaults it
    to false. A timeout registered with the manager off is a guard that never
    fires, which reads in the model as though the thread is protected.

    P-11. REGISTER_WORKER_THREAD names the consumer the component must answer to
    from worker_thread_consumer(). The base implementation returns nullptr for a
    name it does not know, so a misspelling starts the thread and runs nothing.
    The name may be a constant, so it is reported only when its exact text appears
    nowhere else in the project -- the same tolerance P-03 uses.

    The plan asked P-11 to catch an event sent to a thread name that no thread
    answers to. That is not detectable: send_event takes a DispatcherThread
    reference, never a name, so the mistake cannot be written. This is the
    adjacent defect that can.
    """
    timeouts = []
    watchdog_on = False
    consumers = []
    literals = set()

    for path in sources:
        text = read(path)
        if text is None:
            continue
        lines = text.splitlines()
        for number, raw in enumerate(lines):
            line = strip_noise(raw)

            if WATCHDOG_ON_RE.search(line):
                watchdog_on = True
            setup = SETUP_RE.search(line)
            if setup:
                args = split_args(line, line.index('(', setup.end() - 1))
                if args and len(args) >= 5 and args[4].strip() == 'true':
                    watchdog_on = True

            match = WATCHDOG_MACRO_RE.search(line)
            if match:
                args = split_args(line, match.end() - 1)
                index = WATCHDOG_TIMEOUT_ARG[match.group(1)]
                if args and len(args) > index:
                    value = args[index]
                    if value not in ('0', 'areg::WATCHDOG_IGNORE',
                                     'WATCHDOG_IGNORE', 'NECommon::WATCHDOG_IGNORE'):
                        timeouts.append((value, path, number + 1))

            worker = WORKER_MACRO_RE.search(raw)
            if worker:
                # The literal is the point of this rule, so the arguments are read
                # from the raw line, before strip_noise() blanks its strings.
                args = split_args(raw, worker.end() - 1)
                if args and len(args) >= 2:
                    name = args[1].strip()
                    if len(name) > 1 and name.startswith('"') and name.endswith('"'):
                        consumers.append((name[1:-1], path, number + 1))
            else:
                literals.update(LITERAL_RE.findall(raw))

    if timeouts and not watchdog_on:
        value, path, number = timeouts[0]
        findings.append(Finding(
            'P-10', 'advice', path, number,
            'a watchdog timeout of %s is registered but the watchdog manager is '
            'never started, so nothing enforces it. Call '
            'areg::Application::setup() with its fifth argument, startWatchdog, '
            'set to true, or register the thread with no timeout' % value))

    for name, path, number in consumers:
        if name not in literals:
            findings.append(Finding(
                'P-11', 'advice', path, number,
                'nothing in the project answers to the worker thread consumer '
                'name "%s". The component returns it from '
                'worker_thread_consumer(); a name it does not recognise yields '
                'nullptr and the worker thread runs nothing' % name))


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
    check_threads(sources, findings, read)

    findings = [f for f in findings
                if not suppressed(read(f.path).splitlines() if read(f.path) else [],
                                  f.line, f.rule)]

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
