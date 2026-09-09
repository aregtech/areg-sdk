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
CLASSES = {'String': 'areg::String', 'DateTime': 'areg::DateTime'}


def fail(message):
    sys.stderr.write('error: {}\n'.format(message))
    sys.exit(1)


def to_snake(name):
    """StringOnChange -> string_on_change; hello_service stays as it is."""
    text = re.sub(r'(.)([A-Z][a-z]+)', r'\1_\2', name)
    text = re.sub(r'([a-z0-9])([A-Z])', r'\1_\2', text)
    return text.lower()


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
        for method in root.findall('./MethodList/Method'):
            kind = (method.get('MethodType') or '').lower()
            entry = (method.get('Name'), self._params(method))
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
    if os.path.exists(path) and not force:
        print('kept   {}'.format(path))
        return text
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


def print_contract(iface, document):
    """The names a document generates, and nothing about how it was written.

    This is what a caller needs from whoever authored the document: the class to
    build against, what to override and what to call. It is derived from the
    document, so it cannot disagree with what the generator emits.
    """
    print('document:  {}'.format(document))
    print('interface: {}'.format(iface.name))
    if document.lower().endswith('.fsml'):
        print('classes:   {n}FSM (the machine), {n}ActionHandler (implement this)'
              .format(n=iface.name))
        for name, params in iface.triggers:
            print('  call     {}({})'.format(to_snake(name), iface.signature(params)))
        for name, params in iface.actions:
            print('  override {}({})'.format(to_snake(name), iface.signature(params)))
        for name, kind in iface.attributes:
            spelled = to_snake(name)
            print('  on the machine object: {}() / set_{}({})'
                  .format(spelled, spelled, iface.attribute_setter(kind, True)))
        print_types(iface)
        return 0
    print('classes:   {n}Provider and {n}Consumer build on the generated {n} base'
          .format(n=iface.name))
    for name, params in iface.requests:
        print('  override request_{}({})'.format(to_snake(name), iface.signature(params)))
    for name, params in iface.responses:
        print('  provider calls response_{}({}); consumer overrides it'
              .format(to_snake(name), iface.signature(params)))
    for name, params in iface.broadcasts:
        print('  provider calls broadcast_{}({}); consumer subscribes with '
              'notify_on_broadcast_{}(true)'
              .format(to_snake(name), iface.signature(params), to_snake(name)))
    for name, kind in iface.attributes:
        spelled = to_snake(name)
        print('  provider calls set_{}({}); consumer subscribes with '
              'notify_on_{}_update(true)'
              .format(spelled, iface.attribute_setter(kind, False), spelled))
    print_types(iface)
    return 0


def print_types(iface):
    """The data types the document generates, as the generator declares them."""
    for type_name, kind in iface.types:
        full = '{}::{}'.format(iface.name, type_name)
        if kind in ('enumeration', 'enumerate'):
            print('  enum class {}, with '
                  'const char * {}::as_string({} value)'
                  .format(full, iface.name, full))
        elif kind == 'structure':
            print('  struct {}, with a field-by-field == and <<'.format(full))
        elif kind == 'imported':
            print('  {} names the type your own header declares'.format(full))
        else:
            print('  {} is an alias to an areg container'.format(full))
    for name, kind in iface.constants:
        print('  constant {}::{} of type {}'.format(iface.name, name, kind))


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


APP_INCLUDES = ['#include <iostream>',
                '',
                '#include "areg/base/areg_global.h"',
                '#include "areg/appbase/Application.hpp"',
                '#include "areg/component/Component.hpp"',
                '#include "areg/component/ComponentLoader.hpp"',
                '#include "areg/component/ComponentThread.hpp"']


def provider_class(iface, cls):
    """The provider component, with every request answered."""
    pad = ' ' * (len(cls) + 13)
    lines = ['class {} final : public    areg::Component'.format(cls),
             '{}, protected {}ProviderBase'.format(pad, iface.name),
             '{',
             'public:',
             '    {}(const areg::ComponentEntry & entry, areg::ComponentThread & owner)'.format(cls),
             '        : areg::Component(entry, owner)',
             '        , {}ProviderBase(static_cast<areg::Component &>(self()))'.format(iface.name),
             '    {']
    if iface.attributes:
        lines.append('        // An attribute is invalid until it is set once.')
        for attr_name, type_name in iface.attributes:
            lines.append('        set_{}({});'.format(to_snake(attr_name),
                                                      default_expr(iface, type_name)))
    lines += ['    }', '', 'protected:']

    answered = dict((name, params) for name, params in iface.responses)
    for name, params in iface.requests:
        lines.append('    void request_{}({}) final'.format(to_snake(name),
                                                            iface.signature(params)))
        lines.append('    {')
        lines.append('        // TODO(you): the rule this request carries out.')
        if name in answered:
            args = ', '.join(default_expr(iface, t) for _, t in answered[name])
            lines.append('        response_{}({});'.format(to_snake(name), args))
        lines.append('    }')
        lines.append('')
    lines += ['private:',
              '    inline {} & self()'.format(cls),
              '    {   return (*this); }',
              '',
              '    {}() = delete;'.format(cls),
              '    AREG_NOCOPY_NOMOVE({});'.format(cls),
              '};']
    return lines


def consumer_class(iface, cls):
    """The consumer component, subscribed and handling everything it subscribed to."""
    pad = ' ' * (len(cls) + 13)
    lines = ['class {} final : public    areg::Component'.format(cls),
             '{}, protected {}ConsumerBase'.format(pad, iface.name),
             '{',
             'public:',
             '    {}(const areg::ComponentEntry & entry, areg::ComponentThread & owner)'.format(cls),
             '        : areg::Component(entry, owner)',
             '        , {}ConsumerBase(entry.mDependencyServices[0].mRoleName, owner)'.format(iface.name),
             '    { }',
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
    lines.append('                // TODO(you): the first request of the scenario.')
    lines += ['            }',
              '        }',
              '',
              '        return result;',
              '    }',
              '']

    first = True
    for name, params in iface.responses:
        lines.append('    void response_{}({}) final'.format(to_snake(name),
                                                             iface.signature(params)))
        lines.append('    {')
        lines.append('        // TODO(you): what this answer means for the scenario.')
        if first:
            lines.append('        // The scenario ends here until a later step replaces it.')
            lines.append('        areg::Application::signal_quit();')
            first = False
        lines.append('    }')
        lines.append('')

    for name, _ in iface.requests:
        lines.append('    void request_{}_failed(areg::ResultType reason) final'.format(to_snake(name)))
        lines += ['    {',
                  '        std::cerr << "request {} failed, reason " '
                  '<< static_cast<int>(reason) << std::endl;'.format(name),
                  '        areg::Application::signal_quit();',
                  '    }',
                  '']

    for name, params in iface.broadcasts:
        lines.append('    void broadcast_{}({}) final'.format(to_snake(name),
                                                              iface.signature(params)))
        lines += ['    {',
                  '        // TODO(you): what this broadcast means for the scenario.',
                  '    }',
                  '']

    for attr_name, type_name in iface.attributes:
        cpp = iface.cpp_type(type_name)[0]
        lines.append('    void on_{}_update({} {}, areg::DataState state) final'
                     .format(to_snake(attr_name), cpp, attr_name))
        lines += ['    {',
                  '        if (state == areg::DataState::DataIsOK)',
                  '        {',
                  '            // TODO(you): the new value is ready to use.',
                  '        }',
                  '    }',
                  '']
    lines += ['private:',
              '    {}() = delete;'.format(cls),
              '    AREG_NOCOPY_NOMOVE({});'.format(cls),
              '};']
    return lines


MAIN_BODY = ['int main()',
             '{',
             '    areg::Application::setup();',
             '    areg::Application::load_model(_modelName);',
             '    areg::Application::wait_quit(areg::WAIT_INFINITE);',
             '    areg::Application::unload_model(_modelName);',
             '    areg::Application::release();',
             '    return 0;',
             '}',
             '']

PROVIDER_ROLE = 'ServiceProvider'
CONSUMER_ROLE = 'ServiceConsumer'


def provider_registration(iface, indent):
    pad = ' ' * indent
    return [pad + 'BEGIN_REGISTER_THREAD("ProviderThread")',
            pad + '    BEGIN_REGISTER_COMPONENT("{}", {})'.format(PROVIDER_ROLE, PROVIDER_ROLE),
            pad + '        REGISTER_IMPLEMENT_SERVICE({}::ServiceName, {}::InterfaceVersion)'
            .format(iface.name, iface.name),
            pad + '    END_REGISTER_COMPONENT("{}")'.format(PROVIDER_ROLE),
            pad + 'END_REGISTER_THREAD("ProviderThread")']


def app_files(iface, mode, include_root):
    """The whole application: the components, the model and main().

    Returns a list of (file name, text). The result compiles and runs as written;
    every place a rule belongs is marked TODO(you).
    """
    provider_base = '#include "{}/{}ProviderBase.hpp"'.format(include_root, iface.name)
    consumer_base = '#include "{}/{}ConsumerBase.hpp"'.format(include_root, iface.name)
    head = ['/**', ' * \\file    {}', ' * \\brief   {}', ' **/']

    if mode == 'local':
        lines = [l.format('main.cpp') if '{}' in l and 'file' in l else
                 l.format('Provider and consumer of the {} service, in two threads of '
                          'one process.'.format(iface.name)) if '{}' in l else l
                 for l in head]
        lines += APP_INCLUDES + ['', provider_base, consumer_base, '']
        lines += provider_class(iface, PROVIDER_ROLE) + ['']
        lines += consumer_class(iface, CONSUMER_ROLE) + ['']
        lines += ['constexpr char const _modelName[]{{ "{}Model" }};'.format(iface.name),
                  '',
                  'BEGIN_MODEL(_modelName)',
                  '']
        lines += provider_registration(iface, 4)
        lines += ['',
                  '    BEGIN_REGISTER_THREAD("ConsumerThread")',
                  '        BEGIN_REGISTER_COMPONENT("{}", {})'.format(CONSUMER_ROLE, CONSUMER_ROLE),
                  '            REGISTER_DEPENDENCY("{}")'.format(PROVIDER_ROLE),
                  '        END_REGISTER_COMPONENT("{}")'.format(CONSUMER_ROLE),
                  '    END_REGISTER_THREAD("ConsumerThread")',
                  '',
                  'END_MODEL(_modelName)',
                  '']
        lines += MAIN_BODY
        return [('main.cpp', '\n'.join(lines))]

    provider = ['/**',
                ' * \\file    provider.cpp',
                ' * \\brief   The process that provides the {} service.'.format(iface.name),
                ' **/']
    provider += APP_INCLUDES + ['', provider_base, '']
    provider += provider_class(iface, PROVIDER_ROLE) + ['']
    provider += ['constexpr char const _modelName[]{ "ProviderModel" };',
                 '',
                 'BEGIN_MODEL(_modelName)']
    provider += provider_registration(iface, 4)
    provider += ['END_MODEL(_modelName)', '']
    provider += MAIN_BODY

    consumer = ['/**',
                ' * \\file    consumer.cpp',
                ' * \\brief   The process that consumes the {} service.'.format(iface.name),
                ' **/']
    consumer += APP_INCLUDES + ['#include "areg/base/String.hpp"', '', consumer_base, '']
    consumer += consumer_class(iface, CONSUMER_ROLE) + ['']
    consumer += ['constexpr char const _modelName[]{ "ConsumerModel" };',
                 '',
                 '// A unique role name lets several consumer processes run at the same time.',
                 'const areg::String _consumer(areg::generate_name("{}"));'.format(CONSUMER_ROLE),
                 '',
                 'BEGIN_MODEL(_modelName)',
                 '    BEGIN_REGISTER_THREAD("ConsumerThread")',
                 '        BEGIN_REGISTER_COMPONENT(_consumer, {})'.format(CONSUMER_ROLE),
                 '            REGISTER_DEPENDENCY("{}")'.format(PROVIDER_ROLE),
                 '        END_REGISTER_COMPONENT(_consumer)',
                 '    END_REGISTER_THREAD("ConsumerThread")',
                 'END_MODEL(_modelName)',
                 '']
    consumer += MAIN_BODY
    return [('provider.cpp', '\n'.join(provider)),
            ('consumer.cpp', '\n'.join(consumer))]


APP_NOTE = (
    '  These files compile and run as written. Every place a rule of your own\n'
    '  belongs is marked TODO(you); the model, main() and every subscription are\n'
    '  already correct and need no page. Build, run, then fill the TODOs in.')


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
    parser.add_argument('--mode', choices=['ipc', 'local'], default='ipc',
                        help='with --app: ipc writes provider.cpp and consumer.cpp, '
                             'local writes one main.cpp. Match setup_project.py')
    parser.add_argument('--contract', action='store_true',
                        help='print the names the document generates and write no '
                             'file: what a caller needs from whoever wrote it')
    args = parser.parse_args()

    iface = Interface(args.doc)
    if args.contract:
        return print_contract(iface, args.doc)
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
            fail('--app builds an application from a .siml service contract. Run this '
                 'tool without --app on the .fsml to get the machine host.')
        produced = app_files(iface, args.mode, include_root)
        for file_name, text in produced:
            write(os.path.join(args.out, file_name), text, args.force)
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
