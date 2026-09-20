#!/usr/bin/env python3
"""Write the .dtml, .siml and .fsml documents of a project from one JSON description.

The three grammars carry numeric IDs, and they refer to each other by those numbers: a
transition names its target state by ID, a guard names an attribute by ID. Assigning
and resolving them is bookkeeping that has nothing to do with the design, and it is
where an author spends effort and gets it wrong. This tool takes states, methods,
attributes and types BY NAME, writes every document, and refuses a name that was never
declared -- in any of the three documents, so the data types a service and its state
machine share are declared once and checked once.

    python3 tools/agent/gen_docs.py --spec design.json --outdir src/services
    python3 tools/agent/gen_docs.py --template design.json   # every key, empty, to fill
    python3 tools/agent/gen_docs.py --example            # a whole spec to copy
    python3 tools/agent/gen_docs.py --spec a.json --spec b.json --outdir src/services

A spec holds up to three sections, all of them optional:

    {
      "datatypes":  {"name": "SharedTypes", "declare": [ ... ]},   -> SharedTypes.dtml
      "interfaces": [ { ... } ],                                   -> one .siml each
      "machines":   [ { ... } ]                                    -> one .fsml each
    }

A large project splits across several spec files and passes each with its own --spec:
they are merged, so every interface sees the shared data types and the cross-document
checks still run over all of them.

Interfaces and machines may also declare types of their own, in "types", when only that
one document needs them. A type declared in "datatypes" is spelled Space::Type in an
interface or a machine, where Space is the data type document's name; the include is
added for you.
"""
import argparse
import difflib
import json
import os
import re
import sys
import xml.etree.ElementTree as ET

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))

# Rule 108 is decided in one place, the shared catalogue, and reported from two:
# here, before a document is written, and check_contract.py, after. Reading the
# number rather than writing it is what keeps the two from drifting apart.
RULES_XML = os.path.join(os.path.dirname(os.path.dirname(
    os.path.dirname(os.path.abspath(__file__)))), 'tools', 'schema', 'rules.xml')
FINAL_ENTRY_RULE = 'RULE_FINAL_ENTRY_ORDER'
WARNING_BAND = 100


def rule_number(name, band):
    """The number the shared catalogue gives a rule, or None when it cannot be read."""
    try:
        root = ET.parse(RULES_XML).getroot()
    except (ET.ParseError, OSError):
        return None
    for rule in root.iter('Rule'):
        if rule.get('Name') == name:
            return str(int(rule.get('Number')) + band)
    return None

import codegen_names  # noqa: E402
from docmodel import (CONTAINERS, PREDEFINED, TYPE_KINDS,
                      Vocabulary, Writer, described, esc, esc_text, fail, named_list,
                      reserve_params, spell, unique, write_constants,
                      write_datatypes, write_includes, write_method, write_overview,
                      write_params)

DTML_VERSION = '1.0.0'
SIML_VERSION = '1.1.0'
FSML_VERSION = '1.2.0'

CATEGORIES = ('Private', 'Public', 'Internet')
STATE_KINDS = {'normal': 'Normal', 'final': 'Final', 'history': 'History'}
GUARD_OPS = ('eq', 'ne', 'lt', 'le', 'gt', 'ge')
SOURCES = {'param': 'Param', 'attr': 'Attribute', 'const': 'Constant', 'expr': 'Expression',
           'raw': 'Expression'}

# The key of a note in a spec. The reader skips it wherever it appears.
NOTE = '#|'

# Every key each kind of spec object may carry. Any other key is refused.
KEYS = {
    'spec': ('datatypes', 'interfaces', 'machines'),
    'datatypes': ('name', 'description', 'version', 'declare', 'includes'),
    'interface': ('name', 'category', 'description', 'version', 'types', 'attributes',
                  'requests', 'responses', 'broadcasts', 'constants', 'includes',
                  'machine', 'steps', 'driver', 'timers'),
    'driver': ('connect_seconds', 'reconnect_seconds', 'stall_ticks'),
    'machine': ('name', 'description', 'version', 'threading', 'types', 'attributes',
                'constants', 'triggers', 'timers', 'events', 'actions', 'conditions',
                'submachines', 'includes', 'initial', 'states'),
    'enumerator': ('name', 'value', 'description'),
    'field': ('name', 'type', 'default', 'description'),
    'service attribute': ('name', 'type', 'notify', 'description'),
    'machine attribute': ('name', 'type', 'value', 'description'),
    'request': ('name', 'description', 'params', 'answer', 'answer_description', 'response'),
    'method': ('name', 'description', 'params'),
    'parameter': ('name', 'type', 'default', 'description', 'values'),
    'constant': ('name', 'type', 'value', 'description'),
    'include': ('name', 'description', 'alias', 'version'),
    'timer': ('name', 'timeout', 'repeat', 'description'),
    'service timer': ('name', 'timeout', 'repeat', 'start', 'description'),
    'condition': ('name', 'description', 'params', 'implement', 'body', 'return'),
    'submachine': ('name', 'path', 'version', 'description'),
    'state': ('name', 'kind', 'depth', 'description', 'entry', 'exit', 'transitions',
              'initial', 'final_event', 'states', 'submachine'),
    'transition': ('on', 'to', 'guard', 'set', 'do', 'description'),
    'step': ('name', 'description', 'send', 'args', 'await', 'wait'),
}
TYPE_KEYS = {
    'Enumeration': ('name', 'kind', 'description', 'values', 'derives'),
    'Structure': ('name', 'kind', 'description', 'fields'),
    'Container': ('name', 'kind', 'description', 'container', 'of', 'key'),
    'Imported': ('name', 'kind', 'description', 'header', 'location', 'object', 'namespace'),
}
# An object step and an object guard node, by the key that says what it is, in the
# order write_operation and write_guard_node look for it.
STEP_KEYS = {'call': ('call', 'args'), 'send': ('send', 'args'), 'start': ('start',),
             'stop': ('stop',), 'set': ('set', 'to')}
GUARD_KEYS = {'all': ('all',), 'any': ('any',), 'not': ('not',), 'call': ('call', 'args')}
SINGULAR = {'attributes': 'attribute', 'requests': 'request', 'responses': 'response',
            'broadcasts': 'broadcast', 'constants': 'constant', 'includes': 'include',
            'params': 'parameter', 'answer': 'answer parameter', 'declare': 'type',
            'types': 'type', 'values': 'enumerator', 'fields': 'field', 'events': 'event',
            'timers': 'timer', 'triggers': 'trigger', 'actions': 'action',
            'conditions': 'condition', 'submachines': 'submachine', 'steps': 'step',
            'states': 'state', 'interfaces': 'service interface', 'machines': 'state machine',
            'datatypes': 'data type document'}


# ----------------------------------------------------------------------------- shared

def declared_types(spec, where):
    types = named_list(spec, 'types', where)
    unique(types, '"types" of ' + where)
    return types


def vocabulary_for(spec, shared_space, shared_names, where, prefix=''):
    types = declared_types(spec, where)
    vocab = Vocabulary(shared_space, shared_names, prefix)
    vocab.declare(set(t['name'] for t in types))
    return types, vocab


def shared_includes(spec, vocab, extra):
    """The IncludeList entries: the data type document when it is used, then the rest."""
    entries = []
    if vocab.space and uses_shared_space(spec, vocab):
        entries.append({'name': '{}{}.dtml'.format(vocab.prefix, vocab.space),
                        'description': 'The data types this project declares once.'})
    entries.extend(named_list(spec, 'includes', spec['name']))
    entries.extend(extra)
    return entries


def uses_shared_space(spec, vocab):
    """Whether anything in this document spells a type of the shared document."""
    prefix = vocab.space + '::'
    return prefix in json.dumps(spec)


# ------------------------------------------------------------------------------- dtml

def build_dtml(spec):
    if not spec.get('name'):
        fail('"datatypes" has no "name"; it is the namespace every host spells')
    where = 'the data type document "{}"'.format(spec['name'])
    types = named_list(spec, 'declare', where)
    unique(types, where)
    if not types:
        fail('{} declares nothing'.format(where))
    vocab = Vocabulary()
    vocab.declare(set(t['name'] for t in types))

    writer = Writer()
    writer.add(0, '<?xml version="1.0" encoding="UTF-8" standalone="yes"?>')
    writer.add(0, '<DataTypeDocument FormatVersion="{}">'.format(DTML_VERSION))
    write_overview(writer, 1, spec)
    write_datatypes(writer, 1, types, vocab)
    write_includes(writer, 1, named_list(spec, 'includes', where))
    writer.add(0, '</DataTypeDocument>')
    return writer.text()


# ------------------------------------------------------------------------------- siml

# What codegen.jar writes in front of a name of each kind. A name that already starts
# with one of these is written twice: request "request_get" becomes request_request_get().
GENERATED_PREFIXES = {
    'request': ('request_', 'response_', 'broadcast_'),
    'response': ('request_', 'response_', 'broadcast_'),
    'broadcast': ('request_', 'response_', 'broadcast_'),
    'attribute': ('on_', 'notify_on_'),
}
GENERATED_SPELLING = {
    'request': 'request_{}()',
    'response': 'response_{}()',
    'broadcast': 'broadcast_{}()',
    'attribute': 'on_{}_update()',
}


def check_generated_prefixes(entries, kind, where):
    """Refuses a name that starts with a prefix the generator adds to it."""
    for entry in entries:
        name = entry['name']
        for prefix in GENERATED_PREFIXES[kind]:
            if name.startswith(prefix) and len(name) > len(prefix):
                fail('{} "{}" of {} starts with "{}", which the generator adds itself: it '
                     'would be generated as {}. Name it "{}".'
                     .format(kind, name, where, prefix,
                             GENERATED_SPELLING[kind].format(name), name[len(prefix):]))


def build_siml(spec, shared_space, shared_names, prefix=''):
    if not spec.get('name'):
        fail('every entry of "interfaces" needs a "name"')
    where = 'the service interface "{}"'.format(spec['name'])
    types, vocab = vocabulary_for(spec, shared_space, shared_names, where, prefix)

    category = spec.get('category', 'Public')
    if category not in CATEGORIES:
        fail('{} is "{}"; a category is one of {}'
             .format(where, category, ', '.join(CATEGORIES)))

    attributes = named_list(spec, 'attributes', where)
    requests = named_list(spec, 'requests', where)
    responses = named_list(spec, 'responses', where)
    broadcasts = named_list(spec, 'broadcasts', where)
    constants = named_list(spec, 'constants', where)
    unique(attributes, where)
    unique(broadcasts, where)
    check_generated_prefixes(requests, 'request', where)
    check_generated_prefixes(responses, 'response', where)
    check_generated_prefixes(broadcasts, 'broadcast', where)
    check_generated_prefixes(attributes, 'attribute', where)

    # A request that carries "answer" declares its response here, so the two names
    # cannot drift apart.
    named = set(r['name'] for r in responses)
    for request in requests:
        if request.get('answer') is None:
            continue
        if request['name'] in named:
            fail('request "{}" of {} carries "answer" and there is also a response of '
                 'that name; write one or the other'.format(request['name'], where))
        responses.append({'name': request['name'], 'params': request['answer'],
                          'description': request.get('answer_description',
                                                     'The answer to {}.'.format(request['name']))})
        request['response'] = request['name']

    for response in responses:
        if response['name'] not in set(r['name'] for r in requests):
            fail('response "{}" of {} answers no request of that name'
                 .format(response['name'], where))

    writer = Writer()
    writer.add(0, '<?xml version="1.0" encoding="UTF-8" standalone="yes"?>')
    writer.add(0, '<ServiceInterface FormatVersion="{}">'.format(SIML_VERSION))
    write_overview(writer, 1, spec, ' Category="{}"'.format(category))
    write_datatypes(writer, 1, types, vocab)

    if attributes:
        writer.add(1, '<AttributeList>')
        for entry in attributes:
            atype = vocab.check(entry.get('type'),
                                'attribute "{}" of {}'.format(entry['name'], where))
            notify = entry.get('notify', 'OnChange')
            if notify not in ('OnChange', 'Always'):
                fail('attribute "{}" of {} notifies "{}"; it is OnChange or Always'
                     .format(entry['name'], where, notify))
            head = '<Attribute ID="{}" Name="{}" DataType="{}" Notify="{}"'.format(
                writer.reserve(('attr', entry['name'])), esc(entry['name']),
                esc(atype), notify)
            if entry.get('description'):
                writer.add(2, head + '>')
                described(writer, 3, entry)
                writer.add(2, '</Attribute>')
            else:
                writer.add(2, head + '/>')
        writer.add(1, '</AttributeList>')

    if requests or responses or broadcasts:
        writer.add(1, '<MethodList>')
        for entry in requests:
            extra = ''
            if entry.get('response'):
                if entry['response'] not in set(r['name'] for r in responses):
                    fail('request "{}" of {} names response "{}", which is not declared'
                         .format(entry['name'], where, entry['response']))
                extra = ' Response="{}"'.format(esc(entry['response']))
            write_method(writer, 2, entry, 'Request', vocab, extra)
        for entry in responses:
            write_method(writer, 2, entry, 'Response', vocab)
        for entry in broadcasts:
            write_method(writer, 2, entry, 'Broadcast', vocab)
        writer.add(1, '</MethodList>')

    write_constants(writer, 1, constants, vocab)
    write_includes(writer, 1, shared_includes(spec, vocab, []))
    writer.add(0, '</ServiceInterface>')
    return writer.text()


# ------------------------------------------------------------------------------- fsml

class Machine:
    """The machine spec, checked, with an identifier reserved for everything named."""

    def __init__(self, spec, shared_space, shared_names, prefix=''):
        if not spec.get('name'):
            fail('every entry of "machines" needs a "name"')
        self.spec = spec
        self.where = 'the state machine "{}"'.format(spec['name'])
        self.types, self.vocab = vocabulary_for(spec, shared_space, shared_names,
                                               self.where, prefix)

        self.attributes = named_list(spec, 'attributes', self.where)
        self.constants = named_list(spec, 'constants', self.where)
        self.events = named_list(spec, 'events', self.where)
        self.timers = named_list(spec, 'timers', self.where)
        self.triggers = named_list(spec, 'triggers', self.where)
        self.actions = named_list(spec, 'actions', self.where)
        self.conditions = named_list(spec, 'conditions', self.where)
        self.submachines = named_list(spec, 'submachines', self.where)
        if not self.actions and not self.conditions:
            fail('{} has no action, so it generates nothing to implement'.format(self.where))

        self.attribute_names = unique(self.attributes, self.where)
        self.attribute_types = dict((a['name'], a.get('type'))
                                    for a in self.attributes if isinstance(a, dict))
        self.constant_names = unique(self.constants, self.where)
        self.timer_names = unique(self.timers, self.where)
        self.event_names = unique(self.events, self.where)
        self.trigger_names = unique(self.triggers, self.where)
        self.action_names = unique(self.actions, self.where)
        self.condition_names = unique(self.conditions, self.where)
        self.submachine_names = unique(self.submachines, self.where)

        clash = (self.timer_names & self.event_names) | (self.timer_names & self.trigger_names) \
            | (self.event_names & self.trigger_names)
        if clash:
            fail('a timer, an event and a trigger cannot share a name: {}'
                 .format(', '.join(sorted(clash))))
        clash = self.attribute_names & self.constant_names
        if clash:
            fail('an attribute and a constant cannot share a name, because a guard names '
                 'either of them the same way: {}'.format(', '.join(sorted(clash))))

        self.states = spec.get('states') or []
        if not self.states:
            fail('{} has no "states"'.format(self.where))
        self.state_names = set()
        collect_states(self.states, self.state_names, self.where)

        # A "final_event" is by definition an event this machine sends to itself, so
        # declaring it twice is bookkeeping. Anything a state ends on and "events" does
        # not carry is declared here, in the order the states name it.
        for name in final_events(self.states):
            if name not in self.event_names:
                self.events.append({'name': name,
                                    'description': 'Sent when the nested level of a '
                                                   'composite reaches its Final state.'})
                self.event_names.add(name)

        self.params_of = {}
        # The declared type of each of them, so a value written for a String is spelt
        # as C++ spells one.
        self.types_of = {}
        for kind, entries in (('Trigger', self.triggers), ('Event', self.events),
                              ('Action', self.actions), ('Condition', self.conditions)):
            for entry in entries:
                declared = named_list(entry, 'params', entry['name'])
                self.params_of[(kind, entry['name'])] = [p['name'] for p in declared]
                self.types_of[(kind, entry['name'])] = \
                    dict((p['name'], p.get('type')) for p in declared)

    def stimulus_kind(self, name):
        if name in self.trigger_names:
            return 'Trigger'
        if name in self.timer_names:
            return 'Timer'
        if name in self.event_names:
            return 'Event'
        fail('"{}" is named by a transition of {} but is not a trigger, a timer or an '
             'event'.format(name, self.where))


def final_events(states):
    """Every event a state ends on, in the order the document names them."""
    found = []
    for state in states:
        if state.get('final_event') and state['final_event'] not in found:
            found.append(state['final_event'])
        found.extend(n for n in final_events(state.get('states') or [])
                     if n not in found)
    return found


def collect_states(states, seen, where):
    """Every state name in the tree, so a transition target can be checked."""
    for state in states:
        name = state.get('name')
        if not name:
            fail('every state of {} needs a name'.format(where))
        if name in seen:
            fail('two states of {} are named "{}"; a state name is unique document-wide'
                 .format(where, name))
        seen.add(name)
        if state.get('states') and state.get('submachine'):
            fail('state "{}" carries both "states" and "submachine"; it hosts one or the '
                 'other, never both'.format(name))
        collect_states(state.get('states') or [], seen, where)


def reserve_all(machine, writer, states):
    for state in states:
        writer.reserve(('state', state['name']))
        reserve_all(machine, writer, state.get('states') or [])


# The document types whose values are written in C++ as a quoted literal.
TEXT_TYPES = ('String', 'WideString')


def as_cpp_text(text, type_name):
    """A literal value as C++ spells it for its declared type.

    Text for a String is quoted here: a design says what a value is, not how C++
    writes one, and an unquoted CARD-0001 reaches the compiler as a token.
    """
    if type_name not in TEXT_TYPES:
        return text
    if len(text) > 1 and text.startswith('"') and text.endswith('"'):
        return text
    return '"{}"'.format(text.replace(chr(92), chr(92) * 2).replace('"', chr(92) + '"'))


def source_of(machine, value, stimulus, where, type_name=None):
    """How a written value reaches the generated code: a declared name or a literal.

    "param:x", "attr:X", "const:X", "raw:<c++>" (or "expr:<c++>") and "lit:<text>" say
    it outright. A bare name that was declared is that declaration; anything else is a
    literal, and a literal of a String parameter is quoted here. A JSON true or false is the C++
    token of that name, not Python's spelling of it.
    """
    text = spell(value)
    prefix, sep, rest = text.partition(':')
    if prefix == 'lit' and sep:
        return 'Value', as_cpp_text(rest, type_name)
    if prefix in SOURCES and sep and not rest:
        fail('{} writes "{}:" with nothing after the colon. Name what it reads, or '
             'write "lit:" for an empty value.'.format(where, prefix))
    if prefix in SOURCES and rest:
        if prefix == 'param' and stimulus and rest not in machine.params_of.get(stimulus, []):
            fail('{} reads parameter "{}", which "{}" does not declare. It declares: {}'
                 .format(where, rest, stimulus[1],
                         ', '.join(machine.params_of.get(stimulus, [])) or 'none'))
        if prefix == 'attr' and rest not in machine.attribute_names:
            fail('{} reads attribute "{}", which is not declared'.format(where, rest))
        if prefix == 'const' and rest not in machine.constant_names:
            fail('{} reads constant "{}", which is not declared'.format(where, rest))
        return SOURCES[prefix], rest
    if stimulus and text in machine.params_of.get(stimulus, []):
        return 'Param', text
    if text in machine.attribute_names:
        return 'Attribute', text
    if text in machine.constant_names:
        return 'Constant', text
    return 'Value', as_cpp_text(text, type_name)


def write_arguments(machine, writer, depth, owner_kind, owner, args, stimulus, where):
    """The ArgumentList an ActionCall or an EventSend carries."""
    declared = machine.params_of.get((owner_kind, owner), [])
    types = machine.types_of.get((owner_kind, owner), {})
    if not args:
        return
    if not isinstance(args, dict):
        fail('{} passes arguments as {{"parameter": "value"}}'.format(where))
    for name in args:
        if name not in declared:
            fail('{} passes "{}", which "{}" does not declare. It declares: {}'
                 .format(where, name, owner, ', '.join(declared) or 'none'))
    writer.add(depth, '<ArgumentList>')
    for name, value in args.items():
        kind, text = source_of(machine, value, stimulus, where, types.get(name))
        head = '<Argument ID="{}" Name="{}" Source="{}"'.format(
            writer.ident(), esc(name), kind)
        if kind == 'Expression':
            writer.add(depth + 1, head + '>')
            writer.add(depth + 2, '<Expression><![CDATA[{}]]></Expression>'.format(text))
            writer.add(depth + 1, '</Argument>')
        else:
            writer.add(depth + 1, head + ' Value="{}"/>'.format(esc(text)))
    writer.add(depth, '</ArgumentList>')


def write_operation(machine, writer, depth, step, stimulus, where):
    """One entry of an OperationList, however the spec spells it.

    A string is "action", "start Timer", "stop Timer" or "send Event". An object is
    {"call": ...}, {"send": ...}, {"start": ...}, {"stop": ...} or {"set": ...}, and
    "call" and "send" may add "args".
    """
    if isinstance(step, str):
        verb, _, rest = step.partition(' ')
        if verb in ('start', 'stop', 'send') and rest:
            step = {verb: rest}
        else:
            step = {'call': step}
    if not isinstance(step, dict):
        fail('{} runs {!r}, which is not a step'.format(where, step))

    if 'call' in step:
        name = step['call']
        if name not in machine.action_names:
            fail('{} calls "{}", which is not in "actions". It declares: {}'
                 .format(where, name, ', '.join(sorted(machine.action_names))))
        args = step.get('args')
        head = '<ActionCall ID="{}" Action="{}"'.format(writer.ident(), esc(name))
        if not args:
            writer.add(depth, head + '/>')
        else:
            writer.add(depth, head + '>')
            write_arguments(machine, writer, depth + 1, 'Action', name, args, stimulus, where)
            writer.add(depth, '</ActionCall>')
    elif 'send' in step:
        name = step['send']
        if name not in machine.event_names:
            fail('{} sends "{}", which is not in "events"'.format(where, name))
        args = step.get('args')
        head = '<EventSend ID="{}" Event="{}"'.format(writer.ident(), esc(name))
        if not args:
            writer.add(depth, head + '/>')
        else:
            writer.add(depth, head + '>')
            write_arguments(machine, writer, depth + 1, 'Event', name, args, stimulus, where)
            writer.add(depth, '</EventSend>')
    elif 'start' in step or 'stop' in step:
        verb = 'start' if 'start' in step else 'stop'
        name = step[verb]
        if name not in machine.timer_names:
            fail('{} {}s "{}", which is not in "timers"'.format(where, verb, name))
        writer.add(depth, '<Timer{} ID="{}" Timer="{}"/>'.format(
            'Start' if verb == 'start' else 'Stop', writer.ident(), esc(name)))
    elif 'set' in step:
        write_assignment(machine, writer, depth, step['set'], step.get('to'), stimulus, where)
    else:
        fail('{} runs {!r}: a step calls, sends, starts, stops or sets'.format(where, step))


def write_assignment(machine, writer, depth, attribute, value, stimulus, where):
    if attribute not in machine.attribute_names:
        fail('{} assigns "{}", which is not in "attributes"'.format(where, attribute))
    kind, text = source_of(machine, value, stimulus, where,
                           machine.attribute_types.get(attribute))
    head = '<AttributeSet ID="{}" Attribute="{}" Source="{}"'.format(
        writer.ident(), esc(attribute), kind)
    if kind == 'Expression':
        writer.add(depth, head + '>')
        writer.add(depth + 1, '<Expression><![CDATA[{}]]></Expression>'.format(text))
        writer.add(depth, '</AttributeSet>')
    else:
        writer.add(depth, head + ' Value="{}"/>'.format(esc(text)))


def write_operation_list(machine, writer, depth, tag, steps, assignments, stimulus, where):
    if not steps and not assignments:
        return
    # "set" runs before "do", so an action already sees the attributes this transition
    # assigns. A step that has to run earlier is written as {"set": ...} inside "do".
    writer.add(depth, '<{}>'.format(tag))
    for attribute, value in (assignments or {}).items():
        write_assignment(machine, writer, depth + 1, attribute, value, stimulus, where)
    for step in steps or []:
        write_operation(machine, writer, depth + 1, step, stimulus, where)
    writer.add(depth, '</{}>'.format(tag))


def condition_params(machine, name):
    """What one condition declares: each parameter, and whether it carries a Default."""
    for entry in machine.conditions:
        if entry['name'] == name:
            return [(p['name'], p.get('default') is not None)
                    for p in named_list(entry, 'params', name)]
    return []


def guard_call_arguments(machine, name, args, where):
    """Each declared parameter of a guard call paired with the value bound to it.

    The generator binds an Arg to a formal by the formal's document ID and refuses a
    formal that is neither mapped nor defaulted. Checking that here means the refusal
    names the spec the author wrote, not the XML this tool generated from it.
    """
    declared = condition_params(machine, name)
    names = [param for param, _ in declared]
    if isinstance(args, dict):
        for key in args:
            if key not in names:
                fail('{} passes "{}" to "{}", which does not declare it. It declares: '
                     '{}'.format(where, key, name, ', '.join(names) or 'no parameters'))
        given = dict(args)
    elif isinstance(args, (list, tuple)):
        if len(args) > len(declared):
            fail('{} passes {} argument(s) to "{}", which declares {}: {}'
                 .format(where, len(args), name, len(declared),
                         ', '.join(names) or 'no parameters'))
        given = dict(zip(names, args))
    elif args is None:
        given = {}
    else:
        fail('{}: the arguments of a guard call are a list in the order "{}" declares '
             'its parameters, or a {{"parameter": value}} object; got {}'
             .format(where, name, type(args).__name__))
    unmapped = [param for param, has_default in declared
                if param not in given and not has_default]
    if unmapped:
        fail('{} calls "{}" and maps nothing to {}, which declare(s) no default. '
             '"{}" declares: {}. Give a value for each, as a list in that order or as '
             '{{"parameter": value}}, or give the parameter a "default".'
             .format(where, name, ', '.join('"%s"' % u for u in unmapped), name,
                     ', '.join(names)))
    return [(param, given[param]) for param, _ in declared if param in given]


def write_guard_node(machine, writer, depth, node, stimulus, where):
    """One node of the guard tree: a group, a negation, a comparison or a call."""
    if isinstance(node, dict):
        for key, tag in (('all', 'And'), ('any', 'Or')):
            if key in node:
                parts = node[key]
                if not isinstance(parts, list) or len(parts) < 2:
                    fail('{}: "{}" combines two or more conditions'.format(where, key))
                writer.add(depth, '<{}>'.format(tag))
                for part in parts:
                    write_guard_node(machine, writer, depth + 1, part, stimulus, where)
                writer.add(depth, '</{}>'.format(tag))
                return
        if 'not' in node:
            writer.add(depth, '<Not>')
            write_guard_node(machine, writer, depth + 1, node['not'], stimulus, where)
            writer.add(depth, '</Not>')
            return
        if 'call' in node:
            name = node['call']
            if name not in machine.condition_names:
                fail('{} asks "{}", which is not in "conditions"'.format(where, name))
            pairs = guard_call_arguments(machine, name, node.get('args'), where)
            head = '<Call id="{}" name="{}"'.format(
                writer.of(('condition', name)), esc(name))
            if not pairs:
                writer.add(depth, head + '/>')
                return
            writer.add(depth, head + '>')
            for param, value in pairs:
                writer.add(depth + 1, '<Arg id="{}">'.format(
                    writer.of(('param', 'Condition', name, param))))
                write_guard_node(machine, writer, depth + 2, value, stimulus, where)
                writer.add(depth + 1, '</Arg>')
            writer.add(depth, '</Call>')
            return
        fail('{}: a guard object is "all", "any", "not" or "call"; got {}'
             .format(where, ', '.join(sorted(node))))

    if isinstance(node, list):
        if len(node) != 3:
            fail('{}: a comparison is [left, operator, right]; got {!r}'.format(where, node))
        left, operator, right = node
        if operator not in GUARD_OPS:
            fail('{}: an operator is one of {}; got "{}"'
                 .format(where, ', '.join(GUARD_OPS), operator))
        writer.add(depth, '<Cmp op="{}">'.format(operator))
        write_guard_operand(machine, writer, depth + 1, left, stimulus, where)
        write_guard_operand(machine, writer, depth + 1, right, stimulus, where)
        writer.add(depth, '</Cmp>')
        return
    write_guard_operand(machine, writer, depth, node, stimulus, where)


def write_guard_operand(machine, writer, depth, value, stimulus, where):
    """A leaf of the guard tree. A guard names a declaration by ID, never by name."""
    text = spell(value)
    prefix, _, rest = text.partition(':')
    if prefix in ('raw', 'expr', 'lambda') and rest:
        writer.add(depth, '<{0}><![CDATA[{1}]]></{0}>'
                   .format('Lambda' if prefix == 'lambda' else 'Raw', rest))
        return
    kind, name = source_of(machine, value, stimulus, where)
    if kind == 'Attribute':
        writer.add(depth, '<Attr id="{}" name="{}"/>'
                   .format(writer.of(('attr', name)), esc(name)))
    elif kind == 'Constant':
        writer.add(depth, '<Const id="{}" name="{}"/>'
                   .format(writer.of(('const', name)), esc(name)))
    elif kind == 'Param':
        writer.add(depth, '<Param id="{}" name="{}"/>'
                   .format(writer.of(('param', stimulus[0], stimulus[1], name)), esc(name)))
    else:
        writer.add(depth, '<Lit>{}</Lit>'.format(esc_text(name)))


def write_transition(machine, writer, depth, transition, owner):
    stimulus_name = transition.get('on')
    if not stimulus_name:
        fail('every transition of "{}" needs an "on"'.format(owner))
    kind_of = machine.stimulus_kind(stimulus_name)
    stimulus = (kind_of, stimulus_name) if kind_of != 'Timer' else None
    where = 'the transition of "{}" on "{}"'.format(owner, stimulus_name)

    target = transition.get('to')
    if target is not None and target not in machine.state_names:
        fail('{} goes to "{}", which is not a state of {}'
             .format(where, target, machine.where))
    head = '<Transition ID="{}" Kind="{}" StimulusKind="{}" Stimulus="{}"'.format(
        writer.ident(), 'External' if target else 'Internal', kind_of, esc(stimulus_name))
    if target:
        head += ' To="{}"'.format(writer.of(('state', target)))

    guard = transition.get('guard')
    steps = transition.get('do')
    assignments = transition.get('set')
    if guard is None and not (steps or assignments or transition.get('description')):
        writer.add(depth, head + '/>')
        return

    # cTransition is a sequence: Description, then Guard, then OperationList.
    writer.add(depth, head + '>')
    described(writer, depth + 1, transition)
    if guard is not None:
        writer.add(depth + 1, '<Guard state="ok">')
        writer.add(depth + 2, '<Expr>')
        write_guard_node(machine, writer, depth + 3, guard, stimulus, where)
        writer.add(depth + 2, '</Expr>')
        writer.add(depth + 1, '</Guard>')
    write_operation_list(machine, writer, depth + 1, 'OperationList', steps, assignments,
                         stimulus, where)
    writer.add(depth, '</Transition>')


def write_state(machine, writer, depth, state):
    kind = STATE_KINDS.get(str(state.get('kind', 'normal')).lower())
    if kind is None:
        fail('state "{}" is a "{}"; a kind is one of {}'
             .format(state['name'], state.get('kind'), ', '.join(sorted(STATE_KINDS))))
    head = '<State ID="{}" Name="{}" Kind="{}"'.format(
        writer.of(('state', state['name'])), esc(state['name']), kind)
    if kind == 'History':
        depth_of = str(state.get('depth', 'Shallow')).capitalize()
        if depth_of not in ('Shallow', 'Deep'):
            fail('state "{}" remembers "{}"; a history depth is Shallow or Deep'
                 .format(state['name'], state.get('depth')))
        head += ' HistoryDepth="{}"'.format(depth_of)
    if state.get('submachine'):
        if state['submachine'] not in machine.submachine_names:
            fail('state "{}" hosts "{}", which is not in "submachines"'
                 .format(state['name'], state['submachine']))
        head += ' Submachine="{}"'.format(esc(state['submachine']))
    if state.get('final_event'):
        if state['final_event'] not in machine.event_names:
            fail('state "{}" ends on "{}", which is not in "events"'
                 .format(state['name'], state['final_event']))
        head += ' OnFinal="{}"'.format(esc(state['final_event']))

    transitions = state.get('transitions') or []
    children = state.get('states') or []
    if not (transitions or children or state.get('entry') or state.get('exit')
            or state.get('description')):
        writer.add(depth, head + '/>')
        return

    writer.add(depth, head + '>')
    described(writer, depth + 1, state)
    where = 'state "{}"'.format(state['name'])
    write_operation_list(machine, writer, depth + 1, 'EntryList', state.get('entry'),
                         None, None, 'the entry of ' + where)
    write_operation_list(machine, writer, depth + 1, 'ExitList', state.get('exit'),
                         None, None, 'the exit of ' + where)
    if transitions:
        writer.add(depth + 1, '<TransitionList>')
        for transition in transitions:
            write_transition(machine, writer, depth + 2, transition, state['name'])
        writer.add(depth + 1, '</TransitionList>')
    if children:
        write_state_list(machine, writer, depth + 1, children, state.get('initial'),
                         '{}Start'.format(state['name']), state['name'])
    writer.add(depth, '</State>')


def write_state_list(machine, writer, depth, states, initial, start_name, owner):
    """A StateList, led by the Start pseudo-state that names the first real one."""
    if not initial:
        fail('"{}" has states but no "initial" naming the one it starts in'.format(owner))
    if initial not in machine.state_names:
        fail('the "initial" of "{}" names "{}", which is not a state'.format(owner, initial))
    writer.add(depth, '<StateList>')
    writer.add(depth + 1, '<State ID="{}" Name="{}" Kind="Start">'
               .format(writer.ident(), esc(start_name)))
    writer.add(depth + 2, '<TransitionList>')
    writer.add(depth + 3, '<Transition ID="{}" Kind="Initial" To="{}"/>'
               .format(writer.ident(), writer.of(('state', initial))))
    writer.add(depth + 2, '</TransitionList>')
    writer.add(depth + 1, '</State>')
    for state in states:
        write_state(machine, writer, depth + 1, state)
    writer.add(depth, '</StateList>')


def build_fsml(spec, shared_space, shared_names, prefix=''):
    machine = Machine(spec, shared_space, shared_names, prefix)
    writer = Writer()
    vocab = machine.vocab

    # Everything a number can point at gets that number before anything points at it.
    reserve_all(machine, writer, machine.states)
    for entry in machine.attributes:
        writer.reserve(('attr', entry['name']))
    for entry in machine.constants:
        writer.reserve(('const', entry['name']))
    for entry in machine.conditions:
        writer.reserve(('condition', entry['name']))
    for kind, entries in (('Trigger', machine.triggers), ('Event', machine.events),
                          ('Action', machine.actions), ('Condition', machine.conditions)):
        reserve_params(writer, kind, entries)

    writer.add(0, '<?xml version="1.0" encoding="UTF-8" standalone="yes"?>')
    writer.add(0, '<StateMachine FormatVersion="{}">'.format(FSML_VERSION))
    threading = spec.get('threading', 'Local')
    if threading not in ('Local', 'Shared'):
        fail('{} runs "{}"; threading is Local or Shared'.format(machine.where, threading))
    write_overview(writer, 1, spec, ' Threading="{}"'.format(threading))
    write_datatypes(writer, 1, machine.types, vocab)

    if machine.attributes:
        writer.add(1, '<AttributeList>')
        for entry in machine.attributes:
            atype = vocab.check(entry.get('type'),
                                'attribute "{}" of {}'.format(entry['name'], machine.where))
            head = '<Attribute ID="{}" Name="{}" DataType="{}" Value="{}"'.format(
                writer.of(('attr', entry['name'])), esc(entry['name']), esc(atype),
                esc(entry.get('value', '')))
            if entry.get('description'):
                writer.add(2, head + '>')
                described(writer, 3, entry)
                writer.add(2, '</Attribute>')
            else:
                writer.add(2, head + '/>')
        writer.add(1, '</AttributeList>')

    if machine.events:
        writer.add(1, '<EventList>')
        for entry in machine.events:
            params = named_list(entry, 'params', entry['name'])
            head = '<Event ID="{}" Name="{}"'.format(writer.ident(), esc(entry['name']))
            if not params and not entry.get('description'):
                writer.add(2, head + '/>')
                continue
            writer.add(2, head + '>')
            described(writer, 3, entry)
            write_params(writer, 3, entry['name'], 'Event', params, vocab)
            writer.add(2, '</Event>')
        writer.add(1, '</EventList>')

    if machine.timers:
        writer.add(1, '<TimerList>')
        for entry in machine.timers:
            timeout = entry.get('timeout', 300)
            if not str(timeout).isdigit() or int(timeout) < 1:
                fail('timer "{}" of {} waits "{}"; a timeout is milliseconds, at least 1'
                     .format(entry['name'], machine.where, timeout))
            head = '<Timer ID="{}" Name="{}" Timeout="{}" Repeat="{}"'.format(
                writer.ident(), esc(entry['name']), esc(timeout), esc(entry.get('repeat', 1)))
            if entry.get('description'):
                writer.add(2, head + '>')
                described(writer, 3, entry)
                writer.add(2, '</Timer>')
            else:
                writer.add(2, head + '/>')
        writer.add(1, '</TimerList>')

    if machine.triggers or machine.actions or machine.conditions:
        writer.add(1, '<MethodList>')
        for entry in machine.triggers:
            write_method(writer, 2, entry, 'Trigger', vocab)
        for entry in machine.actions:
            write_method(writer, 2, entry, 'Action', vocab)
        for entry in machine.conditions:
            implement = entry.get('implement', 'Handler')
            if implement not in ('Handler', 'Embedded'):
                fail('condition "{}" of {} is "{}"; it is a Handler the code implements or '
                     'an Embedded body written here'
                     .format(entry['name'], machine.where, implement))
            body = entry.get('body')
            if implement == 'Embedded' and not body:
                fail('condition "{}" of {} is Embedded and carries no "body"'
                     .format(entry['name'], machine.where))
            if implement == 'Handler' and body:
                fail('condition "{}" of {} carries a "body" but is a Handler; make it '
                     'Embedded or drop the body'.format(entry['name'], machine.where))
            extra = ' Return="{}" Implement="{}"'.format(
                esc(entry.get('return', 'bool')), implement)
            write_method(writer, 2, entry, 'Condition', vocab, extra, body,
                         writer.of(('condition', entry['name'])))
        writer.add(1, '</MethodList>')

    write_constants(writer, 1, machine.constants, vocab)
    hosted = []
    for entry in machine.submachines:
        if not entry.get('path'):
            fail('submachine "{}" of {} has no "path" naming its .fsml document'
                 .format(entry['name'], machine.where))
        hosted.append({'name': entry['path'], 'alias': entry['name'],
                       'version': entry.get('version', '1.0.0'),
                       'description': entry.get('description')})
    includes = shared_includes(spec, vocab, hosted)
    write_includes(writer, 1, includes, allow_alias=True)

    write_state_list(machine, writer, 1, machine.states, spec.get('initial'), 'Start',
                     spec['name'])
    writer.add(0, '</StateMachine>')
    return writer.text()


# ---------------------------------------------------------------------------- project

def refuse_key(key, allowed, where):
    """Stop on a key the generator does not read, naming the keys it does."""
    if key.startswith(('#', '/', '_')) or key.lower() in ('comment', 'comments', 'note',
                                                          'notes'):
        fail('{} carries "{}". A note in a spec is a "{}" key; any other key is read as '
             'a field, and "{}" is not one.'.format(where, key, NOTE, key))
    near = difflib.get_close_matches(key, allowed, n=1)
    fail('{} carries "{}", which is not one of its keys. {}Its keys: {}'
         .format(where, key, 'Did you mean "{}"? '.format(near[0]) if near else '',
                 ', '.join(allowed)))


def check_keys(node, allowed, where):
    if isinstance(node, dict):
        for key in node:
            if key not in allowed:
                refuse_key(key, allowed, where)


def listed(owner, key):
    """The object entries of one list; a bare name carries no key to check."""
    entries = owner.get(key) if isinstance(owner, dict) else None
    return [entry for entry in entries if isinstance(entry, dict)] \
        if isinstance(entries, list) else []


def check_list(owner, key, kind, where):
    """Check every object entry of one list against its kind, and return them."""
    entries = listed(owner, key)
    for entry in entries:
        here = '{} "{}" of {}'.format(SINGULAR.get(key, key), entry.get('name', '?'), where)
        check_keys(entry, KEYS[kind], here)
        for inner in ('params', 'answer'):
            check_list(entry, inner, 'parameter', here)
    return entries


def check_types(owner, key, where):
    for entry in listed(owner, key):
        here = 'type "{}" of {}'.format(entry.get('name', '?'), where)
        kind = TYPE_KINDS.get(str(entry.get('kind', '')).lower())
        if kind:
            check_keys(entry, TYPE_KEYS[kind], here)
        check_list(entry, 'values', 'enumerator', here)
        check_list(entry, 'fields', 'field', here)


def check_steps(steps, where):
    for step in steps if isinstance(steps, list) else []:
        verb = next((v for v in STEP_KEYS if v in step), None) \
            if isinstance(step, dict) else None
        if verb:
            check_keys(step, STEP_KEYS[verb], 'a "{}" step of {}'.format(verb, where))


def check_guard(node, where):
    if isinstance(node, list):
        for part in node:
            check_guard(part, where)
        return
    verb = next((v for v in GUARD_KEYS if v in node), None) if isinstance(node, dict) else None
    if verb is None:
        return
    check_keys(node, GUARD_KEYS[verb], 'a guard "{}" of {}'.format(verb, where))
    if verb != 'call':
        check_guard(node[verb], where)
        return
    args = node.get('args')
    for value in args.values() if isinstance(args, dict) else \
            args if isinstance(args, list) else []:
        check_guard(value, where)


def check_states(states, where):
    for state in states if isinstance(states, list) else []:
        if not isinstance(state, dict):
            continue
        name = state.get('name', '?')
        here = 'state "{}" of {}'.format(name, where)
        check_keys(state, KEYS['state'], here)
        check_steps(state.get('entry'), 'the entry of ' + here)
        check_steps(state.get('exit'), 'the exit of ' + here)
        for transition in listed(state, 'transitions'):
            there = 'the transition of "{}" on "{}"'.format(name, transition.get('on', '?'))
            check_keys(transition, KEYS['transition'], there)
            check_guard(transition.get('guard'), there)
            check_steps(transition.get('do'), there)
        check_states(state.get('states'), where)


def check_shape(project):
    """Refuse every key the generator would not read, before any document is written."""
    shared = project.get('datatypes')
    if isinstance(shared, dict):
        where = 'the data type document "{}"'.format(shared.get('name', '?'))
        check_keys(shared, KEYS['datatypes'], where)
        check_types(shared, 'declare', where)
        check_list(shared, 'includes', 'include', where)
    for spec in project['interfaces']:
        if not isinstance(spec, dict):
            continue
        where = 'the service interface "{}"'.format(spec.get('name', '?'))
        check_keys(spec, KEYS['interface'], where)
        check_types(spec, 'types', where)
        # driver is one object, not a list, so check_list does not reach it. A key
        # misspelled here would otherwise fall back to the default in silence.
        if isinstance(spec.get('driver'), dict):
            check_keys(spec['driver'], KEYS['driver'],
                       'the driver of "{}"'.format(spec.get('name', '?')))
        for key, kind in (('attributes', 'service attribute'), ('requests', 'request'),
                          ('responses', 'method'), ('broadcasts', 'method'),
                          ('constants', 'constant'), ('includes', 'include'),
                          ('steps', 'step'), ('timers', 'service timer')):
            check_list(spec, key, kind, where)
    for spec in project['machines']:
        if not isinstance(spec, dict):
            continue
        where = 'the state machine "{}"'.format(spec.get('name', '?'))
        check_keys(spec, KEYS['machine'], where)
        check_types(spec, 'types', where)
        for key, kind in (('attributes', 'machine attribute'), ('constants', 'constant'),
                          ('triggers', 'method'), ('timers', 'timer'), ('events', 'method'),
                          ('actions', 'method'), ('conditions', 'condition'),
                          ('submachines', 'submachine'), ('includes', 'include')):
            check_list(spec, key, kind, where)
        check_states(spec.get('states'), where)


def entry_names(owner, key):
    """The names one list of a spec declares, in order. An entry with no name is skipped."""
    entries = owner.get(key) if isinstance(owner, dict) else None
    if isinstance(entries, dict):
        return [name for name in entries if isinstance(name, str) and name != NOTE]
    names = []
    for entry in entries if isinstance(entries, list) else []:
        name = entry.get('name') if isinstance(entry, dict) else entry
        if isinstance(name, str) and name:
            names.append(name)
    return names


def check_names(owner, key, where):
    """No two names of one list are one: each name of a document gets one ID."""
    kind = SINGULAR.get(key, key)
    seen = set()
    for name in entry_names(owner, key):
        if name in seen:
            fail('two {}s of {} are named "{}". A name is unique among the {}s of one '
                 'document'.format(kind, where, name, kind))
        seen.add(name)


def check_type_names(owner, key, where):
    """The names of the types one document declares, their enumerators and fields."""
    check_names(owner, key, where)
    for entry in listed(owner, key):
        here = 'type "{}" of {}'.format(entry.get('name', '?'), where)
        check_names(entry, 'values', here)
        check_names(entry, 'fields', here)


def check_param_names(owner, key, where):
    """The parameter names of every method in one list of a document."""
    for entry in listed(owner, key):
        here = '{} "{}" of {}'.format(SINGULAR.get(key, key), entry.get('name', '?'), where)
        for inner in ('params', 'answer'):
            check_names(entry, inner, here)


def check_state_names(states, where):
    for state in states if isinstance(states, list) else []:
        if isinstance(state, dict):
            check_names({'states': [state]}, 'states', where)
            check_state_names(state.get('states'), where)


def check_unique_names(project):
    """No list of the design names two entries alike, before any document is built.
    Whether a name compiles is codegen.jar's to say; check_codegen asks it."""
    shared = project.get('datatypes')
    if isinstance(shared, dict):
        check_names({'datatypes': [shared]}, 'datatypes', 'the project')
        check_type_names(shared, 'declare', 'the data type document "{}"'
                         .format(shared.get('name', '?')))
    check_names(project, 'interfaces', 'the project')
    for spec in listed(project, 'interfaces'):
        where = 'the service interface "{}"'.format(spec.get('name', '?'))
        check_type_names(spec, 'types', where)
        check_names(spec, 'attributes', where)
        for key in ('requests', 'responses', 'broadcasts'):
            check_names(spec, key, where)
            check_param_names(spec, key, where)
        check_names(spec, 'constants', where)
    check_names(project, 'machines', 'the project')
    for spec in listed(project, 'machines'):
        where = 'the state machine "{}"'.format(spec.get('name', '?'))
        check_type_names(spec, 'types', where)
        check_names(spec, 'attributes', where)
        for key in ('constants', 'timers', 'triggers', 'conditions', 'events', 'actions',
                    'submachines'):
            check_names(spec, key, where)
        for key in ('triggers', 'events', 'actions', 'conditions'):
            check_param_names(spec, key, where)
        check_state_names(spec.get('states'), where)


def check_codegen(documents, prefix):
    """codegen.jar generates every document built from this design, before any is
    written. What it refuses is reported in its own words: a name C++ cannot carry, a
    keyword, two attributes with one accessor."""
    import hashlib
    import tempfile
    digest = hashlib.sha1('\n'.join(name + '\n' + text for name, text in documents)
                          .encode('utf-8')).hexdigest()
    holder = os.path.join(tempfile.gettempdir(), 'areg-gen-docs', digest)
    paths = []
    for name, text in documents:
        path = os.path.abspath(os.path.join(holder, prefix, name))
        os.makedirs(os.path.dirname(path), exist_ok=True)
        with open(path, 'w', encoding='utf-8', newline='\n') as handle:
            handle.write(text)
        paths.append(path)
    try:
        report = codegen_names.refusal(paths, holder)
    except codegen_names.CodegenError as error:
        fail(str(error))
    if report:
        fail('codegen.jar refuses the documents this design describes. Nothing was '
             'written to --outdir: the documents were generated into a scratch '
             'directory to check them, so a "files written" line below reports that '
             'run and not your tree. Each finding names the element, the list it is '
             'in and the rule it breaks. Correct what the finding points at -- a name, '
             'a type, or the structure behind a type -- and generate again; '
             'python3 {}/explain_rule.py <number> reads the rule out in full, with '
             'its fix:\n'
             .format(os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
                     .replace(chr(92), '/'))
             + report.replace(os.path.abspath(holder) + os.sep, ''))


def without_notes(node):
    if isinstance(node, dict):
        return {key: without_notes(value) for key, value in node.items() if key != NOTE}
    if isinstance(node, list):
        return [without_notes(value) for value in node]
    return node


def canonical(node):
    return json.dumps(without_notes(node), sort_keys=True)


def samples_of(node, found):
    """The canonical text of every object entry of a list in the template."""
    if isinstance(node, dict):
        for value in node.values():
            samples_of(value, found)
    elif isinstance(node, list):
        for item in node:
            if isinstance(item, dict):
                found.add(canonical(item))
            samples_of(item, found)
    return found


def is_empty(value):
    return value is None or value == '' or value == [] or value == {}


# Keys an empty list means something on. A request with "answer": [] declares a
# response that carries no value; without the key it has none. Every other empty
# list means the same as an absent key.
MEANINGFUL_EMPTY = ('answer',)

# Every key any level of a spec reads. A key outside this set is a misspelling
# wherever it stands, so an empty value does not make it absent: it is kept here so
# the key checks refuse it by name. Dropping it instead leaves the misspelling in the
# file to be refused later, once a value has been written into it.
KNOWN_KEYS = frozenset(
    [NOTE] + [key for names in KEYS.values() for key in names]
    + [key for names in TYPE_KEYS.values() for key in names]
    + [key for names in STEP_KEYS.values() for key in names]
    + [key for names in GUARD_KEYS.values() for key in names])


def settle(node, skipped):
    """The spec as the generator reads it: no notes, no template sample left as the
    template wrote it, and no empty value, which means the same as an absent key."""
    if isinstance(node, dict):
        result = {}
        for key, value in node.items():
            if key == NOTE:
                continue
            # A list the template wrote and nobody filled arrives here holding its
            # sample, and empties when the sample is dropped. One written empty was
            # written by the author, and the two mean opposite things.
            written_empty = isinstance(value, list) and not value
            value = settle(value, skipped)
            if not is_empty(value):
                result[key] = value
            elif written_empty and key in MEANINGFUL_EMPTY:
                result[key] = []
            elif key not in KNOWN_KEYS:
                result[key] = value
        return result
    if isinstance(node, list):
        result = []
        for item in node:
            if isinstance(item, dict) and canonical(item) in SAMPLES:
                skipped.append(item)
                continue
            item = settle(item, skipped)
            if not is_empty(item):
                result.append(item)
        return result
    return node


def load_spec(path):
    """One spec file as the generator reads it, and how many samples it left untouched."""
    try:
        with open(path, encoding='utf-8') as handle:
            raw = json.load(handle)
    except (OSError, ValueError) as problem:
        fail('cannot read {}: {}'.format(path, problem))
    if not isinstance(raw, dict):
        fail('{} is not a spec object'.format(path))
    skipped = []
    spec = settle(raw, skipped)
    if NOTE in raw and not spec:
        fail('{} is still the template: nothing in it is filled. Give each document the '
             'task needs a "name" and its entries, and delete a section it does not need.'
             .format(path))
    return spec, len(skipped)


def render(node, depth=0, lead=0):
    """JSON with every value that fits on its line kept on one line."""
    pad = '  ' * depth
    flat = json.dumps(node, separators=(', ', ': '))
    if not isinstance(node, (dict, list)) or not node \
            or len(pad) + lead + len(flat) <= TEMPLATE_WIDTH:
        return flat
    inner = '  ' * (depth + 1)
    if isinstance(node, list):
        return '[\n{}\n{}]'.format(
            ',\n'.join(inner + render(item, depth + 1) for item in node), pad)
    lines, line = [], ''
    for key, value in node.items():
        head = json.dumps(key) + ': '
        item = head + render(value, depth + 1, len(head))
        if '\n' not in item and line and len(line) + 2 + len(item) <= TEMPLATE_WIDTH:
            line += ', ' + item
            continue
        if line:
            lines.append(line)
        line = inner + item
        if '\n' in item:
            lines.append(line)
            line = ''
    if line:
        lines.append(line)
    return '{{\n{}\n{}}}'.format(',\n'.join(lines), pad)


def write_template(path):
    """Write the template to path, unless a file there already carries work.

    Returns "wrote", or "work" for a file that is not an untouched template, which is
    never replaced.
    """
    if os.path.exists(path):
        try:
            with open(path, encoding='utf-8') as handle:
                raw = json.load(handle)
        except (OSError, ValueError):
            return 'work'
        if not (isinstance(raw, dict) and NOTE in raw and not settle(raw, [])):
            return 'work'
    os.makedirs(os.path.dirname(os.path.abspath(path)), exist_ok=True)
    with open(path, 'w', encoding='utf-8', newline='\n') as handle:
        handle.write(render(TEMPLATE) + '\n')
    return 'wrote'


def merge(specs):
    """One project from every spec file given."""
    project = {'datatypes': None, 'interfaces': [], 'machines': []}
    for path, spec in specs:
        if not isinstance(spec, dict):
            fail('{} is not a spec object'.format(path))
        check_keys(spec, KEYS['spec'], path)
        if spec.get('datatypes'):
            if project['datatypes']:
                fail('{} declares a second "datatypes" document; a project has one'
                     .format(path))
            project['datatypes'] = spec['datatypes']
        project['interfaces'].extend(spec.get('interfaces') or [])
        project['machines'].extend(spec.get('machines') or [])
    if not (project['datatypes'] or project['interfaces'] or project['machines']):
        fail('the spec describes no document')
    return project


def check_final_entry(project):
    """Rule 108, in the vocabulary the design is written in, before it is generated.

    A composite reports its level finished by sending itself the event named by
    "final_event", and that event is queued. Entry operations of the nested "final"
    state therefore run while the machine is still inside the composite. The same
    rule is reported by check_contract.py after the document is written; naming it
    here saves writing the document twice.
    """
    number = rule_number(FINAL_ENTRY_RULE, WARNING_BAND)
    if number is None:
        return
    escape = 'areg-check: ignore ' + number

    def walk(states, composite, machine):
        for state in states or []:
            if not isinstance(state, dict):
                continue
            nested = state.get('states')
            if (composite is not None and composite.get('final_event')
                    and str(state.get('kind', '')).lower() == 'final'
                    and state.get('entry')
                    and escape not in (state.get('description') or '')):
                fail('machine "{}": state "{}" is the final state of "{}" and carries '
                     '"entry". Those run while the machine is still inside "{}": the '
                     '"{}" event has not been dispatched yet. Move them to the '
                     'transition of "{}" that fires on "{}". If they are meant to run '
                     'before the level is left, put "{}" in the description of "{}". '
                     '(rule {})'
                     .format(machine, state.get('name', '?'), composite.get('name', '?'),
                             composite.get('name', '?'), composite['final_event'],
                             composite.get('name', '?'), composite['final_event'],
                             escape, state.get('name', '?'), number))
            walk(nested, state, machine)

    for machine in project.get('machines') or []:
        if isinstance(machine, dict):
            walk(machine.get('states'), None, machine.get('name', '?'))


def paired(project):
    """The (interface, machine) pairs of this project.

    A machine is folded into the provider of one service, so the two are one program
    and carry one type per attribute name. An interface names its machine with
    "machine"; a project holding one of each is that pair without saying so. Two
    services that name no machine share nothing and may both carry "Status".
    """
    machines = dict((entry.get('name'), entry) for entry in project['machines']
                    if isinstance(entry, dict))
    pairs, named = [], False
    for interface in project['interfaces']:
        wanted = interface.get('machine')
        if wanted is None:
            continue
        named = True
        if wanted not in machines:
            fail('the service "{}" names the machine "{}", which this project does not '
                 'declare. Its machines: {}'
                 .format(interface.get('name', '?'), wanted,
                         ', '.join(sorted(n for n in machines if n)) or 'none'))
        pairs.append((interface, machines[wanted]))
    if not named and len(project['interfaces']) == 1 and len(project['machines']) == 1:
        pairs.append((project['interfaces'][0], project['machines'][0]))
    return pairs


def cross_check(project):
    """What only a service and the machine behind it can see: one name, one meaning."""
    for interface, machine in paired(project):
        types = {}
        for entry in (interface, machine):
            for attribute in named_list(entry, 'attributes', entry.get('name', '?')):
                name = attribute.get('name')
                declared = attribute.get('type')
                if name in types and types[name][0] != declared:
                    fail('attribute "{}" is a {} in "{}" and a {} in "{}". A service and '
                         'the machine behind it carry one type per name.'
                         .format(name, types[name][0], types[name][1], declared,
                                 entry['name']))
                types[name] = (declared, entry['name'])


# The driver declares these two steps itself, around the ones a spec lists.
STEP_RESERVED = ('start', 'done')
IDENTIFIER = re.compile(r'^[A-Za-z_]\w*$')


# One tick of a stepped consumer, in seconds. The driver's stall watchdog counts
# these, so a tick count and a second are the same number here.
DRIVER_TICK_SECONDS = 1

DRIVER_DEFAULTS = {'connect_seconds': 10, 'reconnect_seconds': 10, 'stall_ticks': 30}

# Ticks a default stall watchdog outlasts the reconnect deadline and the longest
# timed step by.
STALL_MARGIN_TICKS = 10


def whole(value):
    """True for a whole number that is not a bool."""
    return isinstance(value, int) and not isinstance(value, bool)


def driver_of(spec):
    """The driver settings of one interface, every key present.

    A stall watchdog the spec does not name outlasts the reconnect deadline and every
    timed step, so the default never ends a run that is still making progress.
    """
    settings = dict(DRIVER_DEFAULTS)
    given = spec.get('driver') if isinstance(spec, dict) else None
    if isinstance(given, dict):
        settings.update((key, value) for key, value in given.items() if key != NOTE)
    if not (isinstance(given, dict) and 'stall_ticks' in given):
        reconnect = settings['reconnect_seconds']
        floor = [settings['stall_ticks']]
        if whole(reconnect) and reconnect > 0:
            floor.append(reconnect + STALL_MARGIN_TICKS)
        steps = spec.get('steps') if isinstance(spec, dict) else None
        for step in steps if isinstance(steps, list) else []:
            wait = step.get('wait') if isinstance(step, dict) else None
            if whole(wait) and wait > 0:
                floor.append(-(-wait // (1000 * DRIVER_TICK_SECONDS)) + STALL_MARGIN_TICKS)
        settings['stall_ticks'] = max(floor)
    return settings


def check_drivers(project):
    """A consumer's deadlines are whole seconds, and its watchdog outlives them."""
    for spec in project['interfaces']:
        given = spec.get('driver') if isinstance(spec, dict) else None
        if given is None:
            continue
        where = 'the driver of "{}"'.format(spec.get('name', '?'))
        if not isinstance(given, dict):
            fail('{} is an object of {}'.format(where, ', '.join(sorted(DRIVER_DEFAULTS))))
        settings = driver_of(spec)
        for key in sorted(DRIVER_DEFAULTS):
            value = settings[key]
            if isinstance(value, bool) or not isinstance(value, int) or value < 0:
                fail('{} gives "{}" as {!r}. It is a whole number of {}, and 0 turns it '
                     'off'.format(where, key, value,
                                  'ticks' if key == 'stall_ticks' else 'seconds'))
        stall = settings['stall_ticks'] * DRIVER_TICK_SECONDS
        reconnect = settings['reconnect_seconds']
        if stall and reconnect and stall <= reconnect:
            fail('{} lets the stall watchdog fire before the reconnect deadline '
                 '({} tick(s) against {} second(s)): a provider that goes away would be '
                 'reported by whichever timer wins. Make stall_ticks longer than '
                 'reconnect_seconds'.format(where, settings['stall_ticks'], reconnect))


def check_service_timers(project):
    """A provider's own timers: unique names, a timeout in ms, a count, a start flag."""
    for spec in project['interfaces']:
        timers = spec.get('timers') if isinstance(spec, dict) else None
        if not timers:
            continue
        where = 'the timers of "{}"'.format(spec.get('name', '?'))
        if not isinstance(timers, list):
            fail('{} are a list of timer objects'.format(where))
        seen = set()
        for entry in timers:
            name = entry.get('name') if isinstance(entry, dict) else None
            if not isinstance(name, str) or not IDENTIFIER.match(name):
                fail('{}: a timer is {{"name": "<identifier>", "timeout": <ms>}}; got {!r}'
                     .format(where, entry))
            if name in seen:
                fail('{} declare "{}" twice'.format(where, name))
            seen.add(name)
            here = 'timer "{}" of "{}"'.format(name, spec.get('name', '?'))
            timeout, repeat = entry.get('timeout'), entry.get('repeat', 1)
            if not whole(timeout) or timeout <= 0:
                fail('{} gives timeout {!r}: a whole number of milliseconds above 0'
                     .format(here, timeout))
            if not whole(repeat) or repeat < 0:
                fail('{} gives repeat {!r}: how many times it fires, 0 until stopped'
                     .format(here, repeat))
            if not isinstance(entry.get('start', False), bool):
                fail('{} gives start {!r}: true starts it with the component'
                     .format(here, entry.get('start')))


def check_sequences(project):
    """A consumer's steps name only what their service declares, in a shape one driver runs."""
    enums = enum_values(project)
    for spec in project['interfaces']:
        steps = spec.get('steps') if isinstance(spec, dict) else None
        if not steps:
            continue
        where = 'the steps of "{}"'.format(spec.get('name', '?'))
        if not isinstance(steps, list):
            fail('{} are a list of step objects'.format(where))
        requests = dict((entry.get('name'), entry) for entry in listed(spec, 'requests'))
        awaited = set(entry.get('name') for key in ('responses', 'broadcasts', 'attributes')
                      for entry in listed(spec, key))
        awaited |= set(name for name, entry in requests.items()
                       if 'answer' in entry or entry.get('response'))
        seen = set()
        for step in steps:
            if not isinstance(step, dict):
                fail('{} list {!r}, which is not a step object'.format(where, step))
            name = step.get('name')
            here = 'step "{}" of {}'.format(name, where)
            if not isinstance(name, str) or not IDENTIFIER.match(name):
                fail('{} has no name a C++ identifier can carry'.format(here))
            key = name.replace('_', '').lower()
            if key in STEP_RESERVED:
                fail('{} takes a name the driver declares itself ({}, underscores and '
                     'case aside); name it after what it does'
                     .format(here, ', '.join(key.capitalize() for key in STEP_RESERVED)))
            if key in seen:
                fail('{} is named twice: a step name is unique, underscores and case '
                     'aside'.format(here))
            seen.add(key)
            send, target, wait = step.get('send'), step.get('await'), step.get('wait') or 0
            args = step.get('args') or {}
            if send is not None and send not in requests:
                fail('{} sends "{}", which is not a request of the service. Its requests: '
                     '{}'.format(here, send, ', '.join(sorted(requests)) or 'none'))
            if not isinstance(args, dict):
                fail('{}: args is an object, {{"<parameter>": <C++ value>}}'.format(here))
            params = [entry.get('name') for entry in listed(requests.get(send), 'params')]
            for given in args:
                if given not in params:
                    fail('{} gives "{}", which request "{}" does not take. It takes: {}'
                         .format(here, given, send, ', '.join(params) or 'nothing'))
            for param in params:
                if param not in args:
                    fail('{} gives no value for parameter "{}" of request "{}"'
                         .format(here, param, send))
            # A value outside the set its parameter declares reaches a provider with
            # no case for it, which usually does nothing, and the step then awaits an
            # update nothing sends. The run reads as a stall with no cause.
            for entry in listed(requests.get(send), 'params'):
                allowed = entry.get('values')
                given = args.get(entry.get('name'))
                if isinstance(allowed, list) and allowed and given not in allowed:
                    fail('{} sends {}({}={}), and "{}" takes only {}'
                         .format(here, send, entry.get('name'), json.dumps(given),
                                 entry.get('name'),
                                 ', '.join(json.dumps(one) for one in allowed)))
                # The generator qualifies a field of an enumeration with its type, and
                # a name that type has no field of becomes a C++ error naming the
                # generated call site rather than the step that wrote it.
                fields = enums.get(str(entry.get('type', '')).rsplit('::', 1)[-1])
                if fields and isinstance(given, str) \
                        and not given.startswith(('expr:', 'raw:')) \
                        and given.rsplit('::', 1)[-1] not in fields:
                    fail('{} sends {}({}={}), and "{}" has no such field. It has: {}'
                         .format(here, send, entry.get('name'), json.dumps(given),
                                 entry.get('type'), ', '.join(fields)))
            if isinstance(wait, bool) or not isinstance(wait, int) or wait < 0:
                fail('{}: wait is a number of milliseconds'.format(here))
            if target is not None and wait:
                fail('{} awaits "{}" and also waits {} ms. A step does one of the two: '
                     'split it into two steps'.format(here, target, wait))
            stall = driver_of(spec)['stall_ticks']
            if whole(stall) and stall and wait >= stall * DRIVER_TICK_SECONDS * 1000:
                fail('{} waits {} ms, and the stall watchdog of the driver ends the run after '
                     '{} second(s) with no step begun. Make the driver\'s stall_ticks longer '
                     'than the wait, or leave stall_ticks out and the generator makes it '
                     'longer'.format(here, wait, stall))
            if target is not None and target not in awaited:
                fail('{} awaits "{}", which is no response, broadcast or attribute of the '
                     'service'.format(here, target))
            if send is None and target is None and not wait:
                fail('{} sends nothing, awaits nothing and waits for no time'.format(here))


def attribute_reads(node, found):
    """Every name a rule of the machine reads, collected from one node.

    A guard operand, an argument of an action call or an event send, and the value
    side of a "set" are reads. The key side of a "set" is a write, and a write alone
    is not a use.
    """
    if isinstance(node, str):
        found.add(node[5:] if node.startswith('attr:') else node)
    elif isinstance(node, list):
        for item in node:
            attribute_reads(item, found)
    elif isinstance(node, dict):
        for key, value in node.items():
            if key == 'set' and isinstance(value, dict):
                attribute_reads(list(value.values()), found)
            elif key in ('description', 'name', 'on', 'to', 'send', 'call'):
                continue
            else:
                attribute_reads(value, found)


def shared_request_actions(project, spec):
    """Actions run on two or more triggers that each forward a request with an answer.

    An action has no parameter naming the stimulus that ran it, so its body cannot
    tell which of those requests it is answering. A trigger forwards a request when
    one name holds the other.
    """
    answered = [request.get('name') for entry in project.get('interfaces') or []
                for request in entry.get('requests') or []
                if request.get('name') and request.get('answer')]
    if not answered:
        return []

    def forwards(trigger):
        return any(name == trigger or name in trigger or trigger in name
                   for name in answered)

    runs = {}

    def walk(states):
        for state in states or []:
            for move in state.get('transitions') or []:
                on = move.get('on')
                for step in move.get('do') or []:
                    name = step if isinstance(step, str) else \
                        step.get('call') if isinstance(step, dict) else None
                    if isinstance(name, str) and on:
                        runs.setdefault(name, [])
                        if on not in runs[name]:
                            runs[name].append(on)
            walk(state.get('states'))

    walk(spec.get('states'))
    actions = set(entry.get('name') for entry in spec.get('actions') or []
                  if isinstance(entry, dict))
    return [(name, triggers) for name, triggers in runs.items()
            if name in actions and len([on for on in triggers if forwards(on)]) > 1]


def unread_attributes(spec):
    """Machine attributes nothing in the machine reads.

    An attribute no guard, condition call, action argument or event argument reads
    holds no part in sequencing. It is business data the component computes, and it
    belongs to the component: keeping it in the machine costs a declaration, a
    setter on every transition, and the design thought behind both.
    """
    declared = [entry.get('name') for entry in spec.get('attributes') or []
                if entry.get('name')]
    if not declared:
        return []
    found = set()
    attribute_reads(spec.get('states') or [], found)
    return [name for name in declared if name not in found]


def state_key(name):
    """One state or enum-value name reduced to what a comparison can use."""
    return ''.join(ch for ch in name if ch.isalnum()).upper()


def published_states(states, ancestors=()):
    """Every state a consumer could be told about, with the keys its name may match.

    A final state and a history pseudostate are not phases a peer observes, and a
    composite state is entered only by entering one of its children, so neither is
    reported. A child usually spells its parent's name into its own, so the parent's
    key is offered as a prefix to strip.
    """
    found = []
    for state in states or []:
        name = state.get('name')
        if not name or (state.get('kind') or 'normal') != 'normal':
            continue
        children = state.get('states')
        if children or state.get('submachine'):
            found.extend(published_states(children, ancestors + (name,)))
            continue
        keys = {state_key(name)}
        for parent in ancestors:
            head = state_key(parent)
            if head and keys and state_key(name).startswith(head):
                keys.add(state_key(name)[len(head):])
        found.append((name, set(key for key in keys if key)))
    return found


def enum_values(project):
    """Every enum this project declares, by the name an attribute may spell it with."""
    found = {}
    sources = [project.get('datatypes') or {}]
    sources.extend(project.get('interfaces') or [])
    for owner in sources:
        for entry in named_list(owner, 'declare') + named_list(owner, 'types'):
            if entry.get('kind') != 'enum':
                continue
            names = [value.get('name') for value in named_list(entry, 'values')
                     if value.get('name')]
            found[entry['name']] = names
    return found


SPELT_OUT = 4


def says(keys, offered):
    """True when one of these names is a name the peer is offered.

    Spelt-out names are compared loosely in both directions, so a state HEATING is
    said by a value HeatingWater and a state PAUSED by an attribute IsPaused. A short
    name is compared whole, or it would match by accident inside a longer one.
    """
    for key in keys:
        for name in offered:
            if key == name:
                return True
            if len(key) >= SPELT_OUT and key in name:
                return True
            if len(name) >= SPELT_OUT and name in key:
                return True
    return False


def otherwise_visible(interface):
    """Names the peer sees besides one attribute's values: the other members.

    A design may publish a phase as its own flag or announce it as a broadcast rather
    than as a value of the state attribute. That peer can see the phase, so nothing
    is missing from it.
    """
    offered = set()
    for key in ('attributes', 'broadcasts', 'responses'):
        for entry in named_list(interface, key):
            offered.add(state_key(entry['name']))
    return offered


def step_holds(step, answered):
    """What a step waits for before the next begins: a name, milliseconds, or None.

    A step that sends a request with an answer and names nothing else waits for that
    answer. A step that waits for nothing ends at once, in the same dispatch.
    """
    target, wait = step.get('await'), step.get('wait') or 0
    if target is None and not wait and step.get('send') in answered:
        return step.get('send')
    return target if target is not None else (wait or None)


def late_awaits(project, key='attributes'):
    """Steps that await a message an earlier request may send while another step holds.

    A message arriving while the current step waits for something else is dropped
    there. A step that sends nothing and awaits one therefore misses what an earlier
    request caused when a step between them held for something else. A step that
    awaits the same message receives it, and a step that ends at once holds nothing.
    Returns (interface, step, message, the holding step, what it holds for, and
    whether that is the answer to its own request).
    """
    found = []
    for interface in project.get('interfaces') or []:
        steps = interface.get('steps') if isinstance(interface, dict) else None
        if not isinstance(steps, list):
            continue
        steps = [step for step in steps if isinstance(step, dict)]
        targets = set(entry.get('name') for entry in listed(interface, key))
        answered = set(entry.get('name') for entry in listed(interface, 'requests')
                       if 'answer' in entry or entry.get('response'))
        for index, step in enumerate(steps):
            target = step.get('await')
            if step.get('send') is not None or target not in targets:
                continue
            holder = None
            for earlier in reversed(steps[:index]):
                held = step_holds(earlier, answered)
                if held == target:
                    break
                if held is not None and holder is None:
                    holder = (earlier.get('name'), held, held == earlier.get('send'))
                if earlier.get('send') is not None and holder is not None:
                    found.append((interface.get('name', '?'), step.get('name'), target)
                                 + holder)
                    break
    return found


def holding(step, held, answer):
    """How a note names what a holding step waits for."""
    if isinstance(held, int):
        return '"{}" waits {} ms'.format(step, held)
    return '"{}" waits for {}"{}"'.format(step, 'the answer to ' if answer else '', held)


def state_mirrors(project, spec):
    """Attributes that publish this machine's states, and the states they cannot say.

    An attribute whose enum takes several of a machine's state names is how a peer
    watches that machine. A state missing from it, and named nowhere else on the
    interface, is a phase the peer cannot see -- and nothing else reports that: the
    documents generate, the code compiles, and the consumer waits for an update that
    is never sent.
    """
    enums = enum_values(project)
    states = published_states(spec.get('states'))
    if len(states) < 2:
        return []
    found = []
    for interface in project.get('interfaces') or []:
        elsewhere = otherwise_visible(interface)
        for attribute in named_list(interface, 'attributes'):
            declared = (attribute.get('type') or '').split('::')[-1]
            if declared not in enums:
                continue
            offered = set(state_key(value) for value in enums[declared])
            matched = [name for name, keys in states if says(keys, offered)]
            missing = [name for name, keys in states
                       if not says(keys, offered) and not says(keys, elsewhere)]
            # One state name in common is already evidence that the enum mirrors
            # this machine: a peer reads it to tell the phases apart. Below that
            # nothing tells a mirror from an unrelated enumeration.
            if len(matched) >= 1 and missing:
                found.append((interface.get('name', '?'), attribute['name'],
                              (attribute.get('notify') or 'OnChange'),
                              len(matched), missing))
    return found


def trigger_coverage(spec):
    """Which states answer each trigger, in declaration order.

    A trigger has no effect in a state that declares no transition on it. Codegen
    accepts the machine, the generated call compiles, and nothing reports that it
    did nothing. A composite state answers for every state it contains.
    """
    names = [entry['name'] for entry in named_list(spec, 'triggers', spec.get('name', '?'))]
    if not names:
        return []
    answered = dict((name, []) for name in names)

    def walk(states, entry, enters):
        """entry: the state this level starts in. enters: this level is entered at start."""
        for state in states or []:
            label = state.get('name')
            if not label:
                continue
            starts = enters and label == entry
            mark = label + ('*' if starts else '') \
                         + ('+' if state.get('states') else '')
            seen = set()
            for move in state.get('transitions') or []:
                on = move.get('on')
                if on in answered and on not in seen:
                    seen.add(on)
                    answered[on].append(mark)
            walk(state.get('states'), state.get('initial'), starts)

    walk(spec.get('states'), spec.get('initial'), True)
    return [(name, answered[name]) for name in names]


def check_identities(documents):
    """No two documents of one run write the same file.

    The overwrite preflight sees files that were there before the run. Two documents
    of the run itself collide inside it: the first is written, the second replaces it,
    and both are reported as written. A filesystem that folds case collides on a name
    that differs only in case, so the comparison folds it too.
    """
    seen = {}
    for name, _text in documents:
        key = name.lower()
        if key in seen:
            both = '"{}"'.format(name) if seen[key] == name \
                else '"{}" and "{}", which are one file on Windows and macOS' \
                .format(seen[key], name)
            fail('two documents of this project are written as {}: the second would '
                 'replace the first. Give every service, machine and datatypes block a '
                 'name of its own.'.format(both))
        seen[key] = name


def build_all(project, prefix=''):
    """Every document of the project, as (file name, text)."""
    shared = project.get('datatypes')
    space = shared['name'] if shared else None
    names = set(t['name'] for t in named_list(shared, 'declare', 'datatypes')) if shared else set()

    documents = []
    if shared:
        documents.append(('{}.dtml'.format(shared['name']), build_dtml(shared)))
    for spec in project['interfaces']:
        documents.append(('{}.siml'.format(spec['name']), build_siml(spec, space, names, prefix)))
    for spec in project['machines']:
        documents.append(('{}.fsml'.format(spec['name']), build_fsml(spec, space, names, prefix)))
    return documents


EXAMPLE = {
    "datatypes": {
        "name": "GateTypes",
        "description": "Types the service and the machine behind it both carry.",
        "declare": [
            {"name": "Quality", "kind": "enum", "description": "How far a reading is trusted.",
             "values": [{"name": "Suspect", "value": 0}, {"name": "Good", "value": 1}]},
            {"name": "Reading", "kind": "struct",
             "fields": [{"name": "value", "type": "uint32", "description": "What it measured."},
                        {"name": "quality", "type": "Quality", "default": "Quality::Good"}]}
        ]
    },
    "interfaces": [{
        "name": "GateService",
        "category": "Public",
        "description": "Contract between the gate and the program that operates it.",
        "types": [{"name": "History", "kind": "container", "container": "Array",
                   "of": "GateTypes::Reading",
                   "description": "Readings, newest last. Only this document needs it."}],
        "attributes": [{"name": "Width", "type": "uint32", "notify": "OnChange",
                        "description": "How far the gate stands open, in millimetres."},
                       {"name": "Recent", "type": "History", "notify": "Always",
                        "description": "Every reading since the gate last closed."}],
        "constants": [{"name": "MaxWidth", "type": "uint32", "value": "2000"}],
        "requests": [
            {"name": "open", "description": "Open the gate to a width.",
             "params": [{"name": "width", "type": "uint32",
                         "description": "One of the widths the gate is built for.",
                         "values": [600, 1200, 2000]}],
             "answer": [{"name": "accepted", "type": "bool"},
                        {"name": "reason", "type": "String"}]},
            {"name": "close", "description": "Close the gate.",
             "params": [{"name": "by", "type": "String"}]}
        ],
        "broadcasts": [{"name": "gate_moved",
                        "params": [{"name": "reading", "type": "GateTypes::Reading"}]}],
        "driver": {"connect_seconds": 10, "reconnect_seconds": 10},
        "steps": [{"name": "open_wide", "send": "open", "args": {"width": 1200}},
                  {"name": "hold", "wait": 500},
                  {"name": "close_gate", "send": "close",
                   "args": {"by": "night shift"}, "await": "Width",
                   "description": "A String value is written as it reads: the "
                                  "generator quotes it. Its check calls stay() "
                                  "until Width is 0. \"await\" names the update "
                                  "a step waits for, never a value. While a step "
                                  "waits, an update for anything else is dropped "
                                  "there, so a later step that sends nothing and "
                                  "awaits that one never sees it: await an update "
                                  "in the step whose request causes it. The list "
                                  "is linear, and a check loops back with "
                                  "go_to(Step::Name)."}]
    }],
    "machines": [{
        "name": "Gate",
        "description": "Opens to a width, then closes.",
        "attributes": [{"name": "Width", "type": "uint32", "value": "0"},
                       {"name": "Quality", "type": "GateTypes::Quality",
                        "value": "GateTypes::Quality::Good"},
                       {"name": "LastReading", "type": "GateTypes::Reading",
                        "description": "What the gate last measured."}],
        "constants": [{"name": "MaxWidth", "type": "uint32", "value": "2000"}],
        "timers": [{"name": "StepTimer", "timeout": 300}],
        "events": [{"name": "Refused", "params": [{"name": "asked", "type": "uint32"}]},
                   {"name": "Opened", "description": "The OPENING level reached its Final."}],
        "triggers": [{"name": "open", "params": [{"name": "width", "type": "uint32"}]},
                     {"name": "close"}],
        "actions": [{"name": "on_opening"}, {"name": "on_open"},
                    {"name": "on_refused",
                     "params": [{"name": "asked", "type": "uint32"},
                                {"name": "reading", "type": "GateTypes::Reading"}]}],
        "conditions": [{"name": "has_power", "description": "True while the gate has power."},
                       {"name": "can_reach", "description": "True when the motor reaches it.",
                        "params": [{"name": "width", "type": "uint32"}]}],
        "initial": "CLOSED",
        "states": [
            {"name": "CLOSED",
             "transitions": [
                 {"on": "open", "to": "OPENING",
                  "guard": {"all": [["width", "le", "MaxWidth"],
                                    ["Quality", "eq", "GateTypes::Quality::Good"],
                                    {"call": "has_power"},
                                    {"call": "can_reach", "args": {"width": "width"}}]},
                  "set": {"Width": "param:width"}},
                 {"on": "open", "description": "Too wide, untrusted, or no power.",
                  "set": {"Quality": "GateTypes::Quality::Suspect"},
                  "do": [{"send": "Refused", "args": {"asked": "width"}}]}
             ]},
            {"name": "OPENING",
             "description": "A level of its own: its \"states\" run through their own "
                            "\"initial\", and \"final_event\" is how the level reports "
                            "that it finished.",
             "entry": ["on_opening"],
             "initial": "UNLOCKING",
             "final_event": "Opened",
             "transitions": [{"on": "Opened", "to": "OPEN"}],
             "states": [
                 {"name": "UNLOCKING",
                  "entry": ["start StepTimer"], "exit": ["stop StepTimer"],
                  "transitions": [{"on": "StepTimer", "to": "SWINGING"}]},
                 {"name": "SWINGING",
                  "entry": ["start StepTimer"], "exit": ["stop StepTimer"],
                  "transitions": [{"on": "StepTimer", "to": "SWUNG"}]},
                 {"name": "SWUNG", "kind": "final"}
             ]},
            {"name": "OPEN",
             "entry": ["on_open"],
             "transitions": [{"on": "close", "to": "CLOSED",
                              "guard": ["Width", "gt", "lit:0"]},
                             {"on": "Refused",
                              "do": [{"call": "on_refused",
                                      "args": {"asked": "asked",
                                               "reading": "attr:LastReading"}}]}]}
        ]
    }]
}

TEMPLATE_WIDTH = 92

TEMPLATE = {
    NOTE: ["The design of this project: every document is written from this file. Fill the",
           "values and keep the keys. An entry left exactly as written here is skipped and an",
           "empty value is absent, so delete only a section the task does not need. A list",
           "takes as many entries as the design has: copy its sample for each."],
    "datatypes": {
        NOTE: ["Types two documents share, spelled <name>::<Type> inside them. A type only one",
               "document needs goes in that document's own types list, in the same shape.",
               "kind: enum (values), struct (fields), container (container Array or LinkedList",
               "with of; HashMap, Map or Pair with key and of), imported (header, object)."],
        "name": "", "description": "",
        "declare": [
            {"name": "", "kind": "enum", "description": "",
             "values": [{"name": "", "value": 0, "description": ""}]},
            {"name": "", "kind": "struct", "description": "",
             "fields": [{"name": "", "type": "", "default": "", "description": ""}]},
            {"name": "", "kind": "container", "container": "Array", "of": "", "description": ""}
        ]
    },
    "interfaces": [{
        NOTE: ["A .siml: the contract between provider and consumer. category: Public across",
               "processes, Private inside one, Internet across machines.",
               "type: bool char uint8 int16 uint16 int32 uint32 int64 uint64 float double String",
               "WideString BinaryBuffer DateTime, a type of types, or <datatypes name>::<Type>.",
               "notify: OnChange sends a value only when it differs from the one held; Always",
               "sends every set. A request with answer also declares its response, of the same",
               "name; without answer it has none. A broadcast reaches every subscribed consumer.",
               "A request, response or broadcast name is kept as written after its prefix, so",
               "write it snake_case: insert_coin is request_insert_coin. Attributes are converted.",
               "A parameter name used in several answers and broadcasts has one type in all.",
               "values: the legal values of a parameter, when they are a set and the type does",
               "not already say so. A step that sends one outside it is refused here rather",
               "than reaching a provider that ignores it and a step that then awaits for ever.",
               "Leave it out for a parameter that takes any value of its type."],
        "name": "", "category": "Public", "description": "",
        "types": [],
        "attributes": [{"name": "", "type": "", "notify": "OnChange", "description": ""}],
        "requests": [{"name": "", "description": "",
                      "params": [{"name": "", "type": "", "description": "", "values": []}],
                      "answer": [{"name": "", "type": "", "description": ""}]}],
        "broadcasts": [{"name": "", "description": "",
                        "params": [{"name": "", "type": "", "description": ""}]}],
        "constants": [{"name": "", "type": "", "value": "", "description": ""}],
        "timers": [{
            NOTE: ["Timers the provider owns, for periodic or delayed work it does itself;",
                   "delete it when there is none. timeout: milliseconds. repeat: how many",
                   "times it fires, 0 until stopped. start: true starts it with the component;",
                   "otherwise a body calls start_<name>(). Each gets a timer_<name> section."],
            "name": "", "timeout": 1000, "repeat": 0, "start": False, "description": ""
        }],
        "driver": {
            NOTE: ["What the generated consumer gives up after. These values are the",
                   "defaults and are used as they stand; change a number, never a key.",
                   "connect_seconds: how long to wait for the provider to appear.",
                   "reconnect_seconds: how long to wait for it to come back.",
                   "0 turns either off and waits for ever.",
                   "stall_ticks is left out on purpose: the generator makes it longer than",
                   "reconnect_seconds and than every step's wait, one tick a second. Add it",
                   "only to override that; shorter than either is refused."],
            "connect_seconds": 10, "reconnect_seconds": 10
        },
        "steps": [{
            NOTE: ["For a consumer that drives a scenario and then exits, branching included;",
                   "delete it only for one that stays up and reacts. The generator writes the",
                   "sequencing, and each step that awaits something gets one marker for its check.",
                   "send: a request, with args {parameter: C++ value}. await: a response, a",
                   "broadcast or an attribute; a request with an answer awaits its response",
                   "unless the step names another. wait: milliseconds, instead of await.",
                   "Steps run in order and the run exits 0 after the last. A check calls fail(),",
                   "stay() to wait for the next arrival, or go_to(Step::Name) for a loop."],
            "name": "", "send": "", "args": {}, "await": "", "wait": 0, "description": ""
        }]
    }],
    "machines": [{
        NOTE: ["A .fsml, only when behaviour depends on what happened before; delete this",
               "section otherwise. The provider owns it: a request handler fires a trigger, an",
               "action performs an effect, and every decision is a guard here.",
               "Every name a state uses is declared in a list of this machine. A trigger, a",
               "timer and an event never share a name; a state name is unique across levels.",
               "attributes: what guards compare and set assigns, each with its initial value.",
               "conditions: a question a guard asks the code. actions: an effect the code runs."],
        "name": "", "description": "",
        "types": [],
        "attributes": [{"name": "", "type": "", "value": "", "description": ""}],
        "constants": [{"name": "", "type": "", "value": "", "description": ""}],
        "triggers": [{"name": "", "description": "", "params": [{"name": "", "type": ""}]}],
        "timers": [{"name": "", "timeout": 300, "description": ""}],
        "events": [{"name": "", "description": "", "params": [{"name": "", "type": ""}]}],
        "actions": [{"name": "", "description": "", "params": [{"name": "", "type": ""}]}],
        "conditions": [{"name": "", "description": "", "params": [{"name": "", "type": ""}]}],
        "initial": "",
        "states": [{
            NOTE: ["on: a trigger, a timer or an event. to: a sibling state; empty, the state",
                   "stays. Transitions on one stimulus are tried in order and the first whose",
                   "guard holds is taken: the guarded ones first, the fallback last.",
                   "guard: [left, op, right] with op eq ne lt le gt ge; {call: condition, args:",
                   "{param: value}}; {all: [...]}, {any: [...]}, {not: ...}. An operand is a",
                   "declared name, or param:x attr:X const:X lit:text raw:c++, as is a set value.",
                   "set: {Attribute: value}, applied before do. do, entry and exit list: action,",
                   "start Timer, stop Timer, send Event, or {call: action, args: {param: value}}.",
                   "kind: empty, final or history. A composite has its own initial and states;",
                   "its final substate ends the level and sends final_event, which a transition",
                   "of the composite takes. A history substate (depth Shallow or Deep) is the",
                   "target that resumes the level where it left off."],
            "name": "", "kind": "", "entry": [], "exit": [],
            "transitions": [{"on": "", "to": "", "guard": [], "set": {}, "do": []}],
            "initial": "", "final_event": "",
            "states": [{
                NOTE: ["A substate. The composite's own \"initial\" names one of these, and a",
                       "transition of the composite fires on its \"final_event\"."],
                "name": "", "kind": "", "entry": [], "exit": [],
                "transitions": [{"on": "", "to": "", "guard": [], "set": {}, "do": []}]
            }, {
                NOTE: ["kind history resumes this level where it left off, and is a target,",
                       "never a state the machine sits in. depth Shallow or Deep."],
                "name": "", "kind": "history", "depth": "Shallow"
            }, {
                NOTE: ["kind final ends the level and sends the final_event above. It carries",
                       "no entry: that event is queued, so entry operations here would run",
                       "while the machine is still inside the composite."],
                "name": "", "kind": "final"
            }]
        }]
    }]
}

SAMPLES = samples_of(TEMPLATE, set())


# A document rewritten with the same text still gets a new modification time, and a
# build system and run_scenarios.py both read that time as an edit. build_project.py
# --run regenerates every document on every call, so an unconditional write made the
# documents newer than binaries the build had nothing to relink: the scenarios step
# of that same command then refused its own build as stale, and every later call
# widened the gap. The comparison is of text, so a checkout with either line ending
# answers the same.
def unchanged(path, text):
    """Whether this document already holds exactly this text."""
    try:
        with open(path, encoding='utf-8') as handle:
            return handle.read() == text
    except (OSError, UnicodeDecodeError):
        return False


def review(project, skipped):
    """Prints every note a design earns. Writes nothing and reads nothing back."""
    if skipped:
        print('  note  {} sample entr{} of the template, left as written, skipped.'
              .format(skipped, 'y' if skipped == 1 else 'ies'))
    # One note per finding and one explanation for all of them: the same paragraph
    # under every name is re-sent with every later request of the conversation.
    late = {}
    for owner, step, attribute, *held in late_awaits(project):
        late.setdefault(owner, []).append('"{}" awaits "{}" while {}'
                                          .format(step, attribute, holding(*held)))
    for owner in sorted(late):
        print('  note  {}: {}.'.format(owner, '; '.join(late[owner])))
        print('        An update arriving while another step waits is dropped there, and '
              'the awaiting step sees the next one. That is correct when later updates '
              'follow and its check stay()s until the value it wants. If the dropped '
              'update can be the last one, the step that sends the request awaits the '
              'attribute instead.')
    # The same shape on a broadcast, which is worse: an attribute sends another update
    # the next time it is set, a broadcast never comes again. Whether it is a fault
    # depends on something no design states -- where the provider sends it -- so this
    # names the shape and the one question that settles it, once, for every row.
    once = {}
    for owner, step, message, *held in late_awaits(project, 'broadcasts'):
        once.setdefault(owner, []).append('"{}" awaits broadcast "{}" while {}'
                                          .format(step, message, holding(*held)))
    for owner in sorted(once):
        print('  note  {}: {}.'.format(owner, '; '.join(once[owner])))
        print('        A broadcast is delivered once. Ask of each: does the provider '
              'send it while handling that earlier request? Then it arrives before '
              'this step begins, is dropped, and never comes again -- await it on the '
              'step that sends the request instead. Does something timed separate '
              'them, a timer or a state the machine sits in? Then the shape is '
              'correct. A run that stalls here names the dropped message and the step '
              'it arrived on.')
    unread = {}
    for spec in project['machines']:
        for name in unread_attributes(spec):
            unread.setdefault(spec.get('name', '?'), []).append(name)
    for owner in sorted(unread):
        print('  note  {}: attribute(s) {} written and never read by a guard, a '
              'condition or an argument.'
              .format(owner, ', '.join('"%s"' % n for n in unread[owner])))
        print('        Data no rule of the machine reads belongs to the component '
              'that computes it, not to the machine.')
    for spec in project['machines']:
        shared = shared_request_actions(project, spec)
        if shared:
            print('  note  {}: {}.'.format(spec.get('name', '?'), '; '.join(
                'action "{}" runs on {}'.format(name, ', '.join(triggers))
                for name, triggers in shared)))
            print('        Each of these triggers forwards a request with its own '
                  'response, and an action cannot tell which trigger ran it. Declare '
                  'one action per trigger, or pass what differs as an argument.')
        for owner, attribute, notify, matched, missing in state_mirrors(project, spec):
            print('  note  {}: attribute "{}" of {} takes {} of this machine\'s state '
                  'names and has no value for: {}. A consumer cannot see the machine '
                  'enter a state the attribute cannot express. Give it a value per '
                  'state a peer must tell apart, or accept that the state is invisible.'
                  .format(spec.get('name', '?'), attribute, owner, matched,
                          ', '.join(missing)))
            if matched == 1:
                print('          One name in common can be coincidence: if this '
                      'enumeration does not mirror the machine, nothing is wrong here.')
            if notify != 'Always':
                print('          Notify="{}" makes it worse than invisible: leaving such '
                      'a state and re-entering the one it came from re-sets the value '
                      'already held, which notifies nobody, so a consumer waiting for '
                      'that update waits for ever.'.format(notify))
        coverage = trigger_coverage(spec)
        if coverage:
            print('  note  {}: which states answer each trigger (* the initial state, '
                  '+ also while inside it). A trigger called in a state not listed '
                  'beside it does nothing, and nothing reports it.'
                  .format(spec.get('name', '?')))
            width = max(len(name) for name, _ in coverage)
            for name, states in coverage:
                print('          {}  {}'.format(name.ljust(width),
                                                ', '.join(states) or 'NO STATE'))
    # The documents are legitimate, so this is a note and not a refusal. It is said
    # in the words build_project.py refuses with, several steps later, and it is said
    # on both paths: build_project.py passes --chained, so a note only the bare call
    # prints is a note the documented path never shows.
    for what, found, option in (('service', project.get('interfaces') or [], '--doc'),
                                ('state machine', project.get('machines') or [],
                                 '--machine')):
        if len(found) > 1:
            print('  note  this design describes {} {}s. build_project.py writes one '
                  'application, of one service and at most one machine: name the one '
                  'to build with {}, and write the others with gen_skeleton.py --app '
                  'into their own directories.'.format(len(found), what, option))

def main():
    parser = argparse.ArgumentParser(description=__doc__.splitlines()[0])
    parser.add_argument('--spec', action='append', default=[],
                        help='a JSON description; pass it once per spec file')
    parser.add_argument('--outdir', help='the directory the documents are written to')
    parser.add_argument('--example', action='store_true',
                        help='print a whole spec to copy, and write nothing')
    parser.add_argument('--template', metavar='PATH',
                        help='write a spec with every key present and empty, each section '
                             'with its note, to fill in. A file there that carries work is '
                             'never replaced')
    parser.add_argument('--review', action='store_true',
                        help='check the design and print every note it earns, writing '
                             'nothing. Answers what a build would say about the design '
                             'before any document, source or scenario file exists')
    parser.add_argument('--force', action='store_true', help='overwrite existing documents')
    parser.add_argument('--chained', action='store_true',
                        help='run by build_project.py, which does the next steps itself: '
                             'do not print them')
    args = parser.parse_args()

    if args.example:
        # The same renderer the template uses: every value that fits stays on its
        # line. The page tells the agent to read this in one call, and one value per
        # line makes that call twice the size for nothing.
        print(render(EXAMPLE))
        return 0
    if args.template:
        if write_template(args.template) == 'work':
            fail('{} already carries a design, and a template never replaces one. Fill '
                 'that file, or name another path.'.format(args.template))
        print('wrote {}: every key of a design, empty. Fill the values and keep the keys; '
              'an entry left as written is skipped.'.format(args.template))
        return 0
    if not args.spec or not (args.outdir or args.review):
        parser.error('--spec and --outdir are both required, or --example, '
                     'or --template, or --review')
    # An include names a document the way the project root spells it, so a review of a
    # design not yet written reads it the way build_project.py will write it.
    if not args.outdir:
        args.outdir = os.path.join('src', 'services')

    specs = []
    skipped = 0
    for path in args.spec:
        spec, count = load_spec(path)
        specs.append((path, spec))
        skipped += count

    project = merge(specs)
    check_shape(project)
    check_unique_names(project)
    cross_check(project)
    check_sequences(project)
    check_drivers(project)
    check_service_timers(project)
    check_final_entry(project)
    # An include names a document the way the project root spells it, which is the
    # directory the documents are written to.
    prefix = '' if os.path.isabs(args.outdir) else \
        args.outdir.replace(os.sep, '/').rstrip('/') + '/'
    documents = build_all(project, prefix)
    check_identities(documents)

    if args.review:
        # Every note the build prints, at the point the design is still one file to
        # edit: after the build each of them costs a regeneration. The notes come
        # before the refusal so that one review answers everything the design is
        # asked, and one edit can settle all of it.
        review(project, skipped)
        check_codegen(documents, prefix)
        print('  reviewed {} document(s) of {} spec(s). Nothing was written. Build the '
              'project with build_project.py --spec.'
              .format(len(documents), len(specs)))
        return 0

    check_codegen(documents, prefix)

    try:
        os.makedirs(args.outdir, exist_ok=True)
    except OSError as problem:
        fail('--outdir {} cannot be used as a directory: {}'
             .format(args.outdir, problem.strerror or problem))
    for name, text in documents:
        target = os.path.join(args.outdir, name)
        if os.path.exists(target) and not args.force:
            fail('{} exists; pass --force to overwrite it'.format(target))
    for name, text in documents:
        target = os.path.join(args.outdir, name)
        if unchanged(target, text):
            print('unchanged {}'.format(target))
            continue
        try:
            with open(target, 'w', encoding='utf-8') as handle:
                handle.write(text)
        except OSError as problem:
            fail('{} cannot be written: {}'.format(target,
                                                   problem.strerror or problem))
        print('wrote {}'.format(target))
    review(project, skipped)
    if args.chained:
        print('  {} document(s).'.format(len(documents)))
    else:
        print('  {} document(s). Generate the code with codegen.jar, then the application '
              'with gen_skeleton.py --app.'.format(len(documents)))
    return 0


if __name__ == '__main__':
    sys.exit(main())
