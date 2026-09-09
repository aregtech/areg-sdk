#!/usr/bin/env python3
# ===========================================================================
# Answers what a .fsml, .siml or .dtml document may contain, from the schemas
# in tools/schema. One name in, one short answer out.
#
#   python3 tools/schema_help.py State          # element: parents, attributes, children
#   python3 tools/schema_help.py State/@Kind    # attribute: the values it accepts
#   python3 tools/schema_help.py tGuardOp       # type: the enumeration, verbatim
#   python3 tools/schema_help.py --list --document fsml
#   python3 tools/schema_help.py --search history
#
# A name that appears in more than one document is answered for each of them,
# unless --document narrows it. An answer carries the schema's own wording in one
# line; --full prints the whole documentation block instead.
#
# Everything printed is read from tools/schema/*.xsd at run time and nothing is
# written down here, so an answer cannot disagree with the schema the generator
# validates against.
#
# Exit code 0 when every name was answered, 1 otherwise.
# ===========================================================================
import argparse
import os
import re
import sys
import textwrap
import xml.etree.ElementTree as ET

XS = '{http://www.w3.org/2001/XMLSchema}'
SCHEMA_DIR = os.path.join(os.path.dirname(os.path.abspath(__file__)), 'schema')
DOCUMENTS = ['fsml', 'siml', 'dtml']
WIDTH = 92


def combine(outer, inner):
    """The occurrence of an element under a compositor that has one of its own."""
    def low(one, two):
        if '0' in (one, two):
            return '0'
        return max(one, two, key=lambda value: int(value))

    def high(one, two):
        if 'unbounded' in (one, two):
            return 'unbounded'
        return max(one, two, key=lambda value: int(value))

    return (low(outer[0], inner[0]), high(outer[1], inner[1]))


def tag_of(node):
    """The local name of a schema node, without the XSD namespace."""
    return node.tag[len(XS):] if node.tag.startswith(XS) else node.tag


def text_of(node):
    """The documentation a schema node carries, as one line of plain text."""
    parts = []
    for annotation in node.findall(XS + 'annotation'):
        for documentation in annotation.findall(XS + 'documentation'):
            parts.append(' '.join((documentation.text or '').split()))
    return ' '.join(part for part in parts if part)


def local(name):
    """A type reference without its namespace prefix."""
    return name.split(':')[-1] if name else ''


class Schema(object):
    """One document format, indexed by the names a document writes."""

    def __init__(self, document, path):
        self.document = document
        self.path = path
        self.simple = {}
        self.complex = {}
        self.groups = {}
        self.attribute_groups = {}
        self.parent = {}
        self.decls = {}      #!< element name -> the ways it is declared
        self.owners = {}     #!< complexType name -> element names declared with it
        self._read()
        self._index()

    def _read(self):
        self.root = ET.parse(self.path).getroot()
        for node in self.root.iter():
            for child in node:
                self.parent[child] = node
        for node in self.root:
            name = node.get('name')
            if not name:
                continue
            kind = tag_of(node)
            if kind == 'simpleType':
                self.simple[name] = node
            elif kind == 'complexType':
                self.complex[name] = node
            elif kind == 'group':
                self.groups[name] = node
            elif kind == 'attributeGroup':
                self.attribute_groups[name] = node

    def _index(self):
        """Every element the schema declares, wherever it is declared.

        An element carries either a named type or an inline one, and sits either
        in another element, in a named type, in a group, or at the document root.
        """
        for node in self.root.iter(XS + 'element'):
            name = node.get('name')
            if not name:
                continue
            type_name = local(node.get('type'))
            body = self.complex.get(type_name) if type_name \
                else node.find(XS + 'complexType')
            self.decls.setdefault(name, []).append(
                {'name': name, 'type': type_name, 'node': body,
                 'min': node.get('minOccurs', '1'),
                 'max': node.get('maxOccurs', '1'),
                 'doc': text_of(node),
                 'where': self._container(node)})
            if type_name:
                self.owners.setdefault(type_name, []).append(name)

    def _container(self, node):
        """What holds this declaration: an element, a named type, a group or the root."""
        walk = self.parent.get(node)
        while walk is not None:
            kind = tag_of(walk)
            name = walk.get('name')
            if kind == 'element' and name:
                return ('element', name)
            if kind in ('complexType', 'group') and name:
                return (kind, name)
            walk = self.parent.get(walk)
        return ('root', '')

    def holders(self, where, seen=None):
        """The element names a container stands for."""
        kind, name = where
        if kind == 'root':
            return ['document root']
        if kind == 'element':
            return [name]
        seen = seen or set()
        if name in seen:
            return []
        seen.add(name)
        if kind == 'complexType':
            found = list(self.owners.get(name, []))
            return found or [name]
        found = []
        for reference in self.root.iter(XS + 'group'):
            if local(reference.get('ref')) == name:
                found.extend(self.holders(self._container(reference), seen))
        return found or [name]

    def content_of(self, node, outer=('1', '1'), alternative=False):
        """The elements a type declares directly, in document order.

        A compositor carries its own occurrence and the elements under it carry
        theirs, so the two are combined: children of an optional choice are
        optional however the elements themselves are written.
        """
        if node is None:
            return []
        found = []
        for child in node:
            kind = tag_of(child)
            bounds = (child.get('minOccurs', '1'), child.get('maxOccurs', '1'))
            if kind == 'element':
                name = child.get('name') or local(child.get('ref'))
                low, high = combine(outer, bounds)
                found.append({'name': name, 'min': low, 'max': high,
                              'choice': alternative})
            elif kind in ('sequence', 'all', 'choice'):
                found.extend(self.content_of(child, combine(outer, bounds),
                                             kind == 'choice'))
            elif kind in ('complexContent', 'simpleContent', 'extension',
                          'restriction'):
                found.extend(self.content_of(child, outer, alternative))
            elif kind == 'group':
                reference = local(child.get('ref'))
                if reference in self.groups:
                    found.extend(self.content_of(self.groups[reference],
                                                 combine(outer, bounds),
                                                 alternative))
        return found

    def attributes_of(self, node):
        """The attributes a type carries, its own, its groups' and its base's."""
        if node is None:
            return []
        found = []
        for child in node:
            kind = tag_of(child)
            if kind == 'attribute' and child.get('name'):
                found.append({'name': child.get('name'),
                              'type': local(child.get('type')) or 'string',
                              'use': child.get('use', 'optional'),
                              'default': child.get('default'),
                              'doc': text_of(child)})
            elif kind == 'attributeGroup':
                reference = local(child.get('ref'))
                if reference in self.attribute_groups:
                    found.extend(self.attributes_of(self.attribute_groups[reference]))
            elif kind in ('complexContent', 'simpleContent', 'sequence', 'all',
                          'choice', 'restriction'):
                found.extend(self.attributes_of(child))
            elif kind == 'extension':
                base = self.complex.get(local(child.get('base')))
                found.extend(self.attributes_of(base))
                found.extend(self.attributes_of(child))
        return found

    def values_of(self, type_name):
        """The values a simpleType allows, following restrictions and unions."""
        node = self.simple.get(type_name)
        if node is None:
            return []
        found = []
        for enumeration in node.iter(XS + 'enumeration'):
            found.append(enumeration.get('value'))
        if not found:
            for union in node.iter(XS + 'union'):
                for member in (union.get('memberTypes') or '').split():
                    found.extend(self.values_of(local(member)))
            for restriction in node.iter(XS + 'restriction'):
                base = local(restriction.get('base'))
                if base in self.simple and base != type_name:
                    found.extend(self.values_of(base))
        return found

    def shape_of(self, type_name):
        """What a simpleType accepts when it is not an enumeration."""
        node = self.simple.get(type_name)
        if node is None:
            return ''
        for restriction in node.iter(XS + 'restriction'):
            base = local(restriction.get('base'))
            for pattern in restriction.iter(XS + 'pattern'):
                return '{}, pattern {}'.format(base, pattern.get('value'))
            return base
        for union in node.iter(XS + 'union'):
            return 'one of ' + (union.get('memberTypes') or '')
        return ''

    def uses_of(self, name):
        """The declarations of an element, one per distinct shape it has."""
        merged = []
        for decl in self.decls.get(name, []):
            for kept in merged:
                if kept['type'] == decl['type'] and kept['node'] is decl['node']:
                    kept['where'].extend(self.holders(decl['where']))
                    kept['doc'] = kept['doc'] or decl['doc']
                    break
            else:
                merged.append(dict(decl, where=self.holders(decl['where'])))
        return merged

    def names(self):
        """Every name this schema answers to."""
        return sorted(set(list(self.decls) + list(self.simple) + list(self.complex)))


def load(documents):
    """The schemas asked for, in the order they were asked for."""
    loaded = []
    for document in documents:
        path = os.path.join(SCHEMA_DIR, document + '.xsd')
        if not os.path.isfile(path):
            raise SystemExit('cannot read the schema {}: it is the only place the '
                             'format is written down. Restore it from the '
                             'repository.'.format(path))
        try:
            loaded.append(Schema(document, path))
        except ET.ParseError as failure:
            raise SystemExit('cannot read the schema {}: {}'.format(path, failure))
    return loaded


def one_line(text, limit=200):
    """The first sentence of a documentation block, capped."""
    if not text:
        return ''
    sentence = re.split(r'(?<=[.;])\s', text)[0]
    if len(sentence) > limit:
        sentence = sentence[:limit].rsplit(' ', 1)[0] + ' ...'
    return sentence


def emit(label, text, full):
    """One wrapped block under its label, empty text printing nothing."""
    if not text:
        return
    body = text if full else one_line(text)
    lines = textwrap.wrap(body, width=WIDTH - len(label) - 4,
                          break_on_hyphens=False)
    pad = ' ' * (len(label) + 4)
    print('    {} {}'.format(label, lines[0]))
    for line in lines[1:]:
        print(pad + line)


def occurrence(child):
    """How many times a child element may appear, as a suffix."""
    low, high = child['min'], child['max']
    if low == '0' and high == 'unbounded':
        return '*'
    if low == '1' and high == 'unbounded':
        return '+'
    if low == '0' and high == '1':
        return '?'
    if low == high == '1':
        return ''
    return '{' + (low if low == high else low + '..' + high) + '}'


def show_attributes(schema, node, full):
    """The attribute table of a type, with the values each attribute accepts."""
    attributes = schema.attributes_of(node)
    if not attributes:
        return
    width = max(len(a['name']) for a in attributes)
    print('    attributes:')
    for attribute in attributes:
        values = schema.values_of(attribute['type'])
        if values:
            accepts = ' | '.join(values)
        else:
            accepts = schema.shape_of(attribute['type']) or attribute['type']
        mark = '*' if attribute['use'] == 'required' else ' '
        default = ('  default {}'.format(attribute['default'])
                   if attribute['default'] else '')
        print('      {}{:<{w}}  {}{}'.format(mark, attribute['name'], accepts,
                                             default, w=width))
        if full:
            emit('      ', attribute['doc'], True)
    print('      (* required)')


def show_children(schema, node):
    """The elements a type may hold, in order, with how many of each."""
    children = schema.content_of(node)
    if children:
        print('    children: ' + ' '.join(
            child['name'] + occurrence(child) for child in children))
        if any(child['choice'] for child in children):
            print('    (children marked * or ? that share a choice are alternatives)')


def show_element(schema, name, full):
    """What an element is: where it goes, what it carries, what it holds."""
    for use in schema.uses_of(name):
        print('{}  (.{})  type {}  in {}'.format(
            name, schema.document, use['type'] or 'inline',
            ', '.join(sorted(set(use['where'])))))
        emit('what:', use['doc'] or text_of(use['node']) if use['node'] is not None
             else use['doc'], full)
        show_attributes(schema, use['node'], full)
        show_children(schema, use['node'])
        show_meanings(schema, use['node'])
        print('')


def show_meanings(schema, node):
    """Where the meaning of each enumerated attribute is written down."""
    named = sorted(set(attribute['type']
                       for attribute in schema.attributes_of(node)
                       if schema.values_of(attribute['type'])
                       and text_of(schema.simple[attribute['type']])))
    if named:
        print('    what the values mean: schema_help.py --full ' + ' '.join(named))


def show_type(schema, name, full):
    """What a type accepts: an enumeration verbatim, or the shape it restricts."""
    if name in schema.simple:
        print('{}  (.{})  simpleType'.format(name, schema.document))
        emit('what:', text_of(schema.simple[name]), full)
        values = schema.values_of(name)
        if values:
            print('    values: ' + ' | '.join(values))
        else:
            print('    accepts: ' + (schema.shape_of(name) or 'any text'))
        used_by = sorted(set(
            '{}/@{}'.format(element, attribute['name'])
            for type_name, node in schema.complex.items()
            for attribute in schema.attributes_of(node)
            if attribute['type'] == name
            for element in schema.owners.get(type_name, [])))
        if used_by:
            print('    used by: ' + ' '.join(used_by[:12]))
        print('')
        return True
    if name in schema.complex:
        print('{}  (.{})  complexType, carried by {}'.format(
            name, schema.document,
            ', '.join(schema.owners.get(name, [])) or 'nothing'))
        emit('what:', text_of(schema.complex[name]), full)
        show_attributes(schema, schema.complex[name], full)
        show_children(schema, schema.complex[name])
        print('')
        return True
    return False


def show_attribute(schema, element, attribute_name, full):
    """One attribute of one element: what it accepts and whether it is required."""
    answered = False
    for use in schema.uses_of(element):
        for attribute in schema.attributes_of(use['node']):
            if attribute['name'] != attribute_name:
                continue
            values = schema.values_of(attribute['type'])
            print('{}/@{}  (.{})  type {}  {}'.format(
                element, attribute_name, schema.document, attribute['type'],
                attribute['use']))
            if values:
                print('    values: ' + ' | '.join(values))
            else:
                print('    accepts: ' + (schema.shape_of(attribute['type'])
                                         or attribute['type']))
            if attribute['default']:
                print('    default: ' + attribute['default'])
            emit('what:', attribute['doc'] or text_of(
                schema.simple.get(attribute['type'], ET.Element('x'))), full)
            print('')
            answered = True
    return answered


def show_named_attribute(schema, attribute_name, full):
    """Every element that carries an attribute of this name, and what it accepts."""
    seen = {}
    for element in sorted(schema.decls):
        for use in schema.uses_of(element):
            for attribute in schema.attributes_of(use['node']):
                if attribute['name'] != attribute_name:
                    continue
                values = schema.values_of(attribute['type'])
                accepts = (' | '.join(values) if values
                           else schema.shape_of(attribute['type'])
                           or attribute['type'])
                key = (attribute['type'], accepts, attribute['use'])
                seen.setdefault(key, []).append(element)
    if not seen:
        return False
    for (type_name, accepts, use), elements in sorted(seen.items()):
        print('@{}  (.{})  type {}  {}  on {}'.format(
            attribute_name, schema.document, type_name, use,
            ', '.join(sorted(set(elements)))))
        print('    accepts: ' + accepts)
        node = schema.simple.get(type_name)
        if node is not None:
            emit('what:', text_of(node), full)
        print('')
    return True


def answer(schemas, wanted, full):
    """Answers one name, whatever kind of name it is."""
    if '/@' in wanted:
        element, attribute_name = wanted.split('/@', 1)
        found = False
        for schema in schemas:
            found |= show_attribute(schema, element, attribute_name, full)
        return found
    found = False
    for schema in schemas:
        if wanted in schema.decls:
            show_element(schema, wanted, full)
            found = True
        elif show_type(schema, wanted, full):
            found = True
    if not found:
        for schema in schemas:
            found |= show_named_attribute(schema, wanted, full)
    return found


def near(schemas, wanted, limit=8):
    """The names that look like the one asked for."""
    lowered = wanted.lower()
    hits = []
    for schema in schemas:
        for name in schema.names():
            if lowered in name.lower():
                hits.append('{} (.{})'.format(name, schema.document))
    return hits[:limit]


def show_list(schemas):
    """Every element a document may carry, one line each."""
    for schema in schemas:
        print('== .{}'.format(schema.document))
        for name in sorted(schema.decls):
            where = sorted(set(holder
                               for use in schema.uses_of(name)
                               for holder in use['where']))
            print('  {:<20} in {}'.format(name, ', '.join(where)))
        print('  types: ' + ' '.join(sorted(schema.simple)))
        print('')


def show_search(schemas, needle, full):
    """Every name whose spelling or documentation carries the words asked for."""
    lowered = needle.lower()
    for schema in schemas:
        for name in schema.names():
            node = schema.simple.get(name)
            if node is None:
                node = schema.complex.get(name)
            body = text_of(node) if node is not None else ''
            if not body and name in schema.decls:
                body = schema.decls[name][0]['doc']
            if lowered in name.lower() or lowered in body.lower():
                print('  {:<24} (.{}) {}'.format(name, schema.document,
                                                 one_line(body, 80)))


# The names an agent asks for while authoring a document, so it can ask once instead
# of discovering the list a round trip at a time. Measured from three benchmark runs:
# every fsml name below was asked for in at least two of the three, and the four turns
# the shortest run spent on them collapse to one.
AUTHORING = {
    'fsml': ['Overview', 'StateMachine', 'State', 'State/@OnFinal', 'tStateKind',
             'tHistoryDepth', 'Transition', 'Guard', 'Cmp', 'Attr', 'Lit',
             'EntryList', 'ActionCall', 'OperationList', 'Method', 'Attribute',
             'Timer', 'TimerStart', 'Event', 'EventSend'],
    'siml': ['Overview', 'MethodList', 'Method', 'ParamList', 'Parameter',
             'AttributeList', 'Attribute', 'DataTypeList', 'DataType',
             'ConstantList', 'Constant', 'IncludeList'],
    'dtml': ['Overview', 'DataTypeList', 'DataType', 'FieldList', 'Field',
             'EnumEntry', 'Container'],
}


def authoring_hint(names, kinds):
    """The authoring bundle worth asking for, given the names already asked."""
    asked = set(names)
    scored = []
    for kind in kinds:
        wanted = set(AUTHORING[kind])
        if wanted <= asked:
            return None
        scored.append((len(asked & wanted), kind))
    if not scored:
        return None
    top = max(score for score, _ in scored)
    best = [kind for score, kind in scored if score == top]
    if len(best) == 1:
        return ('-- "--authoring {}" answers all {} names a .{} document needs, '
                'in one call.'.format(best[0], len(AUTHORING[best[0]]), best[0]))
    return ('-- "--authoring {}" answers every name that document kind needs, '
            'in one call.'.format('|'.join(best)))


def show_authoring(schemas, kind, full):
    """Every name authoring this document kind needs, in one answer."""
    missing = 0
    for wanted in AUTHORING[kind]:
        if not answer(schemas, wanted, full):
            missing += 1
    print('-- that is the authoring set for .{}. Ask by name for anything else.'
          .format(kind))
    return missing


def main():
    parser = argparse.ArgumentParser(
        description='Answer what a .fsml, .siml or .dtml document may contain.')
    parser.add_argument('names', nargs='*',
                        help='an element (State), an attribute (State/@Kind) or '
                             'a type (tStateKind)')
    parser.add_argument('--document', choices=DOCUMENTS, action='append',
                        help='answer for this document format only')
    parser.add_argument('--list', action='store_true',
                        help='list every element, with where it goes')
    parser.add_argument('--search', metavar='TEXT',
                        help='find a name from words in it or in its documentation')
    parser.add_argument('--full', action='store_true',
                        help='print the whole documentation instead of one line')
    parser.add_argument('--authoring', choices=sorted(AUTHORING),
                        help='every name authoring this document kind needs, at once')
    args = parser.parse_args()

    schemas = load(args.document or ([args.authoring] if args.authoring
                                     else DOCUMENTS))

    if args.authoring:
        return 1 if show_authoring(schemas, args.authoring, args.full) else 0

    if args.list:
        show_list(schemas)
        return 0

    if args.search:
        show_search(schemas, args.search, args.full)
        return 0

    if not args.names:
        parser.error('give one or more names, --authoring KIND, --search TEXT, '
                     'or --list')

    missing = 0
    for wanted in args.names:
        if not answer(schemas, wanted, args.full):
            print('{} -- no such element, attribute or type'.format(wanted))
            for hint in near(schemas, wanted):
                print('    did you mean: ' + hint)
            missing += 1

    hint = authoring_hint(args.names, args.document or sorted(AUTHORING))
    if hint:
        print(hint)

    return 1 if missing else 0


if __name__ == '__main__':
    sys.exit(main())
