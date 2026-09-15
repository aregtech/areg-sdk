#!/usr/bin/env python3
"""The parts a .dtml, a .siml and a .fsml document all carry.

The three grammars share their declarations tag for tag: a data type, a constant, a
parameter, a field and an include are written the same way in all three, and only the
sections around them differ. Everything shared is written once here, so a correction
reaches every document kind at once and gen_docs.py holds only what each kind says on
its own.

Nothing here has a command line. gen_docs.py is the tool.
"""
import sys
import xml.sax.saxutils as saxutils

PREDEFINED = set('bool char uint8 int16 uint16 int32 uint32 int64 uint64 float double '
                 'String WideString BinaryBuffer DateTime'.split())

# A container and whether it takes a key as well as a value.
CONTAINERS = {'Array': False, 'LinkedList': False, 'HashMap': True, 'Map': True, 'Pair': True}

TYPE_KINDS = {'enum': 'Enumeration', 'enumeration': 'Enumeration',
              'struct': 'Structure', 'structure': 'Structure',
              'imported': 'Imported', 'container': 'Container'}

DEFAULT_OF = {'bool': 'false', 'char': '0', 'float': '0.0', 'double': '0.0'}


def fail(message):
    sys.stderr.write('error: {}\n'.format(message))
    raise SystemExit(2)


def spell(value):
    """A JSON value as a document spells it. XML and C++ both write true and false,
    where Python writes True and False."""
    if isinstance(value, bool):
        return 'true' if value else 'false'
    return str(value)


def esc(value):
    """A value as an XML attribute carries it."""
    return saxutils.escape(spell(value), {'"': '&quot;'})


def esc_text(value):
    """A value as element text carries it."""
    return saxutils.escape(spell(value))


class Writer:
    """Collects the document, and hands out the identifiers it refers to.

    An element that something else refers to by number reserves its identifier under a
    key before anything is written, so a reference never has to wait for its target.
    """

    def __init__(self):
        self.lines = []
        self._next = 0
        self._named = {}

    def ident(self):
        self._next += 1
        return self._next

    def reserve(self, key):
        if key not in self._named:
            self._named[key] = self.ident()
        return self._named[key]

    def of(self, key):
        return self._named.get(key)

    def add(self, depth, line):
        self.lines.append('    ' * depth + line)

    def text(self):
        return '\n'.join(self.lines) + '\n'


def described(writer, depth, spec):
    """The Description child an element carries when the spec gives one."""
    if spec.get('description'):
        writer.add(depth, '<Description>{}</Description>'.format(esc_text(spec['description'])))


def named_list(spec, key, what=None):
    """The entries of a spec list, each allowed to be a bare name."""
    entries = spec.get(key) or []
    if isinstance(entries, dict):
        fail('"{}" is a list, not an object'.format(key))
    result = [{'name': entry} if isinstance(entry, str) else dict(entry) for entry in entries]
    for entry in result:
        if not entry.get('name'):
            fail('every entry of "{}"{} needs a name'
                 .format(key, ' in ' + what if what else ''))
    return result


def unique(entries, what):
    """The names of these entries, refusing a repeat."""
    seen = set()
    for entry in entries:
        if entry['name'] in seen:
            fail('two entries of {} are named "{}"'.format(what, entry['name']))
        seen.add(entry['name'])
    return seen




class Vocabulary:
    """Every type name a document may spell, and where each one came from."""

    def __init__(self, shared_space=None, shared_names=(), prefix=''):
        self.space = shared_space
        self.shared = set(shared_names)
        self.local = set()
        self.prefix = prefix

    def declare(self, names):
        self.local = set(names)

    def known(self, name):
        if name in PREDEFINED or name in self.local:
            return True
        if self.space:
            head, sep, tail = name.partition('::')
            if sep and head == self.space and tail in self.shared:
                return True
        return False

    def check(self, name, where):
        if not name:
            fail('{} has no type'.format(where))
        if self.known(name):
            return name
        offered = sorted(self.local) + \
            sorted('{}::{}'.format(self.space, n) for n in self.shared)
        fail('{} is declared "{}", which is not a type. This document declares: {}. '
             'Predefined: {}.'
             .format(where, name, ', '.join(offered) if offered else 'nothing',
                     ', '.join(sorted(PREDEFINED))))


def default_literal(type_name):
    """The value-initialised literal of a type, for a field that gives no default."""
    if type_name in DEFAULT_OF:
        return DEFAULT_OF[type_name]
    if type_name in PREDEFINED:
        return '' if type_name not in ('uint8', 'int16', 'uint16', 'int32', 'uint32',
                                       'int64', 'uint64') else '0'
    return ''


def enum_entries(entry):
    """The enumerators of an enumeration, however the spec spells them."""
    values = entry.get('values')
    if values is None:
        fail('enumeration "{}" declares no "values"'.format(entry['name']))
    if isinstance(values, dict):
        return [{'name': key, 'value': val} for key, val in values.items()]
    result = []
    for item in values:
        if isinstance(item, str):
            result.append({'name': item})
        else:
            result.append(dict(item))
    for item in result:
        if not item.get('name'):
            fail('every enumerator of "{}" needs a name'.format(entry['name']))
    return result


def write_datatype(writer, depth, entry, vocab):
    """One DataType, whichever of the four kinds it is."""
    kind = TYPE_KINDS.get(str(entry.get('kind', '')).lower())
    if kind is None:
        fail('type "{}" has kind "{}"; a kind is one of {}'
             .format(entry['name'], entry.get('kind'), ', '.join(sorted(set(TYPE_KINDS.values())))))
    head = '<DataType ID="{}" Name="{}" Type="{}"'.format(
        writer.reserve(('type', entry['name'])), esc(entry['name']), kind)
    if kind == 'Enumeration':
        head += ' Values="{}"'.format(esc(entry.get('derives', 'default')))
    writer.add(depth, head + '>')
    described(writer, depth + 1, entry)

    if kind == 'Enumeration':
        writer.add(depth + 1, '<FieldList>')
        for item in enum_entries(entry):
            ehead = '<EnumEntry ID="{}" Name="{}">'.format(writer.ident(), esc(item['name']))
            writer.add(depth + 2, ehead)
            if item.get('value') is not None:
                writer.add(depth + 3, '<Value>{}</Value>'.format(esc_text(item['value'])))
            described(writer, depth + 3, item)
            writer.add(depth + 2, '</EnumEntry>')
        writer.add(depth + 1, '</FieldList>')
    elif kind == 'Structure':
        fields = named_list(entry, 'fields', entry['name'])
        if not fields:
            fail('structure "{}" declares no "fields"'.format(entry['name']))
        writer.add(depth + 1, '<FieldList>')
        for field in fields:
            ftype = vocab.check(field.get('type'),
                                'field "{}" of "{}"'.format(field['name'], entry['name']))
            writer.add(depth + 2, '<Field ID="{}" Name="{}" DataType="{}">'
                       .format(writer.ident(), esc(field['name']), esc(ftype)))
            value = field.get('default', default_literal(ftype))
            if value == '':
                writer.add(depth + 3, '<Value IsDefault="true"/>')
            else:
                writer.add(depth + 3, '<Value IsDefault="true">{}</Value>'.format(esc_text(value)))
            described(writer, depth + 3, field)
            writer.add(depth + 2, '</Field>')
        writer.add(depth + 1, '</FieldList>')
    elif kind == 'Imported':
        header = entry.get('header') or entry.get('location')
        if not header or not entry.get('object'):
            fail('imported type "{}" needs "header" and "object", and may add "namespace"'
                 .format(entry['name']))
        writer.add(depth + 1, '<Location>{}</Location>'.format(esc_text(header)))
        if entry.get('namespace'):
            writer.add(depth + 1, '<Namespace>{}</Namespace>'.format(esc_text(entry['namespace'])))
        writer.add(depth + 1, '<ImportedObject>{}</ImportedObject>'.format(esc_text(entry['object'])))
    else:
        container = entry.get('container', 'Array')
        if container not in CONTAINERS:
            fail('container type "{}" is a "{}"; a container is one of {}'
                 .format(entry['name'], container, ', '.join(sorted(CONTAINERS))))
        value = vocab.check(entry.get('of'), 'container "{}"'.format(entry['name']))
        writer.add(depth + 1, '<Container>{}</Container>'.format(esc_text(container)))
        writer.add(depth + 1, '<BaseTypeValue>{}</BaseTypeValue>'.format(esc_text(value)))
        if CONTAINERS[container]:
            key = vocab.check(entry.get('key'),
                              'container "{}", which is a {} and needs a "key"'
                              .format(entry['name'], container))
            writer.add(depth + 1, '<BaseTypeKey>{}</BaseTypeKey>'.format(esc_text(key)))
        elif entry.get('key'):
            fail('container "{}" is an {}, which carries no key'.format(entry['name'], container))
    writer.add(depth, '</DataType>')


def write_datatypes(writer, depth, types, vocab):
    """The DataTypeList every document kind carries."""
    if not types:
        return
    writer.add(depth, '<DataTypeList>')
    for entry in types:
        write_datatype(writer, depth + 1, entry, vocab)
    writer.add(depth, '</DataTypeList>')


def write_constants(writer, depth, constants, vocab):
    """The ConstantList a .siml and a .fsml carry, written the same way in both."""
    if not constants:
        return
    writer.add(depth, '<ConstantList>')
    for entry in constants:
        if entry.get('value') is None:
            fail('constant "{}" has no "value"'.format(entry['name']))
        ctype = vocab.check(entry.get('type'), 'constant "{}"'.format(entry['name']))
        head = '<Constant ID="{}" Name="{}" DataType="{}" Value="{}"'.format(
            writer.reserve(('const', entry['name'])), esc(entry['name']),
            esc(ctype), esc(entry['value']))
        if entry.get('description'):
            writer.add(depth + 1, head + '>')
            described(writer, depth + 2, entry)
            writer.add(depth + 1, '</Constant>')
        else:
            writer.add(depth + 1, head + '/>')
    writer.add(depth, '</ConstantList>')


def write_params(writer, depth, owner, kind, params, vocab):
    """A ParamList, whose entries a .siml method and a .fsml trigger spell alike."""
    if not params:
        return
    writer.add(depth, '<ParamList>')
    for param in params:
        ptype = vocab.check(param.get('type'),
                            'parameter "{}" of "{}"'.format(param['name'], owner))
        head = '<Parameter ID="{}" Name="{}" DataType="{}"'.format(
            writer.reserve(('param', kind, owner, param['name'])),
            esc(param['name']), esc(ptype))
        if param.get('default') is not None:
            head += ' Default="{}"'.format(esc(param['default']))
        if param.get('description'):
            writer.add(depth + 1, head + '>')
            described(writer, depth + 2, param)
            writer.add(depth + 1, '</Parameter>')
        else:
            writer.add(depth + 1, head + '/>')
    writer.add(depth, '</ParamList>')


def reserve_params(writer, kind, entries):
    """The identifier of every parameter, before a guard has to refer to one."""
    for entry in entries:
        for param in named_list(entry, 'params', entry['name']):
            writer.reserve(('param', kind, entry['name'], param['name']))


def write_includes(writer, depth, includes, allow_alias=False):
    """The IncludeList. A .fsml entry may carry the Alias a Submachine names."""
    if not includes:
        return
    writer.add(depth, '<IncludeList>')
    for entry in includes:
        head = '<Location ID="{}" Name="{}"'.format(writer.ident(), esc(entry['name']))
        if entry.get('alias'):
            if not allow_alias:
                fail('only a state machine include carries an alias; "{}" does not'
                     .format(entry['name']))
            head += ' Alias="{}" Version="{}"'.format(
                esc(entry['alias']), esc(entry.get('version', '1.0.0')))
        if entry.get('description'):
            writer.add(depth + 1, head + '>')
            described(writer, depth + 2, entry)
            writer.add(depth + 1, '</Location>')
        else:
            writer.add(depth + 1, head + '/>')
    writer.add(depth, '</IncludeList>')


def write_overview(writer, depth, spec, extra=''):
    """The Overview every document kind opens with."""
    writer.add(depth, '<Overview ID="{}" Name="{}" Version="{}"{}>'.format(
        writer.ident(), esc(spec['name']), esc(spec.get('version', '1.0.0')), extra))
    described(writer, depth + 1, spec)
    writer.add(depth, '</Overview>')


def write_method(writer, depth, entry, method_type, vocab, extra='', body=None,
                 ident=None):
    """One Method. A .siml spells Request/Response/Broadcast here and a .fsml
    Trigger/Action/Condition, and everything below the attribute line is the same."""
    head = '<Method ID="{}" Name="{}" MethodType="{}"{}'.format(
        ident if ident is not None else writer.ident(),
        esc(entry['name']), method_type, extra)
    params = named_list(entry, 'params', entry['name'])
    if not params and not entry.get('description') and not body:
        writer.add(depth, head + '/>')
        return
    writer.add(depth, head + '>')
    described(writer, depth + 1, entry)
    write_params(writer, depth + 1, entry['name'], method_type, params, vocab)
    if body:
        writer.add(depth + 1, '<Body><![CDATA[{}]]></Body>'.format(body))
    writer.add(depth, '</Method>')
