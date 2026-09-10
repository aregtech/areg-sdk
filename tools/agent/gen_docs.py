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
import json
import os
import sys

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))

from docmodel import (CONTAINERS, PREDEFINED, Vocabulary, Writer, described, esc,
                      esc_text, fail, named_list, reserve_params, unique, write_constants,
                      write_datatypes, write_includes, write_method, write_overview,
                      write_params)

DTML_VERSION = '1.0.0'
SIML_VERSION = '1.1.0'
FSML_VERSION = '1.2.0'

CATEGORIES = ('Private', 'Public', 'Internet')
STATE_KINDS = {'normal': 'Normal', 'final': 'Final', 'history': 'History'}
GUARD_OPS = ('eq', 'ne', 'lt', 'le', 'gt', 'ge')
SOURCES = {'param': 'Param', 'attr': 'Attribute', 'const': 'Constant', 'expr': 'Expression'}


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
        for kind, entries in (('Trigger', self.triggers), ('Event', self.events),
                              ('Action', self.actions), ('Condition', self.conditions)):
            for entry in entries:
                self.params_of[(kind, entry['name'])] = \
                    [p['name'] for p in named_list(entry, 'params', entry['name'])]

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


def source_of(machine, value, stimulus, where):
    """How a written value reaches the generated code: a declared name or a literal.

    "param:x", "attr:X", "const:X", "expr:<c++>" and "lit:<text>" say it outright. A
    bare name that was declared is that declaration; anything else is a verbatim C++
    token, so a string literal carries its own quotes.
    """
    text = str(value)
    prefix, _, rest = text.partition(':')
    if prefix == 'lit' and rest:
        return 'Value', rest
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
    return 'Value', text


def write_arguments(machine, writer, depth, owner_kind, owner, args, stimulus, where):
    """The ArgumentList an ActionCall or an EventSend carries."""
    declared = machine.params_of.get((owner_kind, owner), [])
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
        kind, text = source_of(machine, value, stimulus, where)
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
    kind, text = source_of(machine, value, stimulus, where)
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
            args = node.get('args') or []
            head = '<Call id="{}" name="{}"'.format(
                writer.of(('condition', name)), esc(name))
            if not args:
                writer.add(depth, head + '/>')
                return
            writer.add(depth, head + '>')
            for arg in args:
                writer.add(depth + 1, '<Arg>')
                write_guard_node(machine, writer, depth + 2, arg, stimulus, where)
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
    text = str(value)
    prefix, _, rest = text.partition(':')
    if prefix in ('raw', 'lambda') and rest:
        writer.add(depth, '<{0}><![CDATA[{1}]]></{0}>'
                   .format('Raw' if prefix == 'raw' else 'Lambda', rest))
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
    elif kind == 'Expression':
        fail('{}: a guard writes verbatim C++ as "raw:<c++>", not "expr:"'.format(where))
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
    if not (guard or steps or assignments or transition.get('description')):
        writer.add(depth, head + '/>')
        return

    # cTransition is a sequence: Description, then Guard, then OperationList.
    writer.add(depth, head + '>')
    described(writer, depth + 1, transition)
    if guard:
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

def merge(specs):
    """One project from every spec file given."""
    project = {'datatypes': None, 'interfaces': [], 'machines': []}
    for path, spec in specs:
        if not isinstance(spec, dict):
            fail('{} is not a spec object'.format(path))
        unknown = set(spec) - {'datatypes', 'interfaces', 'machines'}
        if unknown:
            fail('{} carries {}; a spec holds "datatypes", "interfaces" and "machines"'
                 .format(path, ', '.join(sorted(unknown))))
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


def cross_check(project):
    """What only the whole project can see: one name, one meaning."""
    types = {}
    for entry in project['machines'] + project['interfaces']:
        for attribute in named_list(entry, 'attributes', entry.get('name', '?')):
            name = attribute.get('name')
            declared = attribute.get('type')
            if name in types and types[name][0] != declared:
                fail('attribute "{}" is a {} in "{}" and a {} in "{}". A service and the '
                     'machine behind it carry one type per name.'
                     .format(name, types[name][0], types[name][1], declared, entry['name']))
            types[name] = (declared, entry['name'])


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
                        {"name": "quality", "type": "Quality", "default": "Quality::Good"}]},
            {"name": "History", "kind": "container", "container": "Array", "of": "Reading"}
        ]
    },
    "interfaces": [{
        "name": "GateService",
        "category": "Public",
        "description": "Contract between the gate and the program that operates it.",
        "attributes": [{"name": "Width", "type": "uint32", "notify": "OnChange",
                        "description": "How far the gate stands open, in millimetres."}],
        "constants": [{"name": "MaxWidth", "type": "uint32", "value": "2000"}],
        "requests": [
            {"name": "open", "description": "Open the gate to a width.",
             "params": [{"name": "width", "type": "uint32"}],
             "answer": [{"name": "accepted", "type": "bool"},
                        {"name": "reason", "type": "String"}]},
            {"name": "close", "description": "Close the gate."}
        ],
        "broadcasts": [{"name": "gate_moved",
                        "params": [{"name": "reading", "type": "GateTypes::Reading"}]}]
    }],
    "machines": [{
        "name": "Gate",
        "description": "Opens to a width, then closes.",
        "attributes": [{"name": "Width", "type": "uint32", "value": "0"}],
        "constants": [{"name": "MaxWidth", "type": "uint32", "value": "2000"}],
        "timers": [{"name": "StepTimer", "timeout": 300}],
        "events": [{"name": "Refused", "params": [{"name": "asked", "type": "uint32"}]}],
        "triggers": [{"name": "open", "params": [{"name": "width", "type": "uint32"}]},
                     {"name": "close"}],
        "actions": [{"name": "on_opening"}, {"name": "on_open"},
                    {"name": "on_refused", "params": [{"name": "asked", "type": "uint32"}]}],
        "conditions": [{"name": "has_power", "description": "True while the gate has power."}],
        "initial": "CLOSED",
        "states": [
            {"name": "CLOSED",
             "transitions": [
                 {"on": "open", "to": "OPENING",
                  "guard": {"all": [["width", "le", "MaxWidth"], {"call": "has_power"}]},
                  "set": {"Width": "param:width"}},
                 {"on": "open", "description": "Too wide, or no power.",
                  "do": [{"send": "Refused", "args": {"asked": "width"}}]}
             ]},
            {"name": "OPENING",
             "entry": ["on_opening", "start StepTimer"],
             "exit": ["stop StepTimer"],
             "transitions": [{"on": "StepTimer", "to": "OPEN"}]},
            {"name": "OPEN",
             "entry": ["on_open"],
             "transitions": [{"on": "close", "to": "CLOSED"},
                             {"on": "Refused", "do": [{"call": "on_refused",
                                                       "args": {"asked": "asked"}}]}]}
        ]
    }]
}


def main():
    parser = argparse.ArgumentParser(description=__doc__.splitlines()[0])
    parser.add_argument('--spec', action='append', default=[],
                        help='a JSON description; pass it once per spec file')
    parser.add_argument('--outdir', help='the directory the documents are written to')
    parser.add_argument('--example', action='store_true',
                        help='print a whole spec to copy, and write nothing')
    parser.add_argument('--force', action='store_true', help='overwrite existing documents')
    args = parser.parse_args()

    if args.example:
        print(json.dumps(EXAMPLE, indent=2))
        return 0
    if not args.spec or not args.outdir:
        parser.error('--spec and --outdir are both required, or --example')

    specs = []
    for path in args.spec:
        try:
            with open(path, encoding='utf-8') as handle:
                specs.append((path, json.load(handle)))
        except (OSError, ValueError) as problem:
            fail('cannot read {}: {}'.format(path, problem))

    project = merge(specs)
    cross_check(project)
    # An include names a document the way the project root spells it, which is the
    # directory the documents are written to.
    prefix = '' if os.path.isabs(args.outdir) else \
        args.outdir.replace(os.sep, '/').rstrip('/') + '/'
    documents = build_all(project, prefix)

    os.makedirs(args.outdir, exist_ok=True)
    for name, text in documents:
        target = os.path.join(args.outdir, name)
        if os.path.exists(target) and not args.force:
            fail('{} exists; pass --force to overwrite it'.format(target))
    for name, text in documents:
        with open(os.path.join(args.outdir, name), 'w', encoding='utf-8') as handle:
            handle.write(text)
        print('wrote {}'.format(os.path.join(args.outdir, name)))
    print('  {} document(s). Generate the code with codegen.jar, then the application '
          'with gen_skeleton.py --app.'.format(len(documents)))
    return 0


if __name__ == '__main__':
    sys.exit(main())
