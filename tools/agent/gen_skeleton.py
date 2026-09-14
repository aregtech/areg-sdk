#!/usr/bin/env python3
# ===========================================================================
# Writes the component sources a service interface needs: a provider with
# every request implemented, and a consumer with the handlers and the
# subscriptions. It reads the .siml document, so it never guesses a name.
#
#   python3 tools/agent/gen_skeleton.py --doc src/services/Monitor.siml --out src
#   python3 tools/agent/gen_skeleton.py --doc x.siml --out src --only provider
#
# The result compiles against the generated base classes and does nothing
# useful: every body is a place to write the logic. Existing files are kept
# unless --force is given.
#
# Exit code 0 on success, 1 on a bad argument or an unreadable document.
# ===========================================================================
import argparse
import json
import os
import re
import sys
import xml.etree.ElementTree as ET

# Predefined document types that map to a C++ type. Anything absent is a type
# the document declares, and is resolved through the declaration.
SCALARS = {
    'bool': 'bool', 'char': 'char', 'short': 'short', 'int': 'int', 'long': 'long',
    'float': 'float', 'double': 'double',
    'int8': 'int8_t', 'int16': 'int16_t', 'int32': 'int32_t', 'int64': 'int64_t',
    'uint8': 'uint8_t', 'uint16': 'uint16_t', 'uint32': 'uint32_t', 'uint64': 'uint64_t',
}
# Every predefined type the generator spells as a class, and the header it lives in.
# BinaryBuffer is areg::SharedBuffer: the document name and the C++ name differ.
CLASSES = {'String': 'areg::String', 'WideString': 'areg::WideString',
           'BinaryBuffer': 'areg::SharedBuffer', 'DateTime': 'areg::DateTime'}
CLASS_HEADERS = {'areg::String': 'areg/base/String.hpp',
                 'areg::WideString': 'areg/base/WideString.hpp',
                 'areg::SharedBuffer': 'areg/base/SharedBuffer.hpp',
                 'areg::DateTime': 'areg/base/DateTime.hpp'}


def fail(message):
    sys.stderr.write('error: {}\n'.format(message))
    sys.exit(1)


def to_snake(name):
    """StringOnChange -> string_on_change; hello_service stays as it is."""
    text = re.sub(r'(.)([A-Z][a-z]+)', r'\1_\2', name)
    text = re.sub(r'([a-z0-9])([A-Z])', r'\1_\2', text)
    return text.lower()


# Every hole a generated file leaves carries its own name, so the line is unique in
# the file and an Edit can address it without reading the file back.
def marker(slot, what, indent=8):
    """One TODO(you) line, named after the slot it fills."""
    return '{}// TODO(you) {}: {}.'.format(' ' * indent, slot, what)


# A line written only so the skeleton runs before any rule is filled in. It belongs to
# the marker above it and goes when that marker is filled: fill_markers.py drops it,
# and an Edit replaces it along with the marker line. Lines after a marker that carry
# no tag are real code and stay.
PLACEHOLDER_TAG = '   // placeholder(you)'


def placeholder(line):
    """One line that stands only until the marker above it is filled."""
    return line + PLACEHOLDER_TAG


MARKER = re.compile(r'//\s*TODO\(you\)\s+([A-Za-z_][\w]*)\s*:\s*(.*?)\s*$')

# The tool that fills every marker of a project in one command, and the file it
# reads. The generator writes that file already filled in as far as it can be:
# the sections, their order and the names each body may call.
FILLER = os.path.join(os.path.dirname(os.path.abspath(__file__)),
                      'fill_markers.py').replace(os.sep, '/')
WORKSHEET = 'bodies.txt'


# Every marker of a project is filled in one command, from a file the generator
# writes. A request costs its whole context again, so N requests of one Edit cost
# N times what one worksheet costs.
WORKSHEET_NOTE = (
    '  {total} hole(s) in {files} file(s). {path} is written beside this project:\n'
    '  one section for each, in order, with the function it sits in and every name\n'
    '  a body may call. Fill it, then give it to the filler in one command:\n'
    '\n'
    '    python3 {tool} --bodies {path}\n'
    '\n'
    '  A line tagged "// placeholder(you)" under a marker goes when that marker is\n'
    '  filled; a line with no tag is real code. gen_skeleton.py --todos lists the\n'
    '  markers left, each exactly as it stands in its file.')


# The same markers, listed by --todos after the worksheet has been consumed.
TODOS_NOTE = (
    '  {total} marker(s) in {files} file(s). Write one section per marker -- a\n'
    '  "== <marker>" line, then the code that replaces it -- and fill them all in\n'
    '  one command:\n'
    '\n'
    '    python3 {tool} --bodies {path}\n'
    '\n'
    '  A line tagged "// placeholder(you)" under a marker goes when that marker is\n'
    '  filled; a line with no tag is real code.')


# What a class already carries, so a body uses those names and declares no second one.
# A collision compiles in the header and fails on the line that reads the wrong member.
DECLARED = re.compile(r'^\s{4}(?:static\s+)?(?:constexpr\s+)?[\w:]+(?:\s*[&*])?\s+'
                      r'([mc][A-Z]\w*)\s*[;{=]')
HELPER = re.compile(r'^\s{4}(?:inline\s+)?[\w:]+(?:\s*[&*])?\s+(\w+)\s*\([^)]*\)\s*[;{]?\s*$')
RESERVED = ('if', 'while', 'for', 'switch', 'return')


def defined_names(text):
    """The members and helpers this class declares that are not overrides.

    An override is named by --contract and by the marker above its body. These are
    not: they are what the skeleton brought with it, and the only way to learn them
    today is to open the file.
    """
    members, helpers = [], []
    for line in text.splitlines():
        if ' final' in line or '= delete' in line or 'AREG_NOCOPY' in line:
            continue
        found = DECLARED.match(line)
        if found and found.group(1) not in members:
            members.append(found.group(1))
            continue
        found = HELPER.match(line)
        if found and found.group(1) not in RESERVED and found.group(1) not in helpers:
            helpers.append(found.group(1))
    return members, helpers


def print_todos(produced, out, written, holes=0, scenarios=''):
    """How many holes each generated file leaves, and where the worksheet is.

    The lines themselves are not printed here. They are sections of the worksheet,
    which is read at the moment a body is written rather than recalled from the
    output of a command that ran fifteen requests earlier.
    """
    total = 0
    files = 0
    for file_name, text in produced:
        found = [line for line in text.splitlines() if MARKER.search(line)]
        if not found:
            continue
        path = os.path.join(out, file_name).replace('\\', '/')
        print('  {} leaves {} marker(s)'.format(path, len(found)))
        total += len(found)
        files += 1
    if holes:
        print('  {} leaves {} expectation(s)'.format(scenarios, holes))
    total += holes
    files += 1 if holes else 0
    if not total:
        return
    if written is False:
        print('  {} already carries work and was left as it is.'.format(WORKSHEET))
    print(WORKSHEET_NOTE.format(total=total, files=files, tool=FILLER, path=WORKSHEET))


# ---------------------------------------------------------------------------
# The worksheet: one file carrying every open marker, the names the generated
# classes already carry, and the contract the bodies are written against. It is
# written where the markers are written, so nothing has to be recalled from an
# earlier command or looked up in a second one.
# ---------------------------------------------------------------------------
WORKSHEET_HEAD = """\
#| The worksheet of this project: one section per open marker, in file order.
#| Under each "==" line write the code that replaces that marker, then run:
#|
#|   python3 {tool} --bodies {path}
#|
#| A line starting with "#|" is furniture of this file and never reaches a source.
#| Everything else under a "==" line is code, copied as written: a comment in a
#| body is "//", not "#". A section left with no code stays open and nothing is
#| written for it, so one pass can fill what it knows and a later pass the rest.
#|
#| Filling this file and running that command is two requests. Editing the sources
#| one marker at a time is {total} requests instead, and a request is billed for the
#| whole conversation again.
#|
#| No source file has to be opened to fill this in: every name a body may call is
#| named below, every place a body belongs is a section below, and each section
#| names the function it sits in. The filler takes each section it applies out of
#| this file, so what is left here is what is left to do, and it reports the line
#| every body landed on, so no file has to be opened afterwards either.
#|
#| A helper of your own is declared in the "*_state" section of a file and defined
#| in any section of that same file. Nothing else has to be added by hand.
"""

def worksheet_sections(produced, out):
    """Every open marker as (name, hint, path, file, signature), in file order."""
    found = []
    for file_name, text in produced:
        path = os.path.join(out, file_name).replace('\\', '/')
        lines = text.splitlines()
        for index, line in enumerate(lines):
            hit = MARKER.search(line)
            if hit:
                found.append((hit.group(1), hit.group(2), path, file_name,
                              enclosing(lines, index)))
    return found


def enclosing(lines, index):
    """The signature the marker at this line sits inside, or '' when it sits in none.

    A body needs its own parameter names and its class. Reading the generated file
    back to find them is the request this line replaces. A marker in a class body
    sits in no function and gets nothing.
    """
    number = index - 1
    depth = 0
    while number >= 0:
        text = lines[number].strip()
        if text.startswith('}'):
            # A block that closed above the marker is one the marker is not in.
            depth += 1
            number -= 1
            continue
        if text == '{' and depth:
            depth -= 1
            number -= 1
            continue
        if text == '{':
            while number > 0:
                number -= 1
                head = lines[number].strip()
                if not head or head[0] in CARRIED:
                    continue
                if (head.startswith(CONTROL) or '(' not in head or
                        head.startswith('//')):
                    break
                return head
            continue
        number -= 1
    return ''


# A brace opened by one of these belongs to a step of the body, not to the body.
CONTROL = ('if', 'else', 'for', 'while', 'switch', 'do', 'try', 'catch')

# No declaration starts with one of these, so a line that does is the tail of a
# condition the generator wrapped, and the line above it is the one to read.
CARRIED = '()[]<>,&|=+-*/.?:"'


def scenario_holes(path):
    """Every unfilled expectation of a scenario file, as (marker, scenario, process).

    A hole here is not code, so it is not a line of a source file, but it is a
    decision the run has to make and the worksheet is where the decisions live.
    """
    holes = []
    try:
        with open(path, encoding='utf-8') as handle:
            document = json.load(handle)
        scenarios = document['scenarios']
    except (ValueError, OSError, KeyError, TypeError):
        return holes
    for scenario in scenarios:
        for spec in scenario.get('procs', []):
            for entry in spec.get('expect', []):
                found = MARKER.search('// ' + entry) if entry.startswith('TODO(you)') \
                    else None
                if found:
                    holes.append((found.group(1), scenario.get('name', ''),
                                  proc_label(spec)))
    return holes


def worksheet_pristine(path):
    """True when the file holds notes and empty sections only, so rewriting loses nothing."""
    try:
        with open(path, encoding='utf-8') as handle:
            lines = handle.read().splitlines()
    except (IOError, OSError):
        return False
    for line in lines:
        if not line.strip() or line.startswith('#|'):
            continue
        if line.startswith('== '):
            continue
        return False
    return True


ORDER_NOTE = ['a response and an update travel independently, so either can',
              'arrive first. Test the value already held before waiting for an',
              'update that may have been delivered already, or the wait never ends']


def section_notes(sections):
    """The warnings that belong to one section, keyed by its marker name.

    A fact stated on a page the build path does not open is a fact a run pays to
    rediscover. This one only applies where a consumer has both answers and
    updates to wait on, which is where it is written.
    """
    notes = {}
    answers = [name for name, _, _, _, _ in sections if name.startswith('response_')]
    updates = [name for name, _, _, _, _ in sections if name.startswith('update_')]
    if answers and updates:
        notes[answers[0]] = ORDER_NOTE
    return notes


def worksheet_lines(produced, out, iface, document, machine, machine_doc,
                    scenarios=None):
    """The whole worksheet, ready to write."""
    sections = worksheet_sections(produced, out)
    holes = scenario_holes(scenarios) if scenarios else []
    if not (sections or holes):
        return []
    carried = {}
    for file_name, text in produced:
        if file_name.endswith('.hpp'):
            members, helpers = defined_names(text)
            if members or helpers:
                carried[file_name] = (os.path.basename(file_name)[:-4], members, helpers)

    lines = [WORKSHEET_HEAD.format(tool=FILLER, path=WORKSHEET,
                                   total=len(sections) + len(holes))]
    if carried:
        lines.append('#| These names are taken already. Declaring one of them again in\n'
                     '#| a section below shadows it, and the compiler points at the line\n'
                     '#| that reads the wrong one, not at the declaration:\n#|')
        for _, (cls, members, helpers) in sorted(carried.items()):
            parts = []
            if members:
                parts.append('members ' + ', '.join(members))
            if helpers:
                parts.append('helpers ' + ', '.join(h + '()' for h in helpers))
            lines.append('#|   {}: {}'.format(cls, '; '.join(parts)))
        lines.append('#|')
    lines.append('#| The names these bodies may call, spelt as the generator emits them.\n'
                 '#| A name spelt in another namespace than the one below does not\n'
                 '#| compile:\n#|')
    for spec, doc in ((iface, document), (machine, machine_doc)):
        if spec is None:
            continue
        for line in contract_lines(spec, doc):
            lines.append(('#| ' + line).rstrip())
    lines.append('#|')

    notes = section_notes(sections)
    current = None
    for name, hint, path, file_name, signature in sections:
        if path != current:
            lines.append('\n#| ---- {}'.format(path))
            current = path
        lines.append('== {}'.format(name))
        lines.append('#| {}'.format(hint))
        if signature:
            lines.append('#| in: {}'.format(signature))
        for line in notes.get(name, []):
            lines.append('#| {}'.format(line))
        lines.append('')
    if holes:
        lines.append('\n#| ---- {}: what a run has to print to prove a requirement.'
                     .format(scenarios))
        lines.append('#| One regular expression per line, and every one of them has to')
        lines.append('#| match. The generated main() prints nothing, so each line comes')
        lines.append('#| from a body above: the expectation and the code that satisfies')
        lines.append('#| it are written together, in this file, or the run proves')
        lines.append('#| nothing.')
        for name, scenario, process in holes:
            lines.append('== {}'.format(name))
            lines.append('#| what "{}" must print in scenario "{}".'
                         .format(process, scenario))
            lines.append('')
    return lines


def write_worksheet(produced, out, iface, document, machine, machine_doc,
                    scenarios=None):
    """Write the worksheet, unless one already carries work."""
    lines = worksheet_lines(produced, out, iface, document, machine, machine_doc,
                            scenarios)
    if not lines:
        return None
    if os.path.exists(WORKSHEET) and not worksheet_pristine(WORKSHEET):
        return False
    with open(WORKSHEET, 'w', encoding='utf-8', newline='\n') as handle:
        handle.write('\n'.join(lines).rstrip() + '\n')
    return True


class Interface:
    """The parts of a .siml document the components have to match."""

    def __init__(self, path):
        try:
            root = ET.parse(path).getroot()
        except (ET.ParseError, OSError) as error:
            fail('cannot read {}: {}'.format(path, error))

        overview = root.find('Overview')
        if overview is None or not overview.get('Name'):
            fail('the document has no Overview/@Name')
        self.name = overview.get('Name')

        # A declared type is passed by value when it is an enumeration, and by
        # const reference otherwise.
        self.by_value = set()
        self.declared = set()
        self.imported = set()
        # The types this document declares, in document order, with their kind.
        self.types = []
        # The same, for every document this one includes: (document, space, name, kind).
        self.imported_types = []
        for declared in root.findall('./DataTypeList/DataType'):
            kind = (declared.get('Type') or '').lower()
            type_name = declared.get('Name')
            if not type_name:
                continue
            self.declared.add(type_name)
            self.types.append((type_name, kind))
            if kind in ('enumeration', 'enumerate'):
                self.by_value.add(type_name)

        self.constants = []
        for constant in root.findall('./ConstantList/Constant'):
            if constant.get('Name'):
                self.constants.append((constant.get('Name'),
                                       constant.get('DataType')))

        self._read_included(root, path)

        self.requests = []
        self.responses = []
        self.broadcasts = []
        # A .fsml declares its methods in the same list, under two other kinds.
        self.actions = []
        self.triggers = []
        # A Condition is a third .fsml kind: a guard calls it, it carries its own
        # return type, and its generated name has no prefix.
        self.conditions = []
        for method in root.findall('./MethodList/Method'):
            kind = (method.get('MethodType') or '').lower()
            entry = (method.get('Name'), self._params(method))
            if kind == 'condition':
                self.conditions.append((method.get('Name'), self._params(method),
                                        method.get('Return') or 'bool'))
                continue
            if kind == 'request':
                self.requests.append(entry)
            elif kind == 'response':
                self.responses.append(entry)
            elif kind == 'broadcast':
                self.broadcasts.append(entry)
            elif kind == 'action':
                self.actions.append(entry)
            elif kind == 'trigger':
                self.triggers.append(entry)

        self.attributes = []
        for attribute in root.findall('./AttributeList/Attribute'):
            self.attributes.append((attribute.get('Name'), attribute.get('DataType')))

    def _params(self, method):
        result = []
        for param in method.findall('./ParamList/Parameter'):
            result.append((param.get('Name'), param.get('DataType')))
        return result

    def _read_included(self, root, path):
        """Registers the types an included document declares.

        An enumeration is passed by value wherever it was declared, so a document
        that only imports one still has to know it is an enumeration. Without this
        the skeleton writes a const reference, the generated base declares the same
        method by value, and the override is refused as not virtual.
        """
        folder = os.path.dirname(os.path.abspath(path))
        for location in root.findall('./IncludeList/Location'):
            name = location.get('Name')
            if not name:
                continue
            for candidate in (os.path.join(folder, os.path.basename(name)),
                              os.path.join(folder, name),
                              os.path.abspath(name)):
                if os.path.isfile(candidate):
                    self._read_types_of(candidate)
                    break

    def _read_types_of(self, path):
        """The declarations of an included document, qualified by its namespace."""
        try:
            included = ET.parse(path).getroot()
        except (ET.ParseError, OSError):
            return
        overview = included.find('./Overview')
        space = overview.get('Name') if overview is not None else ''
        for declared in included.findall('./DataTypeList/DataType'):
            type_name = declared.get('Name')
            if not type_name:
                continue
            kind = (declared.get('Type') or '').lower()
            self.imported_types.append((os.path.basename(path), space, type_name, kind))
            spellings = [type_name]
            if space:
                spellings.append('{}::{}'.format(space, type_name))
            for spelling in spellings:
                self.imported.add(spelling)
                if kind in ('enumeration', 'enumerate'):
                    self.by_value.add(spelling)

    def cpp_type(self, type_name):
        """The C++ spelling of a document type, and how it is passed.

        The generator passes a primitive and an enumeration by value and
        everything else by const reference: String, BinaryBuffer, Structure,
        Container and Imported. An override that disagrees is refused as not
        virtual, so an enumeration declared in an included document has to be
        known here as one.
        """
        if type_name in SCALARS:
            return SCALARS[type_name], False
        if type_name in CLASSES:
            return CLASSES[type_name], True
        if type_name in self.declared:
            qualified = '{}::{}'.format(self.name, type_name)
            return qualified, type_name not in self.by_value
        # A type from an included document already carries its namespace. An
        # enumeration among them is a primitive and is passed by value.
        return type_name, ('::' in type_name) and (type_name not in self.by_value)

    def signature(self, params):
        if not params:
            return ''
        parts = []
        for param_name, type_name in params:
            cpp, by_ref = self.cpp_type(type_name)
            parts.append('const {} & {}'.format(cpp, param_name) if by_ref
                         else '{} {}'.format(cpp, param_name))
        return ' ' + ', '.join(parts) + ' '

    def passed_as(self, type_name):
        """How a value of this type appears in a generated parameter list.

        A primitive and an enumeration are passed by value, everything else by
        const reference. This is the method-parameter rule, and an override that
        disagrees with it is refused as not virtual.
        """
        cpp, by_ref = self.cpp_type(type_name)
        return 'const {} &'.format(cpp) if by_ref else cpp

    def attribute_setter(self, type_name, machine):
        """How a generated attribute setter takes its value.

        The two generators differ here and neither follows the parameter rule
        above: a .siml provider base takes every attribute by const reference,
        a scalar included, while a .fsml machine applies the parameter rule.
        """
        if machine:
            return self.passed_as(type_name)
        return 'const {} &'.format(self.cpp_type(type_name)[0])

    def call_args(self, params):
        return ', '.join(name for name, _ in params)


def provider_files(iface, class_name, include_root):
    header = ['/**',
              ' * \\file    {}.hpp'.format(class_name),
              ' * \\brief   Provider of the {} service.'.format(iface.name),
              ' **/',
              '#ifndef {}_HPP'.format(class_name.upper()),
              '#define {}_HPP'.format(class_name.upper()),
              '',
              '#include "areg/base/areg_global.h"',
              '#include "areg/component/Component.hpp"',
              '#include "{}/{}ProviderBase.hpp"'.format(include_root, iface.name),
              '',
              'class {} final : public    areg::Component'.format(class_name),
              '{}, protected {}ProviderBase'.format(' ' * (len(class_name) + 13), iface.name),
              '{',
              'public:',
              '    {}(const areg::ComponentEntry & entry, areg::ComponentThread & owner);'.format(class_name),
              '',
              'protected:']
    for name, params in iface.requests:
        header.append('    //!< Implements the {} request.'.format(name))
        header.append('    void request_{}({}) final;'.format(to_snake(name), iface.signature(params)))
    header += ['',
               'private:',
               '    inline {} & self()'.format(class_name),
               '    {   return (*this); }',
               '',
               '    {}() = delete;'.format(class_name),
               '    AREG_NOCOPY_NOMOVE({});'.format(class_name),
               '};',
               '',
               '#endif // {}_HPP'.format(class_name.upper()),
               '']

    source = ['/**',
              ' * \\file    {}.cpp'.format(class_name),
              ' * \\brief   Provider of the {} service.'.format(iface.name),
              ' **/',
              '#include "{}.hpp"'.format(class_name),
              '',
              '#include "areg/component/ComponentThread.hpp"',
              '',
              '{}::{}(const areg::ComponentEntry & entry, areg::ComponentThread & owner)'.format(class_name, class_name),
              '    : areg::Component(entry, owner)',
              '    , {}ProviderBase(static_cast<areg::Component &>(self()))'.format(iface.name),
              '{']
    if iface.attributes:
        source.append('    // An attribute is invalid until it is set once.')
        for attr_name, type_name in iface.attributes:
            source.append('    // set_{}( value );'.format(to_snake(attr_name)))
    source += ['}', '']

    responses = {name for name, _ in iface.responses}
    for name, params in iface.requests:
        source.append('void {}::request_{}({})'.format(class_name, to_snake(name), iface.signature(params)))
        source.append('{')
        source.append('    // TODO: implement the request.')
        if name in responses:
            answer = next(p for n, p in iface.responses if n == name)
            if answer:
                # The answer carries values this stub cannot invent, so the call
                # is written out with its types for the implementation to fill.
                source.append('    // Answer with:')
                source.append('    // response_{}({});'.format(
                    to_snake(name), iface.signature(answer).strip()))
            else:
                source.append('    response_{}();'.format(to_snake(name)))
        source.append('}')
        source.append('')
    return '\n'.join(header), '\n'.join(source)


def consumer_files(iface, class_name, include_root):
    header = ['/**',
              ' * \\file    {}.hpp'.format(class_name),
              ' * \\brief   Consumer of the {} service.'.format(iface.name),
              ' **/',
              '#ifndef {}_HPP'.format(class_name.upper()),
              '#define {}_HPP'.format(class_name.upper()),
              '',
              '#include "areg/base/areg_global.h"',
              '#include "areg/component/Component.hpp"',
              '#include "{}/{}ConsumerBase.hpp"'.format(include_root, iface.name),
              '',
              'class {} final : public    areg::Component'.format(class_name),
              '{}, protected {}ConsumerBase'.format(' ' * (len(class_name) + 13), iface.name),
              '{',
              'public:',
              '    {}(const areg::ComponentEntry & entry, areg::ComponentThread & owner);'.format(class_name),
              '',
              'protected:',
              '    //!< Called on every connection change. The first place a request may be sent.',
              '    bool service_connected(areg::ServiceConnectionState status, areg::ProxyBase & proxy) final;']
    for name, params in iface.responses:
        header.append('    //!< Answer of the {} request.'.format(name))
        header.append('    void response_{}({}) final;'.format(to_snake(name), iface.signature(params)))
    for name, params in iface.requests:
        header.append('    //!< The {} request could not be executed.'.format(name))
        header.append('    void request_{}_failed(areg::ResultType reason) final;'.format(to_snake(name)))
    for name, params in iface.broadcasts:
        header.append('    //!< Broadcast {}.'.format(name))
        header.append('    void broadcast_{}({}) final;'.format(to_snake(name), iface.signature(params)))
    for attr_name, type_name in iface.attributes:
        cpp, by_ref = iface.cpp_type(type_name)
        param = 'const {} & {}'.format(cpp, attr_name) if by_ref else '{} {}'.format(cpp, attr_name)
        header.append('    //!< New value of the {} attribute.'.format(attr_name))
        header.append('    void on_{}_update({}, areg::DataState state) final;'.format(to_snake(attr_name), param))
    header += ['',
               'private:',
               '    {}() = delete;'.format(class_name),
               '    AREG_NOCOPY_NOMOVE({});'.format(class_name),
               '};',
               '',
               '#endif // {}_HPP'.format(class_name.upper()),
               '']

    source = ['/**',
              ' * \\file    {}.cpp'.format(class_name),
              ' * \\brief   Consumer of the {} service.'.format(iface.name),
              ' **/',
              '#include "{}.hpp"'.format(class_name),
              '',
              '#include "areg/appbase/Application.hpp"',
              '#include "areg/component/ComponentThread.hpp"',
              '',
              '{}::{}(const areg::ComponentEntry & entry, areg::ComponentThread & owner)'.format(class_name, class_name),
              '    : areg::Component(entry, owner)',
              '    , {}ConsumerBase(entry.mDependencyServices[0].mRoleName, owner)'.format(iface.name),
              '{',
              '}',
              '',
              'bool {}::service_connected(areg::ServiceConnectionState status, areg::ProxyBase & proxy)'.format(class_name),
              '{',
              '    bool result{ false };',
              '    if ({}ConsumerBase::service_connected(status, proxy))'.format(iface.name),
              '    {',
              '        result = true;',
              '        if (areg::is_service_connected(status))',
              '        {']
    if iface.attributes or iface.broadcasts:
        source.append('            // Subscriptions are made here, and again after every reconnection.')
    for attr_name, _ in iface.attributes:
        source.append('            notify_on_{}_update(true);'.format(to_snake(attr_name)))
    for name, _ in iface.broadcasts:
        source.append('            notify_on_broadcast_{}(true);'.format(to_snake(name)))
    source += ['            // TODO: send the first request here.',
               '        }',
               '    }',
               '',
               '    return result;',
               '}',
               '']

    for name, params in iface.responses:
        source += ['void {}::response_{}({})'.format(class_name, to_snake(name), iface.signature(params)),
                   '{', '    // TODO: handle the answer.', '}', '']
    for name, _ in iface.requests:
        source += ['void {}::request_{}_failed(areg::ResultType /*reason*/)'.format(class_name, to_snake(name)),
                   '{', '    // TODO: retry when is_connected(), or report.', '}', '']
    for name, params in iface.broadcasts:
        source += ['void {}::broadcast_{}({})'.format(class_name, to_snake(name), iface.signature(params)),
                   '{', '    // TODO: handle the broadcast.', '}', '']
    for attr_name, type_name in iface.attributes:
        cpp, by_ref = iface.cpp_type(type_name)
        param = 'const {} & {}'.format(cpp, attr_name) if by_ref else '{} {}'.format(cpp, attr_name)
        source += ['void {}::on_{}_update({}, areg::DataState state)'.format(class_name, to_snake(attr_name), param),
                   '{',
                   '    if (state == areg::DataState::DataIsOK)',
                   '    {',
                   '        // TODO: use the new value.',
                   '    }',
                   '}',
                   '']
    return '\n'.join(header), '\n'.join(source)


def machine_files(iface, class_name, include_root):
    """The component that hosts a state machine and implements its actions."""
    guard = class_name.upper() + '_HPP'
    header = ['/**',
              ' * \\file    {}.hpp'.format(class_name),
              ' * \\brief   Host of the {} state machine.'.format(iface.name),
              ' **/',
              '#ifndef {}'.format(guard),
              '#define {}'.format(guard),
              '',
              '#include "areg/base/areg_global.h"',
              '#include "areg/component/Component.hpp"',
              '#include "{}/{}ActionHandler.hpp"'.format(include_root, iface.name),
              '#include "{}/{}FSM.hpp"'.format(include_root, iface.name),
              '',
              'class {} final : public    areg::Component'.format(class_name),
              '{}, protected {}ActionHandler'.format(' ' * (len(class_name) + 13), iface.name),
              '{',
              'public:',
              '    {}(const areg::ComponentEntry & entry, areg::ComponentThread & owner);'.format(class_name),
              '',
              'protected:',
              '    void startup_component(areg::ComponentThread & comThread) final;',
              '',
              '    void shutdown_component(areg::ComponentThread & comThread) final;',
              '']
    for name, params in iface.actions:
        header.append('    //!< Runs the {} action of the machine.'.format(name))
        header.append('    void action_{}({}) final;'.format(name, iface.signature(params)))
    header += ['',
               'private:',
               '    inline {} & self()'.format(class_name),
               '    {   return (*this); }',
               '',
               '    {}FSM  mFsm;    //!< The state machine.'.format(iface.name),
               '',
               '    {}() = delete;'.format(class_name),
               '    AREG_NOCOPY_NOMOVE({});'.format(class_name),
               '};',
               '',
               '#endif // {}'.format(guard),
               '']

    source = ['/**',
              ' * \\file    {}.cpp'.format(class_name),
              ' * \\brief   Host of the {} state machine.'.format(iface.name),
              ' **/',
              '#include "{}.hpp"'.format(class_name),
              '',
              '#include "areg/component/ComponentThread.hpp"',
              '',
              '{}::{}(const areg::ComponentEntry & entry, areg::ComponentThread & owner)'.format(class_name, class_name),
              '    : areg::Component(entry, owner)',
              '    , {}ActionHandler()'.format(iface.name),
              '    , mFsm(static_cast<{}ActionHandler &>(self()))'.format(iface.name),
              '{',
              '}',
              '',
              'void {}::startup_component(areg::ComponentThread & comThread)'.format(class_name),
              '{',
              '    areg::Component::startup_component(comThread);',
              '    mFsm.init_fsm(&comThread);',
              '}',
              '',
              'void {}::shutdown_component(areg::ComponentThread & comThread)'.format(class_name),
              '{',
              '    mFsm.release_fsm();',
              '    areg::Component::shutdown_component(comThread);',
              '}',
              '']
    for name, params in iface.actions:
        source += ['void {}::action_{}({})'.format(class_name, name, iface.signature(params)),
                   '{',
                   '    // TODO: perform the effect. Never raise a stimulus from here.',
                   '}',
                   '']
    if iface.triggers:
        source += ['// Stimulus the machine accepts, to be called from a request handler:',
                   '//   ' + '  '.join('mFsm.{}();'.format(name) for name, _ in iface.triggers),
                   '']

    return '\n'.join(header), '\n'.join(source)


DECLARATION = re.compile(r'^\s{4}(?!//)([A-Za-z_].*\(.*\).*;)\s*$')


def write(path, text, force):
    """Writes the file or keeps the one that is there. Returns what is on disk.

    A kept file returns its own content, never the text that was proposed for it.
    Every report downstream -- the markers, the worksheet -- is derived from what
    the return value holds, and a report of sections no file contains is filled
    into nothing.
    """
    if os.path.exists(path) and not force:
        print('kept   {}'.format(path))
        try:
            with open(path, encoding='utf-8', errors='replace') as handle:
                return handle.read()
        except OSError:
            return text
    folder = os.path.dirname(path)
    if folder:
        os.makedirs(folder, exist_ok=True)
    with open(path, 'w', encoding='utf-8') as handle:
        handle.write(text)
    print('wrote  {}'.format(path))
    return text


def print_index(path, text):
    """List the declarations a header carries, so the file needs no second read."""
    found = [DECLARATION.match(line).group(1)
             for line in text.splitlines() if DECLARATION.match(line)]
    # The deleted constructor and the copy guard are boilerplate, not work.
    found = [d for d in found if '= delete' not in d and 'AREG_NOCOPY' not in d]
    if not found:
        return
    print('  {} declares:'.format(os.path.basename(path)))
    for line in found:
        print('    {}'.format(line))


BODY_NOTE = ('  Every declaration above is already overridden in the .cpp with an empty\n'
             '  body: open the .cpp and fill them in. The .hpp needs no second read,\n'
             '  and no name on a generated base class is ever invented.\n'
             '  Include these from your own sources by name alone, for example\n'
             '  #include "{}". Only generated headers take a src/ prefix: they are\n'
             '  written into a mirror tree, and your own files are not in it.')

HOST_NOTE = ('  {name} is a standalone component that owns the machine. Delete both\n'
             '  files and hold the machine as a member of the provider instead when the\n'
             '  provider is what drives it.')


def contract_lines(iface, document):
    """The names a document generates, and nothing about how it was written.

    This is what a caller needs from whoever authored the document: the class to
    build against, what to override and what to call. It is derived from the
    document, so it cannot disagree with what the generator emits.
    """
    out = ['document:  {}'.format(document),
           'interface: {}'.format(iface.name)]
    if document.lower().endswith('.fsml'):
        out.append('classes:   {n}FSM (the machine), {n}ActionHandler (implement this)'
                   .format(n=iface.name))
        # The generated names are the document's own, unchanged: a trigger keeps its
        # name, an action carries the action_ prefix, a condition carries none.
        for name, params in iface.triggers:
            out.append('  call     bool {}({})'.format(name, iface.signature(params)))
        for name, params in iface.actions:
            out.append('  override void action_{}({})'
                       .format(name, iface.signature(params)))
        for name, params, returns in iface.conditions:
            out.append('  override {} {}({})'.format(iface.cpp_type(returns)[0], name,
                                                     iface.signature(params)))
        for name, kind in iface.attributes:
            spelled = to_snake(name)
            out.append('  on the machine object: {}() / set_{}({})'
                       .format(spelled, spelled, iface.attribute_setter(kind, True)))
        return out + type_lines(iface)
    if document.lower().endswith('.dtml'):
        out.append('classes:   none. {} is the namespace the types below are spelled in'
                   .format(iface.name))
        return out + type_lines(iface)
    out.append('classes:   {n}Provider and {n}Consumer build on the generated {n} base'
               .format(n=iface.name))
    for name, params in iface.requests:
        out.append('  provider overrides request_{}({}); consumer calls request_{}(...) '
                   'to send it'.format(to_snake(name), iface.signature(params),
                                       to_snake(name)))
    for name, params in iface.responses:
        out.append('  provider calls response_{}({}); consumer overrides it'
                   .format(to_snake(name), iface.signature(params)))
    for name, params in iface.broadcasts:
        out.append('  provider calls broadcast_{}({}); consumer subscribes with '
                   'notify_on_broadcast_{}(true)'
                   .format(to_snake(name), iface.signature(params), to_snake(name)))
    for name, kind in iface.attributes:
        spelled = to_snake(name)
        cpp = iface.cpp_type(kind)[0]
        read = iface.passed_as(kind)
        out.append('  provider calls set_{}({}); consumer subscribes with '
                   'notify_on_{}_update(true)'
                   .format(spelled, iface.attribute_setter(kind, False), spelled))
        # The last value is readable at any time, on both sides, and the two
        # readers do not have the same signature. Reading the generated header to
        # find that out is what this line replaces.
        out.append('  provider reads {t} & {n}(); consumer reads {r} {n}('
                   'areg::DataState & state)'
                   .format(t=cpp, r=read, n=spelled))
    return out + type_lines(iface)


def print_contract(iface, document):
    """The contract of one document, on standard output."""
    for line in contract_lines(iface, document):
        print(line)
    return 0


def type_lines(iface):
    """The data types the signatures above are written in, declared here or included.

    A type an included document declares is spelled in that document's namespace and
    is named by every signature above, so the contract states it here rather than
    leaving the reader to open the included document or the generated header.
    """
    out = []
    for path, space, type_name, kind in iface.imported_types:
        out.append(type_line(space, type_name, kind, ' (from {})'.format(path)))
    for type_name, kind in iface.types:
        out.append(type_line(iface.name, type_name, kind, ''))
    for name, kind in iface.constants:
        out.append('  constant {}::{} of type {}'.format(iface.name, name, kind))
    return out


def type_line(space, type_name, kind, origin):
    """One declared type: its C++ spelling, and what the generator gives it."""
    scope = '{}::'.format(space) if space else ''
    full = scope + type_name
    if kind in ('enumeration', 'enumerate'):
        return ('  enum class {}, with const char * {}as_string({} value){}'
                .format(full, scope, full, origin))
    if kind == 'structure':
        return '  struct {}, with a field-by-field == and <<{}'.format(full, origin)
    if kind == 'imported':
        return '  {} names the type your own header declares{}'.format(full, origin)
    return '  {} is an alias to an areg container{}'.format(full, origin)


# The value a generated call passes until the rule that computes it is written.
# Every spelling below is valid C++17 for its type, so the program compiles and
# runs as generated.
def default_expr(iface, type_name):
    """A value-initialised expression of this document type."""
    if type_name == 'bool':
        return 'false'
    if type_name in SCALARS:
        return '0' if 'float' not in SCALARS[type_name] and \
                      'double' not in SCALARS[type_name] else '0.0'
    return '{}{{}}'.format(iface.cpp_type(type_name)[0])


# The headers a stepping timer needs, added only to the file that carries one.
TIMER_INCLUDES = ['#include "areg/component/Timer.hpp"',
                  '#include "areg/component/TimerConsumer.hpp"']

def provider_class(iface, cls, machine=None):
    """The provider component, with every request answered.

    Given a machine, the same component owns it: the action handler is a base, the
    machine is a member, and every action is declared here. A separate host component
    is what the pair used to need, and merging them is what removes it.
    """
    pad = ' ' * (len(cls) + 13)
    lines = ['class {} final : public    areg::Component'.format(cls),
             '{}, protected {}ProviderBase'.format(pad, iface.name)]
    if machine:
        lines.append('{}, protected {}ActionHandler'.format(pad, machine.name))
    lines += ['{',
              'public:',
              '    {}(const areg::ComponentEntry & entry, areg::ComponentThread & owner)'.format(cls),
              '        : areg::Component(entry, owner)',
              '        , {}ProviderBase(static_cast<areg::Component &>(self()))'.format(iface.name)]
    if machine:
        lines.append('        , {}ActionHandler()'.format(machine.name))
        lines.append('        , mFsm(static_cast<{}ActionHandler &>(self()))'.format(machine.name))
    lines.append('    {')
    if iface.attributes:
        lines.append('        // An attribute is invalid until it is set once.')
        for attr_name, type_name in iface.attributes:
            lines.append('        set_{}({});'.format(to_snake(attr_name),
                                                      default_expr(iface, type_name)))
    lines += ['    }', '', 'protected:']

    if machine:
        lines += ['    void startup_component(areg::ComponentThread & comThread) final',
                  '    {',
                  '        areg::Component::startup_component(comThread);',
                  '        mFsm.init_fsm(&comThread);',
                  '    }',
                  '',
                  '    void shutdown_component(areg::ComponentThread & comThread) final',
                  '    {',
                  '        mFsm.release_fsm();',
                  '        areg::Component::shutdown_component(comThread);',
                  '    }',
                  '']

    answered = dict((name, params) for name, params in iface.responses)
    if machine and machine.triggers:
        lines.append('    // A request handler converts the call into a stimulus and '
                     'decides nothing:')
        for name, params in machine.triggers:
            lines.append('    //   mFsm.{}({});'.format(
                name, ', '.join(pname for pname, _ in params)))
    for name, params in iface.requests:
        lines.append('    void request_{}({}) final'.format(to_snake(name),
                                                            iface.signature(params)))
        lines.append('    {')
        lines.append(marker('request_' + to_snake(name),
                            'the rule this request carries out'))
        if name in answered:
            args = ', '.join(default_expr(iface, t) for _, t in answered[name])
            lines.append(placeholder('        response_{}({});'
                                     .format(to_snake(name), args)))
        lines.append('    }')
        lines.append('')

    if machine and machine.conditions:
        lines.append('    // Every condition a guard of the machine asks. Answer it and '
                     'change nothing.')
        for name, params, returns in machine.conditions:
            lines.append('    {} {}({}) final'.format(machine.cpp_type(returns)[0], name,
                                                      machine.signature(params)))
            lines += ['    {',
                      marker('condition_' + name,
                             'answer the question this guard asks'),
                      placeholder('        return {};'
                                  .format(default_expr(machine, returns))),
                      '    }',
                      '']

    if machine:
        lines.append('    // Every action the machine performs. Never raise a stimulus '
                     'from one.')
        for name, params in machine.actions:
            lines.append('    void action_{}({}) final'.format(name,
                                                               machine.signature(params)))
            lines += ['    {',
                      marker('action_' + name, 'perform the effect'),
                      '    }',
                      '']

    lines += ['private:',
              '    inline {} & self()'.format(cls),
              '    {   return (*this); }',
              '']
    if machine:
        lines += ['    {}FSM  mFsm;    //!< The state machine this component drives.'
                  .format(machine.name), '']
    lines += ['    {}() = delete;'.format(cls),
              '    AREG_NOCOPY_NOMOVE({});'.format(cls),
              '};']
    return lines


# A consumer with one request needs no sequencing. With more than one the responses
# arrive asynchronously, so a straight-line sequence races and the steps are spaced by
# a timer instead.
STEP_INTERVAL_MS = 1000


def steps_scenario(iface):
    """True when the consumer walks a sequence and so needs a stepping timer."""
    return len(iface.requests) > 1


def consumer_class(iface, cls):
    """The consumer component, subscribed and handling everything it subscribed to."""
    stepped = steps_scenario(iface)
    pad = ' ' * (len(cls) + 13)
    lines = ['class {} final : public    areg::Component'.format(cls),
             '{}, protected {}ConsumerBase'.format(pad, iface.name)]
    if stepped:
        lines.append('{}, private   areg::TimerConsumer'.format(pad))
    lines += ['{',
              'public:',
              '    {}(const areg::ComponentEntry & entry, areg::ComponentThread & owner)'.format(cls),
              '        : areg::Component(entry, owner)',
              '        , {}ConsumerBase(entry.mDependencyServices[0].mRoleName, owner)'.format(iface.name)]
    if stepped:
        lines += ['        , areg::TimerConsumer()',
                  '        , mPace(static_cast<areg::TimerConsumer &>(self()), "Pace")']
    lines += ['    { }',
              '',
              'protected:',
              '    bool service_connected(areg::ServiceConnectionState status, areg::ProxyBase & proxy) final',
              '    {',
              '        bool result{ false };',
              '        if ({}ConsumerBase::service_connected(status, proxy))'.format(iface.name),
              '        {',
              '            result = true;',
              '            if (areg::is_service_connected(status))',
              '            {']
    if iface.attributes or iface.broadcasts:
        lines.append('                // Subscriptions are made here, and again after '
                     'every reconnection.')
        for attr_name, _ in iface.attributes:
            lines.append('                notify_on_{}_update(true);'.format(to_snake(attr_name)))
        for name, _ in iface.broadcasts:
            lines.append('                notify_on_broadcast_{}(true);'.format(to_snake(name)))
    # A generated application that waits forever is not one that runs as written.
    # The first request that carries a response completes a round trip, and the
    # response handler below quits, so the program starts and ends on its own.
    answered_first = next((entry for entry in iface.requests
                           if entry[0] in set(n for n, _ in iface.responses)), None)
    if answered_first:
        name, params = answered_first
        args = ', '.join(default_expr(iface, type_name) for _, type_name in params)
        lines.append(marker('first_request',
                            'the first request of the scenario', 16))
        lines.append(placeholder('                request_{}({});'
                                 .format(to_snake(name), args)))
    else:
        lines.append(marker('first_request',
                            'the first request of the scenario', 16))
    if stepped:
        lines += ['',
                  '                // One step of the scenario per tick.',
                  '                mPace.stop_timer();',
                  '                mPace.start_timer({}, static_cast<areg::DispatcherThread &>'
                  '(master_thread()),'.format(STEP_INTERVAL_MS),
                  '                                  areg::TimerBase::CONTINUOUSLY);']
    lines += ['            }',
              '            else if ((status == areg::ServiceConnectionState::Disconnected) ||',
              '                     (status == areg::ServiceConnectionState::ConnectionLost))',
              '            {',
              '                // The provider went away. The framework reconnects and',
              '                // calls this again, so nothing here quits.',
              '                if (is_quitting() == false)',
              '                {',
              marker('peer_lost',
                     'what losing the provider means to this scenario', 20),
              '                }',
              '            }',
              '            else if ((status == areg::ServiceConnectionState::Rejected) ||',
              '                     (status == areg::ServiceConnectionState::Shutdown))',
              '            {',
              '                // Terminal states. The framework does not reconnect',
              '                // from these.',
              marker('service_refused',
                     'what a refused or shut-down service means here', 16),
              '                std::cerr << "service is " << areg::as_string(status)',
              '                          << ", giving up" << std::endl;']
    if stepped:
        lines.append('                mPace.stop_timer();')
    lines += ['                quit_with(1);',
              '            }',
              '        }',
              '',
              '        return result;',
              '    }',
              '']

    if stepped:
        lines += ['    void process_timer(areg::Timer & timer) final',
                  '    {',
                  marker('next_step', 'the next request of the scenario'),
                  '',
                  '        if ((cStallTicks != 0) && (++mIdleTicks >= cStallTicks))',
                  '        {',
                  '            fail("the scenario stopped making progress");',
                  '        }',
                  '    }',
                  '']

    first = True
    for name, params in iface.responses:
        lines.append('    void response_{}({}) final'.format(to_snake(name),
                                                             iface.signature(params)))
        lines.append('    {')
        lines.append(marker('response_' + to_snake(name),
                            'what this answer means for the scenario'))
        if first:
            lines.append('        // placeholder(you): the scenario ends here until a '
                         'later step replaces it.')
            if stepped:
                lines.append(placeholder('        mPace.stop_timer();'))
            lines.append(placeholder('        quit_with(0);'))
            first = False
        lines.append('    }')
        lines.append('')

    for name, _ in iface.requests:
        lines.append('    void request_{}_failed(areg::ResultType reason) final'.format(to_snake(name)))
        lines += ['    {',
                  '        std::cerr << "request {} failed, reason " '
                  '<< static_cast<int>(reason) << std::endl;'.format(name)]
        if stepped:
            lines.append('        mPace.stop_timer();')
        lines += ['        quit_with(1);',
                  '    }',
                  '']

    for name, params in iface.broadcasts:
        lines.append('    void broadcast_{}({}) final'.format(to_snake(name),
                                                              iface.signature(params)))
        lines += ['    {',
                  marker('broadcast_' + to_snake(name),
                         'what this broadcast means for the scenario'),
                  '    }',
                  '']

    for attr_name, type_name in iface.attributes:
        # The base applies the parameter rule, so anything but a primitive arrives by
        # const reference. An override that disagrees is refused as not virtual.
        lines.append('    void on_{}_update({} {}, areg::DataState state) final'
                     .format(to_snake(attr_name), iface.passed_as(type_name), attr_name))
        lines += ['    {',
                  '        if (state == areg::DataState::DataIsOK)',
                  '        {',
                  marker('update_' + to_snake(attr_name),
                         'the new value is ready to use', 12),
                  '        }',
                  '    }',
                  '']
    lines += ['private:',
              '    inline {} & self()'.format(cls),
              '    {   return (*this); }',
              '']
    if stepped:
        lines += ['    //! Ends the scenario as a failure, naming what went wrong.',
                  '    void fail(const char * why)',
                  '    {',
                  '        std::cerr << "FAIL: " << why << std::endl;',
                  '        mPace.stop_timer();',
                  '        quit_with(1);',
                  '    }',
                  '',
                  '    //! Restarts the stall watchdog. Call it wherever the scenario advances.',
                  '    void progressed()',
                  '    {   mIdleTicks = 0; }',
                  '',
                  '    areg::Timer  mPace;   //!< Spaces the requests of the scenario.',
                  '',
                  marker('stall_ticks',
                         'ticks of no progress that end the run; 0 leaves the '
                         'watchdog off', 4),
                  placeholder('    static constexpr uint32_t cStallTicks{ 0 };'),
                  '    uint32_t                  mIdleTicks{ 0 };',
                  '']
    lines += ['    {}() = delete;'.format(cls),
              '    AREG_NOCOPY_NOMOVE({});'.format(cls),
              '};']
    return lines


# unload_model() destroys the components, so a value main() has to read lives in the
# application storage, which outlives them. Generating this is what stops an
# application inventing a global for its exit code: a scenario that asserts a non-zero
# exit passes falsely when nothing ever sets one.
EXIT_CODE = ['constexpr char const _exitCode[]{ "exitCode" };',
             '',
             '//! Ends the application with this exit code, in storage that outlives',
             '//! the components.',
             'void quit_with(int code)',
             '{',
             '    areg::Primitive value{};',
             '    value.valInt.mElement = code;',
             '    areg::Application::store_element(_exitCode, value);',
             '    areg::Application::signal_quit();',
             '}',
             '',
             '//! True once quit_with() has run, so a disconnect that follows is',
             '//! this process shutting down rather than a lost provider.',
             'bool is_quitting()',
             '{',
             '    return areg::Application::is_element_stored(_exitCode);',
             '}',
             '']

EXIT_MAIN = ['int main()',
             '{',
             '    areg::Application::setup();',
             '    areg::Application::load_model(_modelName);',
             '    areg::Application::wait_quit(areg::WAIT_INFINITE);',
             '    areg::Application::unload_model(_modelName);',
             '    areg::Application::release();',
             '    return areg::Application::stored_element(_exitCode).valInt.mElement;',
             '}',
             '']

# The console quit path, asked of nearly every task. End of input is not a quit
# request: a process started without a console is handed a stream nothing is ever
# written to, so the loop blocks there and the service keeps running.
CONSOLE_INCLUDES = ['#include <iostream>', '#include <string>']

MAIN_BODY = ['int main()',
             '{',
             '    areg::Application::setup();',
             '    areg::Application::load_model(_modelName);',
             '',
             '    // Quits on "-q" or "--quit" from the console. Any other input is',
             '    // ignored, and end of input keeps the service running.',
             '    bool quitRequested{ false };',
             '    std::string line;',
             '    while (std::getline(std::cin, line))',
             '    {',
             '        if ((line == "-q") || (line == "--quit"))',
             '        {',
             '            quitRequested = true;',
             '            break;',
             '        }',
             '    }',
             '    if (quitRequested == false)',
             '    {',
             '        areg::Application::wait_quit(areg::WAIT_INFINITE);',
             '    }',
             '',
             '    areg::Application::unload_model(_modelName);',
             '    areg::Application::release();',
             '    return 0;',
             '}',
             '']

def provider_name(iface):
    """The provider component: named after the service, like its generated base."""
    return iface.name + 'Provider'


def consumer_name(iface):
    """The consumer component: named after the service, like its generated base."""
    return iface.name + 'Consumer'


def provider_registration(iface, indent):
    pad = ' ' * indent
    cls = provider_name(iface)
    return [pad + 'BEGIN_REGISTER_THREAD("ProviderThread")',
            pad + '    BEGIN_REGISTER_COMPONENT("{}", {})'.format(cls, cls),
            pad + '        REGISTER_IMPLEMENT_SERVICE({}::ServiceName, {}::InterfaceVersion)'
            .format(iface.name, iface.name),
            pad + '    END_REGISTER_COMPONENT("{}")'.format(cls),
            pad + 'END_REGISTER_THREAD("ProviderThread")']


def class_includes(iface, machine=None):
    """The framework headers the types of these documents need."""
    wanted = set()
    for document in (iface, machine):
        if document is None:
            continue
        groups = [document.attributes]
        for holder in (document.requests, document.responses, document.broadcasts,
                       document.actions, document.triggers):
            for _, params in holder:
                groups.append(params)
        for group in groups:
            for _, type_name in group:
                header = CLASS_HEADERS.get(CLASSES.get(type_name))
                if header:
                    wanted.add(header)
    return ['#include "{}"'.format(h) for h in sorted(wanted)]


INIT_LINE = re.compile(r'^ {8}[:,] ')
CALLED = re.compile(r'^(.*?)\b([A-Za-z_]\w*)\s*\((.*)$')


def split_class(cls, lines):
    """An inline class, as a declaration and its out-of-line definitions.

    Every member function whose body opens on its own line moves to the source,
    qualified with the class name; an inline one stays in the header.
    """
    header, source = [], []
    index = 0
    while index < len(lines):
        line = lines[index]
        opens = index + 1
        while opens < len(lines) and INIT_LINE.match(lines[opens]):
            opens += 1
        text = line.strip()
        is_function = (line.startswith('    ') and not line.startswith('     ')
                       and '(' in text and not text.endswith(';')
                       and not text.startswith('//') and not text.startswith('inline ')
                       and opens < len(lines)
                       and lines[opens].rstrip() in ('    {', '    { }'))
        if not is_function:
            if text in ('protected:', 'private:') and header and header[-1].strip():
                header.append('')
            header.append(line)
            index += 1
            continue
        header.append('    {};'.format(text))
        if text.endswith(' final'):
            text = text[:-len(' final')]
        called = CALLED.match(text)
        source.append('{}{}::{}({}'.format(called.group(1), cls, called.group(2),
                                             called.group(3)))
        source += [init[4:] for init in lines[index + 1:opens]]
        if lines[opens].rstrip() == '    { }':
            source += ['{', '}', '']
            index = opens + 1
        else:
            source.append('{')
            index = opens + 1
            while lines[index].rstrip() != '    }':
                body = lines[index]
                source.append(body[4:] if body.startswith('    ') else body)
                index += 1
            source += ['}', '']
            index += 1
        if index < len(lines) and not lines[index].strip():
            index += 1
    return header, source


def component_files(cls, brief, includes, class_lines, state_slot, prelude=(),
                    state_hint='the members and helpers your rules need, defined here'):
    """The .hpp and the .cpp of one component, named after its class."""
    declaration, definitions = split_class(cls, class_lines)
    private = declaration.index('private:')
    declaration.insert(private + 1, marker(state_slot, state_hint, 4))
    guard = cls.upper() + '_HPP'
    header = ['/**',
              ' * \\file    {}.hpp'.format(cls),
              ' * \\brief   {}'.format(brief),
              ' **/',
              '#ifndef {}'.format(guard),
              '#define {}'.format(guard),
              '',
              '#include "areg/base/areg_global.h"',
              '#include "areg/component/Component.hpp"',
              '#include "areg/component/ComponentThread.hpp"']
    header += includes + ['']
    header += list(prelude)
    header += declaration + ['', '#endif // {}'.format(guard), '']
    source = ['/**',
              ' * \\file    {}.cpp'.format(cls),
              ' * \\brief   {}'.format(brief),
              ' **/',
              '#include "{}.hpp"'.format(cls),
              '',
              '#include <iostream>',
              '',
              '#include "areg/appbase/Application.hpp"',
              '']
    source += definitions
    return [(cls + '.hpp', '\n'.join(header)), (cls + '.cpp', '\n'.join(source))]


# The consumer ends the application through this; main() defines it, next to the
# storage it writes.
QUIT_DECLARATION = ['//! Ends the application with this exit code, in storage that outlives',
                    '//! the components. Defined next to main().',
                    'void quit_with(int code);',
                    '',
                    '//! True once quit_with() has run. Defined next to main().',
                    'bool is_quitting();',
                    '']

MAIN_INCLUDES = ['#include "areg/base/areg_global.h"',
                 '#include "areg/appbase/Application.hpp"',
                 '#include "areg/base/String.hpp"',
                 '#include "areg/component/ComponentLoader.hpp"']


# The task of a two-process application asks for the two programs in separate
# folders, each with its own main(). One process keeps a flat folder: there is
# nothing to separate it from.
PROVIDER_DIR = {'ipc': 'provider/', 'local': ''}
CONSUMER_DIR = {'ipc': 'consumer/', 'local': ''}

# What the scaffold's CMakeLists.txt names, and what it becomes once the
# application is written into its folders.
SCAFFOLD_MAINS = {'provider.cpp': 'provider/main.cpp',
                  'consumer.cpp': 'consumer/main.cpp'}


def app_files(iface, mode, include_root, machine=None):
    """The whole application: one .hpp and .cpp per component, and the model with main().

    Returns a list of (file name, text). The result compiles and runs as written;
    every place a rule belongs is marked TODO(you).
    """
    provider_cls = provider_name(iface)
    consumer_cls = consumer_name(iface)
    provider_base = ['#include "{}/{}ProviderBase.hpp"'.format(include_root, iface.name)]
    if machine:
        provider_base += ['#include "{}/{}ActionHandler.hpp"'.format(include_root, machine.name),
                          '#include "{}/{}FSM.hpp"'.format(include_root, machine.name)]
    consumer_base = ['#include "{}/{}ConsumerBase.hpp"'.format(include_root, iface.name)]

    produced = [(PROVIDER_DIR[mode] + name, text) for name, text in component_files(
        provider_cls, 'Provider of the {} service.'.format(iface.name),
        class_includes(iface, machine) + [''] + provider_base,
        provider_class(iface, provider_cls, machine), 'provider_state')]
    produced += [(CONSUMER_DIR[mode] + name, text) for name, text in component_files(
        consumer_cls, 'Consumer of the {} service.'.format(iface.name),
        class_includes(iface) + TIMER_INCLUDES * steps_scenario(iface) + [''] + consumer_base,
        consumer_class(iface, consumer_cls), 'consumer_state', QUIT_DECLARATION,
        'the members and helpers your rules need, defined here, and the one saying '
        'what step the scenario is on' if steps_scenario(iface) else
        'the members and helpers your rules need, defined here')]

    def head(file_name, brief):
        return ['/**',
                ' * \\file    {}'.format(file_name),
                ' * \\brief   {} The model and main(); the components are in their own files.'
                .format(brief),
                ' **/'] + MAIN_INCLUDES + ['']

    if mode == 'local':
        lines = head('main.cpp', 'The {} service in two threads of one process.'
                     .format(iface.name))
        lines += ['#include "{}.hpp"'.format(provider_cls),
                  '#include "{}.hpp"'.format(consumer_cls), '']
        lines += EXIT_CODE
        lines += ['constexpr char const _modelName[]{{ "{}Model" }};'.format(iface.name),
                  '',
                  'BEGIN_MODEL(_modelName)',
                  '']
        lines += provider_registration(iface, 4)
        lines += ['',
                  '    BEGIN_REGISTER_THREAD("ConsumerThread")',
                  '        BEGIN_REGISTER_COMPONENT("{}", {})'.format(consumer_cls, consumer_cls),
                  '            REGISTER_DEPENDENCY("{}")'.format(provider_cls),
                  '        END_REGISTER_COMPONENT("{}")'.format(consumer_cls),
                  '    END_REGISTER_THREAD("ConsumerThread")',
                  '',
                  'END_MODEL(_modelName)',
                  '']
        lines += EXIT_MAIN
        return produced + [('main.cpp', '\n'.join(lines))]

    provider = head(PROVIDER_DIR[mode] + 'main.cpp',
                    'The process that provides the {} service.'.format(iface.name))
    provider += CONSOLE_INCLUDES + ['']
    provider += ['#include "{}.hpp"'.format(provider_cls), '']
    provider += ['constexpr char const _modelName[]{ "ProviderModel" };',
                 '',
                 'BEGIN_MODEL(_modelName)']
    provider += provider_registration(iface, 4)
    provider += ['END_MODEL(_modelName)', '']
    provider += MAIN_BODY

    consumer = head(CONSUMER_DIR[mode] + 'main.cpp',
                    'The process that consumes the {} service.'.format(iface.name))
    consumer += ['#include "{}.hpp"'.format(consumer_cls), '']
    consumer += EXIT_CODE
    consumer += ['constexpr char const _modelName[]{ "ConsumerModel" };',
                 '',
                 '// A unique role name lets several consumer processes run at the same time.',
                 'const areg::String _consumer(areg::generate_name("{}"));'.format(consumer_cls),
                 '',
                 'BEGIN_MODEL(_modelName)',
                 '    BEGIN_REGISTER_THREAD("ConsumerThread")',
                 '        BEGIN_REGISTER_COMPONENT(_consumer, {})'.format(consumer_cls),
                 '            REGISTER_DEPENDENCY("{}")'.format(provider_cls),
                 '        END_REGISTER_COMPONENT(_consumer)',
                 '    END_REGISTER_THREAD("ConsumerThread")',
                 'END_MODEL(_modelName)',
                 '']
    consumer += EXIT_MAIN
    return produced + [(PROVIDER_DIR[mode] + 'main.cpp', '\n'.join(provider)),
                       (CONSUMER_DIR[mode] + 'main.cpp', '\n'.join(consumer))]


# A generated main() of the scaffold includes one of these and registers a model.
SCAFFOLD_BASE = re.compile(r'\w+(?:Provider|Consumer)Base\.hpp')


def drop_scaffold(out, produced, mode):
    """Remove the placeholder sources the application has replaced.

    A scaffold main() is removed only when the application wrote the file that
    takes its place and the placeholder names another service. A file of the
    caller's own is never touched.
    """
    written = set(name.replace('\\', '/') for name, _ in produced)
    classes = set(os.path.basename(name)[:-4] for name in written if name.endswith('.hpp'))
    dropped = []
    for old, new in SCAFFOLD_MAINS.items():
        if mode != 'ipc' or new not in written:
            continue
        path = os.path.join(out, old)
        if not os.path.isfile(path):
            continue
        with open(path, encoding='utf-8', errors='ignore') as handle:
            text = handle.read()
        # It is the scaffold's only if it holds a model of the generated shape and
        # names no component this call has just written. Anything else is the
        # caller's file and is left where it is.
        if 'BEGIN_MODEL' not in text or not SCAFFOLD_BASE.search(text):
            continue
        if any(name in text for name in classes):
            continue
        os.remove(path)
        dropped.append(os.path.join(out, old).replace('\\', '/'))
    for path in dropped:
        print('removed {} -- the placeholder the application replaced'.format(path))
    return dropped


def app_sources(produced, mode):
    """Which executable compiles which generated file, keyed by its main file."""
    components = [name for name, _ in produced
                  if name.endswith('.cpp') and not name.endswith('main.cpp')]
    if mode == 'local':
        return {'main.cpp': components}
    return {PROVIDER_DIR[mode] + 'main.cpp':
            [n for n in components if n.endswith('Provider.cpp')],
            CONSUMER_DIR[mode] + 'main.cpp':
            [n for n in components if n.endswith('Consumer.cpp')]}


EXECUTABLE = re.compile(r'^(\s*macro_declare_executable\(\s*)([^\s)]+)\s+([^\s)]+)([^)]*)(\).*)$')
DOCUMENT = re.compile(r'^\s*(addServiceInterface|addStateMachine|addDataType)\(\s*([^\s)]+)\s+([^\s)]+)\s*\)')


def update_cmake(path, sources=None, documents=None, prune=None):
    """Name the generated sources and the documents in src/CMakeLists.txt.

    sources maps a main file to the files its executable also compiles; a source
    already listed stays. documents is a list of (function, path relative to the
    project root) that must each have a line. With prune, a document line under
    that folder whose document is not in documents is removed.
    Returns the lines changed, or None when there is no such file.
    """
    if not os.path.isfile(path):
        return None
    with open(path, encoding='utf-8') as handle:
        lines = handle.read().splitlines()
    changed = []
    library = None
    for line in lines:
        found = DOCUMENT.match(line) or EXECUTABLE.match(line)
        if found:
            library = found.group(2) if DOCUMENT.match(line) else found.group(3)
            break
    wanted = [(fn, doc.replace('\\', '/')) for fn, doc in (documents or []) if doc]
    if prune is not None:
        folder = prune.replace('\\', '/').rstrip('/') + '/'
        keep = set(doc for _, doc in wanted)
        kept = []
        for line in lines:
            found = DOCUMENT.match(line)
            if found and found.group(3).startswith(folder) and found.group(3) not in keep:
                changed.append('removed ' + line.strip())
                continue
            kept.append(line)
        lines = kept
    present = set(found.group(3) for found in map(DOCUMENT.match, lines) if found)
    first_exe = next((i for i, line in enumerate(lines) if EXECUTABLE.match(line)), len(lines))
    insert_at = max([i + 1 for i, line in enumerate(lines) if DOCUMENT.match(line)] or [first_exe])
    for function, doc in wanted:
        if doc not in present and library:
            line = '{}({} {})'.format(function, library, doc)
            lines.insert(insert_at, line)
            insert_at += 1
            present.add(doc)
            changed.append('added ' + line)
    # The scaffold declares its executables against a flat provider.cpp and
    # consumer.cpp. The application is written into a folder per process, so the
    # name in the line is replaced before the extra sources are matched against it.
    for index, line in enumerate(lines):
        found = EXECUTABLE.match(line)
        if not found:
            continue
        listed = found.group(4).split()
        renamed = [SCAFFOLD_MAINS.get(name, name) if SCAFFOLD_MAINS.get(name) in
                   (sources or {}) else name for name in listed]
        if renamed != listed:
            lines[index] = '{}{} {} {}{}'.format(found.group(1), found.group(2),
                                                 found.group(3), ' '.join(renamed),
                                                 found.group(5))
            changed.append('{} now compiles {}'.format(found.group(2),
                                                       ' '.join(renamed)))
    for main_file, extra in (sources or {}).items():
        for index, line in enumerate(lines):
            found = EXECUTABLE.match(line)
            if not found or main_file not in found.group(4).split():
                continue
            listed = found.group(4).split()
            missing = [name for name in extra if name not in listed]
            if missing:
                lines[index] = '{}{} {} {}{}'.format(found.group(1), found.group(2),
                                                     found.group(3),
                                                     ' '.join(listed + missing),
                                                     found.group(5))
                changed.append('listed {} in {}'.format(' '.join(missing), found.group(2)))
    if changed:
        with open(path, 'w', encoding='utf-8') as handle:
            handle.write('\n'.join(lines) + '\n')
    return changed


# What a process is expected to print, until the rule that prints it is written.
# It is a regular expression that matches nothing, so a scenario left unfilled fails
# and says which line is missing rather than passing on no evidence at all.
SCENARIO_TODO = ('TODO(you) {}: a line this process prints that proves one '
                 'requirement')

# The scenario that proves the generated console quit loop. It expects no output, so
# it holds no hole and passes as written.
QUIT_SCENARIO = 'quit'


def proc_label(spec):
    return spec.get('name') or spec['binary']


def expect_slot(scenario, spec):
    """The name of one process's expectations in one scenario, as a marker.

    Named, so the worksheet carries these holes beside the code ones and one command
    fills both. The pair is unique: a scenario names each process once.
    """
    return 'expect_{}_{}'.format(re.sub(r'\W+', '_', scenario).strip('_'),
                                 re.sub(r'\W+', '_', proc_label(spec)).strip('_'))


def update_scenarios(path, mode, iface):
    """Point scenarios.json at the application that was just generated.

    The binaries and the router come from the file setup_project.py wrote, because
    they are the names its CMake files declare. Everything the run has to prove is a
    named hole, and the shape around them -- the process list, the exit code, the
    keys "stdin" and "stop" -- is written here instead of read from a page.
    """
    if not os.path.exists(path):
        return
    try:
        with open(path, encoding='utf-8') as handle:
            document = json.load(handle)
        scenarios = document['scenarios']
        procs = scenarios[0]['procs']
    except (ValueError, OSError, KeyError, IndexError, TypeError):
        print('  kept   {} -- it is not the file setup_project.py wrote'.format(path))
        return

    # An expectation that is not the scaffold's was written by whoever is using this
    # tool, and is never overwritten: regenerating a source file must not discard the
    # evidence the run is checked against.
    if not scenarios[0].pop('scaffold', False):
        print('  kept   {} -- its expectations are yours, not the scaffold\'s'
              .format(path))
        return
    for index, spec in enumerate(procs):
        spec['expect'] = [SCENARIO_TODO.format(
            expect_slot(scenarios[0].get('name', 'scenario'), spec))]
        if index == len(procs) - 1:
            spec['exit'] = 0
        else:
            spec.pop('exit', None)
    # A stepped scenario spends a second per request, so the default timeout is
    # raised to cover the whole sequence.
    scenarios[0]['timeout'] = max(int(scenarios[0].get('timeout', 60)),
                                  60 + len(iface.requests) * STEP_INTERVAL_MS // 1000)

    # The provider's main() carries the console quit loop, so the scenario that
    # proves it is written here too. "stdin" is fed the moment the process starts,
    # which is why the provider leads a scenario of its own.
    quit_written = False
    if mode == 'ipc' and not any(s.get('name') == QUIT_SCENARIO for s in scenarios):
        lead = procs[0]
        scenarios.append({'name': QUIT_SCENARIO,
                          'timeout': 30,
                          'router': scenarios[0].get('router', True),
                          'procs': [{'binary': lead['binary'],
                                     'name': proc_label(lead),
                                     'lead': True,
                                     'stdin': ['-q'],
                                     'exit': 0}]})
        quit_written = True

    with open(path, 'w', encoding='utf-8') as handle:
        json.dump(document, handle, indent=2)
        handle.write('\n')
    print('wrote  {}'.format(path))
    print('  {} process(es), router {}. Each "expect" hole is a section of the'
          .format(len(procs), 'on' if scenarios[0].get('router') else 'off'))
    print('  worksheet, one regular expression per line. The generated main() prints')
    print('  nothing, so every line a scenario matches comes from a body you write.')
    print('  Two more keys exist and no page is needed for them.')
    print('  "stdin": ["-q"] is written the moment the process starts, so it goes')
    print('  on the lead of a scenario of its own; on any other process it quits that')
    print('  process before its peers are served. A scenario-level')
    print('  "stop": {"proc": "<name>", "after": "<regex>"} takes a peer away.')
    if quit_written:
        print('  A "{}" scenario was added: the provider alone, fed "-q", exit 0. It'
              .format(QUIT_SCENARIO))
        print('  proves the console quit path the generated main() already carries,')
        print('  and it needs nothing from you.')


APP_NOTE = (
    '  These files compile and run as written. Every place a rule of your own\n'
    '  belongs is one TODO(you) line above; the model, main() and every\n'
    '  subscription are already correct and need no page. Rewriting a whole file\n'
    '  is never needed.')


def report_todos(out, mode):
    """Every marker still left in the application's sources, with its line."""
    names = []
    for folder, dirs, files in os.walk(out) if os.path.isdir(out) else []:
        dirs[:] = [d for d in dirs if d not in ('services', 'build')]
        names += [os.path.join(folder, name) for name in files
                  if name.endswith(('.hpp', '.cpp'))]
    total = 0
    files = 0
    for path in sorted(names):
        if not os.path.exists(path):
            continue
        here = 0
        with open(path, encoding='utf-8') as handle:
            for number, line in enumerate(handle, 1):
                if MARKER.search(line):
                    print('{}:{}:'.format(path.replace('\\', '/'), number))
                    print(line.rstrip('\n').rstrip())
                    here += 1
        total += here
        files += 1 if here else 0
    if total == 0:
        print('no TODO(you) marker is left in {}'.format(out))
    else:
        print(TODOS_NOTE.format(total=total, files=files, tool=FILLER,
                                path=WORKSHEET))
    return 0


def main():
    parser = argparse.ArgumentParser(
        description='Write the components a .siml or .fsml document needs.')
    parser.add_argument('--doc', required=True, help='the .siml or .fsml document')
    parser.add_argument('--out', help='directory to write the sources into')
    parser.add_argument('--only', choices=['provider', 'consumer', 'both'], default='both')
    parser.add_argument('--include-root', default=None,
                        help='include path of the generated headers '
                             '(default: the document folder relative to the project)')
    parser.add_argument('--force', action='store_true', help='overwrite existing files')
    parser.add_argument('--app', action='store_true',
                        help='write the whole application -- the components, the '
                             'model and main() -- instead of a pair of skeletons. '
                             'The result compiles and runs as written')
    parser.add_argument('--machine',
                        help='with --app: the .fsml this application drives. The '
                             'provider owns the machine, implements its actions and '
                             'needs no separate host component')
    parser.add_argument('--mode', choices=['ipc', 'local'], default='ipc',
                        help='with --app: every component gets its own .hpp and .cpp; '
                             'ipc gives each process a folder with its own '
                             'main.cpp, local one '
                             'main.cpp, each holding the model and main(). Match '
                             'setup_project.py')
    parser.add_argument('--scenarios', default='scenarios.json',
                        help='with --app: the scenario file to point at the '
                             'generated application. Left alone when it does not '
                             'exist')
    parser.add_argument('--todos', action='store_true',
                        help='list the TODO(you) markers still left in the '
                             'application, with the file and line of each, and '
                             'write no file')
    parser.add_argument('--contract', action='store_true',
                        help='print the names the document generates and write no '
                             'file: what a caller needs from whoever wrote it')
    args = parser.parse_args()

    iface = Interface(args.doc)
    if args.contract:
        return print_contract(iface, args.doc)
    if args.todos:
        return report_todos(args.out or 'src', args.mode)
    include_root = args.include_root
    if include_root is None:
        include_root = os.path.relpath(os.path.dirname(os.path.abspath(args.doc)),
                                       os.getcwd()).replace('\\', '/')
        if include_root.startswith('..') or os.path.isabs(include_root):
            fail('the document is outside the working directory, so the include path '
                 'cannot be written relative to it. Pass --include-root explicitly.')

    if args.out is None:
        args.out = 'src' if args.app else None
    if args.out is None:
        fail('--out is required: the directory to write the sources into')
    os.makedirs(args.out, exist_ok=True)

    if args.app:
        if args.doc.lower().endswith('.fsml'):
            fail('--app builds an application from a .siml service contract. Pass the '
                 '.siml as --doc, and this document as --machine.')
        machine = None
        if args.machine:
            if not args.machine.lower().endswith('.fsml'):
                fail('--machine takes the .fsml state machine document; got {}'
                     .format(args.machine))
            machine = Interface(args.machine)
            if not machine.actions:
                fail('the machine declares no action, so there is nothing for the '
                     'provider to implement')
            if machine.name == iface.name:
                fail('the machine and the service are both named "{}". A .fsml name '
                     'becomes a C++ namespace, so the two documents need different '
                     'names -- "{}" and "{}Service" is the usual pair.'
                     .format(iface.name, machine.name, machine.name))
            if machine.name in (provider_name(iface), consumer_name(iface)):
                fail('the machine is named "{}", which is also the name of a generated '
                     'component. A .fsml name becomes a C++ namespace; rename the '
                     'machine.'.format(machine.name))
        produced = app_files(iface, args.mode, include_root, machine)
        retained = [(file_name,
                     write(os.path.join(args.out, file_name), text, args.force))
                    for file_name, text in produced]
        # A kept file is one the new documents could not be applied to. Naming it
        # is the difference between a regeneration that reports what it did and one
        # that reports what it would have done.
        unapplied = [file_name for (file_name, kept), (_, fresh)
                     in zip(retained, produced) if kept != fresh]
        if unapplied:
            print('  {} file(s) kept as they are, so the new documents reached '
                  'none of them: {}'.format(len(unapplied), ', '.join(unapplied)))
            print('  --regenerate writes them again and discards what is in them.')
        drop_scaffold(args.out, produced, args.mode)
        documents = [('addServiceInterface', os.path.relpath(args.doc).replace('\\', '/'))]
        if args.machine:
            documents.append(('addStateMachine',
                              os.path.relpath(args.machine).replace('\\', '/')))
        changed = update_cmake(os.path.join(args.out, 'CMakeLists.txt'),
                               app_sources(produced, args.mode), documents)
        for change in changed or []:
            print('  {}/CMakeLists.txt: {}'.format(args.out.replace('\\', '/'), change))
        update_scenarios(args.scenarios, args.mode, iface)
        written = write_worksheet(retained, args.out, iface, args.doc,
                                  machine, args.machine, args.scenarios)
        print_todos(retained, args.out, written,
                    len(scenario_holes(args.scenarios)), args.scenarios)
        print(APP_NOTE)
        return 0

    # A state machine document produces one host component, not a pair.
    if args.doc.lower().endswith('.fsml'):
        if not iface.actions:
            fail('the machine declares no action, so there is nothing to implement')
        name = iface.name + 'Host'
        header, source = machine_files(iface, name, include_root)
        path = os.path.join(args.out, name + '.hpp')
        write(path, header, args.force)
        write(os.path.join(args.out, name + '.cpp'), source, args.force)
        print_index(path, header)
        print(HOST_NOTE.format(name=name))
        return 0

    written = []
    if args.only in ('provider', 'both'):
        name = iface.name + 'Provider'
        header, source = provider_files(iface, name, include_root)
        path = os.path.join(args.out, name + '.hpp')
        write(path, header, args.force)
        write(os.path.join(args.out, name + '.cpp'), source, args.force)
        written.append((path, header))
    if args.only in ('consumer', 'both'):
        name = iface.name + 'Consumer'
        header, source = consumer_files(iface, name, include_root)
        path = os.path.join(args.out, name + '.hpp')
        write(path, header, args.force)
        write(os.path.join(args.out, name + '.cpp'), source, args.force)
        written.append((path, header))
    for path, header in written:
        print_index(path, header)
    print(BODY_NOTE.format(os.path.basename(written[0][0]) if written else 'Yours.hpp'))
    return 0


if __name__ == '__main__':
    sys.exit(main())
