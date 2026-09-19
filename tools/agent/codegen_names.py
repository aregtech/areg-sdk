#!/usr/bin/env python3
"""The C++ names codegen.jar generates for a document, read from what it generated.

codegen.jar is the only authority on how a request, a response, a broadcast, an
attribute accessor, an action, a condition or a trigger is spelled. Nothing here
derives a name from a document: the document is generated into a scratch directory
and the names are read out of the headers written there.

    names = codegen_names.names_of(['src/services/Sensor.siml'], root='.')
    names[path].name('attribute', 'my_Value', 'on_update')   # on_my_value_update

Kinds and roles:
    attribute   valid, get, notify, on_update (consumer); set, invalidate, updated
                (provider) -- a .fsml attribute has get and set only
    request     call, failed
    response    call, notify
    broadcast   call, notify
    action      call
    condition   call
    trigger     call

Nothing here has a command line.
"""
import atexit
import hashlib
import json
import os
import re
import shutil
import subprocess
import tempfile
import xml.etree.ElementTree as ET

HERE = os.path.dirname(os.path.abspath(__file__))
JAR = os.path.join(os.path.dirname(HERE), 'codegen.jar')
CACHE = os.path.join(tempfile.gettempdir(), 'areg-codegen-names')
FORMAT = 5


class CodegenError(Exception):
    """codegen.jar could not be run, refused a document, or wrote no usable header."""


class Names:
    """The generated names of one document."""

    def __init__(self, document, kinds):
        self.document = document
        self.kinds = kinds

    def entries(self, kind):
        """Every element of one kind: {document name: {'names': {...}, 'params': {...}}}."""
        return self.kinds.get(kind, {})

    def has(self, kind, doc_name, role='call'):
        return role in self.entries(kind).get(doc_name, {}).get('names', {})

    def name(self, kind, doc_name, role='call'):
        """The C++ name codegen.jar gave one role of one element."""
        try:
            return self.kinds[kind][doc_name]['names'][role]
        except KeyError:
            raise CodegenError('codegen.jar generated no {} of {} "{}" for {}'
                               .format(role, kind, doc_name, self.document))

    def params(self, kind, doc_name, role='call'):
        """The parameter list of that declaration as generated, defaults removed."""
        return self.kinds.get(kind, {}).get(doc_name, {}).get('params', {}).get(role, '')

    def raw_params(self, kind, doc_name, role='call'):
        """The parameter list of that declaration as generated, defaults kept."""
        return self.kinds.get(kind, {}).get(doc_name, {}).get('raw', {}).get(role, '')

    def returns(self, kind, doc_name, role='call'):
        """The return type of that declaration as generated."""
        return self.kinds.get(kind, {}).get(doc_name, {}).get('returns', {}).get(role, '')

    def members(self):
        """Every name the generated bases declare for the document's elements."""
        found = set()
        for elements in self.kinds.values():
            for entry in elements.values():
                found.update(entry.get('all', []))
                found.update(entry['names'].values())
        return found


def project_root(document):
    """The working directory when the document is under it, else the document's folder."""
    path = os.path.abspath(document)
    here = os.getcwd()
    if os.path.commonpath([here, path]) == here:
        return here
    return os.path.dirname(path)


def names_of(documents, root=None):
    """{absolute document path: Names} for every .siml and .fsml given, one codegen run."""
    wanted = [os.path.abspath(doc) for doc in documents
              if doc.lower().endswith(('.siml', '.fsml'))]
    if not wanted:
        return {}
    root = os.path.abspath(root or project_root(wanted[0]))
    result, missing = {}, []
    for doc in wanted:
        cached = _cached(doc, root)
        if cached is None:
            missing.append(doc)
        else:
            result[doc] = cached
    if missing:
        result.update(_generate(missing, root))
    return result


def names_of_one(document, root=None):
    return names_of([document], root)[os.path.abspath(document)]


def refusal(documents, root):
    """codegen.jar's report when it refuses any of the documents, else None.

    Every document is generated in one run, into a scratch directory that is removed
    after. A run that refused nothing is remembered for these exact inputs.
    """
    root = os.path.abspath(root)
    paths = [os.path.abspath(doc) for doc in documents]
    key = '\n'.join([str(FORMAT), _stamp()]
                    + ['{} {}'.format(_relative(path, root), _digest(path)) for path in paths])
    marker = os.path.join(CACHE, hashlib.sha1(key.encode('utf-8')).hexdigest() + '.ok')
    if os.path.isfile(marker):
        return None
    target = tempfile.mkdtemp(prefix='areg-codegen-')
    try:
        listing = os.path.join(target, 'documents.txt')
        with open(listing, 'w', encoding='utf-8', newline='\n') as handle:
            handle.write('\n'.join(paths) + '\n')
        done = subprocess.run(['java', '-jar', JAR, '--root=' + root, '--docs=' + listing,
                               '--target=' + os.path.join(target, 'out')],
                              cwd=root, capture_output=True, text=True)
    except OSError as problem:
        raise CodegenError('java cannot be run ({}); the documents are checked only by '
                           'codegen.jar'.format(problem.strerror or problem))
    finally:
        shutil.rmtree(target, True)
    output = done.stdout + done.stderr
    if done.returncode != 0 or 'error[' in output:
        lines = [line.rstrip() for line in output.splitlines() if line.strip()]
        return '\n'.join(lines[-24:])
    try:
        os.makedirs(CACHE, exist_ok=True)
        open(marker, 'w').close()
    except OSError:
        pass
    return None


def split_params(text):
    """The parameters of a declaration, split on the commas outside <>, () and {}."""
    parts, depth, current = [], 0, ''
    for char in text:
        if char in '<({':
            depth += 1
        elif char in '>)}':
            depth -= 1
        if char == ',' and depth == 0:
            parts.append(current.strip())
            current = ''
        else:
            current += char
    if current.strip():
        parts.append(current.strip())
    return parts


def without_defaults(text):
    """A parameter list with every default value removed, in the generated spacing."""
    parts = [part.split('=')[0].rstrip() for part in split_params(text)]
    return (' ' + ', '.join(parts) + ' ') if parts else ''


# --------------------------------------------------------------------------------------
# Running codegen.jar
# --------------------------------------------------------------------------------------

def _stamp():
    try:
        status = os.stat(JAR)
    except OSError:
        raise CodegenError('{} is missing; the generated names come only from it'
                           .format(JAR))
    return '{}-{}'.format(status.st_size, int(status.st_mtime))


def _digest(path):
    try:
        with open(path, 'rb') as handle:
            return hashlib.sha1(handle.read()).hexdigest()
    except OSError:
        return None


def _relative(path, root):
    """A path under root spelled from root; any other path as it is."""
    path = os.path.abspath(path)
    try:
        inside = os.path.commonpath([root, path]) == root
    except ValueError:
        inside = False
    return os.path.relpath(path, root).replace(os.sep, '/') if inside else path


def _slot(doc, root):
    """The cache file of a document: the jar, where it sits in its project, its content."""
    key = '\n'.join((str(FORMAT), _stamp(), _relative(doc, root), str(_digest(doc))))
    return os.path.join(CACHE, hashlib.sha1(key.encode('utf-8')).hexdigest() + '.json')


def _cached(doc, root):
    try:
        with open(_slot(doc, root), encoding='utf-8') as handle:
            saved = json.load(handle)
    except (OSError, ValueError):
        return None
    for path, digest in saved.get('inputs', {}).items():
        if _digest(os.path.join(root, path)) != digest:
            return None
    return Names(doc, saved['kinds'])


def _store(doc, root, inputs, names):
    try:
        os.makedirs(CACHE, exist_ok=True)
        slot = _slot(doc, root)
        with open(slot + '.tmp', 'w', encoding='utf-8') as handle:
            json.dump({'inputs': dict((_relative(path, root), _digest(path))
                                      for path in inputs),
                       'kinds': names.kinds}, handle)
        os.replace(slot + '.tmp', slot)
    except OSError:
        pass


def _generate(documents, root):
    target = tempfile.mkdtemp(prefix='areg-codegen-')
    atexit.register(shutil.rmtree, target, True)
    listing = os.path.join(target, 'documents.txt')
    with open(listing, 'w', encoding='utf-8', newline='\n') as handle:
        handle.write('\n'.join(documents) + '\n')
    command = ['java', '-jar', JAR, '--root=' + root, '--docs=' + listing,
               '--target=' + os.path.join(target, 'out')]
    try:
        done = subprocess.run(command, cwd=root, capture_output=True, text=True)
    except OSError as problem:
        raise CodegenError('java cannot be run ({}); the generated names come only from '
                           'codegen.jar'.format(problem.strerror or problem))
    manifests = _manifests(os.path.join(target, 'out'))
    result = {}
    for doc in documents:
        manifest = manifests.get(_key(doc))
        if manifest is None:
            report = (done.stdout + done.stderr).strip().splitlines()
            raise CodegenError('codegen.jar generated nothing for {}:\n  {}'.format(
                doc, '\n  '.join(report[-12:]) or 'exit {}'.format(done.returncode)))
        inputs, outputs = manifest
        headers = dict((os.path.basename(path), os.path.join(target, 'out', path))
                       for path in outputs if path.endswith('.hpp'))
        if doc.lower().endswith('.siml'):
            names = Names(doc, _service(doc, headers))
        else:
            names = Names(doc, _machine(doc, headers))
        _store(doc, root, inputs, names)
        result[doc] = names
    return result


def _key(path):
    return os.path.normcase(os.path.normpath(os.path.abspath(path)))


def _manifests(out):
    """{host document: (inputs, outputs)} for every manifest codegen.jar wrote."""
    found = {}
    for folder, _, files in os.walk(out):
        for name in files:
            if not name.endswith('.files'):
                continue
            inputs, outputs = [], []
            with open(os.path.join(folder, name), encoding='utf-8') as handle:
                for line in handle:
                    line = line.strip()
                    if line.startswith('in:'):
                        inputs.append(line[3:])
                    elif line.startswith('out:'):
                        outputs.append(line[4:])
            if inputs:
                found[_key(inputs[-1])] = (inputs, outputs)
    return found


# --------------------------------------------------------------------------------------
# Reading the generated headers
# --------------------------------------------------------------------------------------

DECLARATION = re.compile(r'^\s*(?:\[\[nodiscard\]\]\s*)?(?:(?:inline|virtual|static)\s+)*'
                         r'(?P<returns>[\w:<>,&*\s]*?)\s*\b(?P<name>[A-Za-z_]\w*)\s*'
                         r'\((?P<params>.*)\)\s*(?P<tail>[^;{()]*)(?:;|\{|$)')
BANNER = re.compile(r'^\s*/{10,}\s*$')
BLOCK = re.compile(r'^\s*\*\s+(Attribute|Request|Response|Broadcast)\s+(\w+)'
                   r'(?:\s+functions)?\s*$')
SECTION = re.compile(r'^\s*//\s+(.*\S)\s*$')
ATTRIBUTE_BRIEF = re.compile(r'\\brief\s+(Returns|Sets) the attribute (\w+)\.')


def _read(headers, suffix):
    for name, path in headers.items():
        if name.endswith(suffix):
            with open(path, encoding='utf-8') as handle:
                return handle.read().splitlines()
    raise CodegenError('codegen.jar wrote no *{} header'.format(suffix))


def _declarations(lines):
    """(line index, returns, name, params, tail) of each one-line declaration."""
    for index, line in enumerate(lines):
        stripped = line.strip()
        if not stripped or stripped.startswith(('/', '*', '#')):
            continue
        match = DECLARATION.match(line)
        if match and match.group('returns').strip() not in ('', 'return'):
            yield (index, match.group('returns').strip(), match.group('name'),
                   match.group('params'), match.group('tail'))


def _entry(entries, kind, doc_name):
    return entries.setdefault(kind, {}).setdefault(
        doc_name, {'names': {}, 'params': {}, 'raw': {}, 'returns': {}, 'all': []})


def _put(entry, role, returns, name, params):
    entry['names'][role] = name
    entry['params'][role] = without_defaults(params)
    entry['raw'][role] = params.strip()
    entry['returns'][role] = returns


def _blocks(lines):
    """{(kind, document name): [declarations]} of the per-element blocks of a base."""
    blocks, current = {}, None
    ends = next((i for i, line in enumerate(lines)
                 if 'End Service Interface operations' in line), len(lines))
    wanted = dict((index, (returns, name, params, tail))
                  for index, returns, name, params, tail in _declarations(lines[:ends]))
    for index, line in enumerate(lines[:ends]):
        block = BLOCK.match(line)
        if block:
            current = (block.group(1).lower(), block.group(2))
            blocks.setdefault(current, [])
        elif BANNER.match(line):
            current = None
        elif current and index in wanted:
            blocks[current].append(wanted[index])
    return blocks


def _service(doc, headers):
    kinds = {}
    for (kind, doc_name), found in _blocks(_read(headers, 'ConsumerBase.hpp')).items():
        entry = _entry(kinds, kind, doc_name)
        for returns, name, params, tail in found:
            entry['all'].append(name)
            if kind == 'attribute':
                if 'areg::DataState & state' in params:
                    _put(entry, 'get', returns, name, params)
                elif 'areg::DataState state' in params:
                    _put(entry, 'on_update', returns, name, params)
                elif params.strip().startswith('bool notify'):
                    _put(entry, 'notify', returns, name, params)
                elif returns == 'bool' and not params.strip():
                    _put(entry, 'valid', returns, name, params)
            elif kind == 'request':
                if 'areg::ResultType reason' in params:
                    _put(entry, 'failed', returns, name, params)
                else:
                    _put(entry, 'call', returns, name, params)
            elif params.strip().startswith('bool notify'):
                _put(entry, 'notify', returns, name, params)
            else:
                _put(entry, 'call', returns, name, params)
    for (kind, doc_name), found in _blocks(_read(headers, 'ProviderBase.hpp')).items():
        if kind != 'attribute':
            continue
        entry = _entry(kinds, kind, doc_name)
        plain = []
        for returns, name, params, tail in found:
            entry['all'].append(name)
            if 'newValue' in params:
                _put(entry, 'set', returns, name, params)
            elif returns == 'void' and not params.strip():
                plain.append((returns, name, params))
        for role, declared in zip(('invalidate', 'updated'), plain):
            _put(entry, role, *declared)
    _check_counts(doc, kinds, {'attribute': './AttributeList/Attribute',
                               'request': "./MethodList/Method[@MethodType='Request']",
                               'response': "./MethodList/Method[@MethodType='Response']",
                               'broadcast': "./MethodList/Method[@MethodType='Broadcast']"})
    return kinds


def _sections(lines):
    """{section title: [declarations]} under each '// <title>' banner of a header."""
    sections, current = {}, None
    wanted = dict((index, rest) for index, *rest in _declarations(lines))
    for index, line in enumerate(lines):
        title = SECTION.match(line)
        if title and index > 0 and BANNER.match(lines[index - 1]):
            current = title.group(1)
            sections.setdefault(current, [])
        elif current and index in wanted:
            sections[current].append((index, wanted[index]))
    return sections


def _machine(doc, headers):
    root = ET.parse(doc).getroot()
    overview = root.find('Overview')
    machine = overview.get('Name') if overview is not None else ''
    methods = root.findall('./MethodList/Method')

    def listed(kind, embedded=None):
        return [method.get('Name') for method in methods
                if method.get('MethodType') == kind
                and (embedded is None
                     or (method.get('Implement') == 'Embedded') == embedded)]

    kinds = {}
    handler = _sections(_read(headers, 'ActionHandler.hpp'))
    machine_class = _sections(_read(headers, 'FSM.hpp'))
    for kind, title, doc_names, source in (
            ('action', '{} actions', listed('Action'), handler),
            ('condition', '{} conditions', listed('Condition', False), handler),
            ('trigger', '{} State Machine Triggers', listed('Trigger'), machine_class)):
        declared = [rest for _, rest in source.get(title.format(machine), [])
                    if rest[1] not in ('', machine)]
        if len(declared) != len(doc_names):
            raise CodegenError('codegen.jar generated {} {}(s) for the {} {} declares'
                               .format(len(declared), kind, len(doc_names), doc))
        for doc_name, (returns, name, params, tail) in zip(doc_names, declared):
            entry = _entry(kinds, kind, doc_name)
            entry['all'].append(name)
            _put(entry, 'call', returns, name, params)
    lines = _read(headers, 'FSM.hpp')
    pending = None
    wanted = dict((index, rest) for index, *rest in _declarations(lines))
    for index, line in enumerate(lines):
        brief = ATTRIBUTE_BRIEF.search(line)
        if brief:
            pending = ('get' if brief.group(1) == 'Returns' else 'set', brief.group(2))
        elif pending and index in wanted:
            returns, name, params, tail = wanted[index]
            entry = _entry(kinds, 'attribute', pending[1])
            entry['all'].append(name)
            _put(entry, pending[0], returns, name, params)
            pending = None
    _check_counts(doc, kinds, {'attribute': './AttributeList/Attribute'})
    return kinds


def _check_counts(doc, kinds, paths):
    """Every element the document declares has its generated names."""
    root = ET.parse(doc).getroot()
    for kind, path in paths.items():
        declared = [node.get('Name') for node in root.findall(path) if node.get('Name')]
        missing = [name for name in declared if name not in kinds.get(kind, {})]
        if missing:
            raise CodegenError('codegen.jar generated nothing for {} {} of {}'
                               .format(kind, ', '.join(missing), doc))
