#!/usr/bin/env python3
"""Write a .fsml state machine document from a compact JSON description.

The .fsml grammar carries numeric IDs and resolves every transition target, guard
operand and nested state by that ID. Assigning those numbers by hand is bookkeeping,
and it is where an author spends effort that has nothing to do with the machine.
This tool takes states, triggers, timers and transitions by name and writes the
document, so a name is the only thing anyone has to get right.

    python3 tools/agent/gen_fsml.py --spec machine.json --out src/services/X.fsml
    python3 tools/agent/gen_fsml.py --example        # a whole spec to copy

The spec, with only "name", "actions", "initial" and "states" required:

    {
      "name": "Gate",
      "description": "one line",
      "attributes": [{"name": "Width", "type": "uint32", "value": "0"}],
      "events":     [{"name": "Opened"}],
      "timers":     [{"name": "StepTimer", "timeout": 300, "repeat": 1}],
      "triggers":   [{"name": "open", "params": [{"name": "width", "type": "uint32"}]}],
      "actions":    [{"name": "on_opening"}],
      "initial": "CLOSED",
      "states": [
        {"name": "CLOSED", "transitions": [{"on": "open", "to": "OPENING"}]},
        {"name": "OPENING",
         "entry": ["on_opening", "start StepTimer"],
         "exit":  ["stop StepTimer"],
         "transitions": [{"on": "StepTimer", "to": "OPEN"}]},
        {"name": "OPEN", "transitions": [{"on": "close", "do": ["on_closing"]}]}
      ]
    }

A transition names its stimulus in "on"; whether that is a trigger, a timer or an
event is read from the lists above, so it is never spelled twice. A transition with
no "to" stays in the state. "do" calls actions, "set" assigns an attribute and
"guard" is [attribute, operator, literal].
"""
import argparse
import json
import os
import sys
import xml.sax.saxutils as saxutils

FORMAT_VERSION = '1.2.0'
STATE_KINDS = {'normal': 'Normal', 'final': 'Final', 'history': 'History'}
GUARD_OPS = ('eq', 'ne', 'lt', 'le', 'gt', 'ge')


def fail(message):
    sys.stderr.write('error: {}\n'.format(message))
    raise SystemExit(2)


def esc(value):
    return saxutils.escape(str(value), {'"': '&quot;'})


class Writer:
    """Collects the document, and hands out the identifiers it refers to."""

    def __init__(self):
        self.lines = []
        self._next = 0

    def ident(self):
        self._next += 1
        return self._next

    def add(self, depth, text):
        self.lines.append('    ' * depth + text)

    def text(self):
        return '\n'.join(self.lines) + '\n'


def described(writer, depth, spec):
    """The Description child an element carries when the spec gives one."""
    if spec.get('description'):
        writer.add(depth, '<Description>{}</Description>'.format(esc(spec['description'])))


def named_list(spec, key):
    """The entries of a spec list, each allowed to be a bare name."""
    result = []
    for entry in spec.get(key) or []:
        result.append({'name': entry} if isinstance(entry, str) else dict(entry))
    for entry in result:
        if not entry.get('name'):
            fail('every entry of "{}" needs a name'.format(key))
    return result


def collect_states(states, seen, order):
    """Every state name in the tree, so a transition target can be checked."""
    for state in states:
        name = state.get('name')
        if not name:
            fail('every state needs a name')
        if name in seen:
            fail('two states are named "{}"; a state name is unique document-wide'.format(name))
        seen.add(name)
        order.append(state)
        collect_states(state.get('states') or [], seen, order)


class Machine:
    """The spec, checked, with an identifier reserved for everything named."""

    def __init__(self, spec):
        self.name = spec.get('name')
        if not self.name:
            fail('the spec has no "name"')
        self.spec = spec
        self.attributes = named_list(spec, 'attributes')
        self.events = named_list(spec, 'events')
        self.timers = named_list(spec, 'timers')
        self.triggers = named_list(spec, 'triggers')
        self.actions = named_list(spec, 'actions')
        if not self.actions:
            fail('a machine with no action generates nothing to implement')

        self.timer_names = set(t['name'] for t in self.timers)
        self.event_names = set(e['name'] for e in self.events)
        self.trigger_names = set(t['name'] for t in self.triggers)
        self.action_names = set(a['name'] for a in self.actions)
        self.attribute_names = set(a['name'] for a in self.attributes)

        clash = (self.timer_names & self.event_names) | (self.timer_names & self.trigger_names) \
            | (self.event_names & self.trigger_names)
        if clash:
            fail('a timer, an event and a trigger cannot share a name: {}'
                 .format(', '.join(sorted(clash))))

        self.states = spec.get('states') or []
        if not self.states:
            fail('the spec has no "states"')
        seen, order = set(), []
        collect_states(self.states, seen, order)
        self.state_names = seen
        self.ids = {}
        self.attribute_ids = {}

    def stimulus_kind(self, name):
        if name in self.trigger_names:
            return 'Trigger'
        if name in self.timer_names:
            return 'Timer'
        if name in self.event_names:
            return 'Event'
        fail('"{}" is named by a transition but is not a trigger, a timer or an event'
             .format(name))


def reserve(machine, writer, states):
    """One identifier per state, before any transition has to name a target."""
    for state in states:
        machine.ids[state['name']] = writer.ident()
        reserve(machine, writer, state.get('states') or [])


def write_operations(machine, writer, depth, transition):
    """The ActionCall and AttributeSet a transition carries."""
    actions = transition.get('do') or []
    assignments = transition.get('set') or {}
    if not actions and not assignments:
        return False
    writer.add(depth, '<OperationList>')
    for action in actions:
        if action not in machine.action_names:
            fail('"{}" is called by a transition but is not in "actions"'.format(action))
        writer.add(depth + 1, '<ActionCall ID="{}" Action="{}"/>'
                   .format(writer.ident(), esc(action)))
    for attribute, value in assignments.items():
        if attribute not in machine.attribute_names:
            fail('"{}" is assigned by a transition but is not in "attributes"'.format(attribute))
        source, _, literal = str(value).partition(':')
        if source in ('param', 'attr', 'const') and literal:
            kind = {'param': 'Param', 'attr': 'Attribute', 'const': 'Constant'}[source]
        else:
            kind, literal = 'Value', str(value)
        writer.add(depth + 1, '<AttributeSet ID="{}" Attribute="{}" Source="{}" Value="{}"/>'
                   .format(writer.ident(), esc(attribute), kind, esc(literal)))
    writer.add(depth, '</OperationList>')
    return True


def write_guard(machine, writer, depth, transition):
    """The Expr a guard compares an attribute against a literal in."""
    guard = transition.get('guard')
    if not guard:
        return
    if len(guard) != 3:
        fail('a guard is [attribute, operator, literal]; got {!r}'.format(guard))
    attribute, operator, literal = guard
    if attribute not in machine.attribute_names:
        fail('a guard reads "{}", which is not in "attributes"'.format(attribute))
    if operator not in GUARD_OPS:
        fail('a guard operator is one of {}; got "{}"'.format(', '.join(GUARD_OPS), operator))
    writer.add(depth, '<Guard state="ok">')
    writer.add(depth + 1, '<Expr>')
    writer.add(depth + 2, '<Cmp op="{}"><Attr id="{}"/><Lit>{}</Lit></Cmp>'
               .format(operator, machine.attribute_ids[attribute], esc(literal)))
    writer.add(depth + 1, '</Expr>')
    writer.add(depth, '</Guard>')


def write_transition(machine, writer, depth, transition):
    stimulus = transition.get('on')
    if not stimulus:
        fail('every transition needs an "on"')
    kind_of = machine.stimulus_kind(stimulus)
    target = transition.get('to')
    if target is not None and target not in machine.state_names:
        fail('a transition goes to "{}", which is not a state'.format(target))
    head = '<Transition ID="{}" Kind="{}" StimulusKind="{}" Stimulus="{}"'.format(
        writer.ident(), 'External' if target else 'Internal', kind_of, esc(stimulus))
    if target:
        head += ' To="{}"'.format(machine.ids[target])
    body = bool(transition.get('do') or transition.get('set') or transition.get('guard'))
    if not body:
        writer.add(depth, head + '/>')
        return
    writer.add(depth, head + '>')
    write_operations(machine, writer, depth + 1, transition)
    write_guard(machine, writer, depth + 1, transition)
    writer.add(depth, '</Transition>')


def write_entry_exit(machine, writer, depth, state, key, tag):
    """An EntryList or an ExitList, from a list of actions and timer verbs."""
    steps = state.get(key) or []
    if not steps:
        return
    writer.add(depth, '<{}>'.format(tag))
    for step in steps:
        verb, _, timer = step.partition(' ')
        if verb in ('start', 'stop') and timer:
            if timer not in machine.timer_names:
                fail('"{}" starts or stops "{}", which is not in "timers"'
                     .format(state['name'], timer))
            writer.add(depth + 1, '<Timer{} ID="{}" Timer="{}"/>'
                       .format('Start' if verb == 'start' else 'Stop',
                               writer.ident(), esc(timer)))
        else:
            if step not in machine.action_names:
                fail('"{}" is run by "{}" but is not in "actions"'.format(step, state['name']))
            writer.add(depth + 1, '<ActionCall ID="{}" Action="{}"/>'
                       .format(writer.ident(), esc(step)))
    writer.add(depth, '</{}>'.format(tag))


def write_state(machine, writer, depth, state):
    kind = STATE_KINDS.get(str(state.get('kind', 'normal')).lower())
    if kind is None:
        fail('a state kind is one of {}; got "{}"'
             .format(', '.join(sorted(STATE_KINDS)), state.get('kind')))
    head = '<State ID="{}" Name="{}" Kind="{}"'.format(
        machine.ids[state['name']], esc(state['name']), kind)
    if kind == 'History':
        head += ' HistoryDepth="{}"'.format(esc(state.get('depth', 'Shallow')))
    if state.get('final_event'):
        if state['final_event'] not in machine.event_names:
            fail('"{}" ends on "{}", which is not in "events"'
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
    write_entry_exit(machine, writer, depth + 1, state, 'entry', 'EntryList')
    write_entry_exit(machine, writer, depth + 1, state, 'exit', 'ExitList')
    if transitions:
        writer.add(depth + 1, '<TransitionList>')
        for transition in transitions:
            write_transition(machine, writer, depth + 2, transition)
        writer.add(depth + 1, '</TransitionList>')
    if children:
        write_state_list(machine, writer, depth + 1, children, state.get('initial'),
                         '{}Start'.format(state['name']))
    writer.add(depth, '</State>')


def write_state_list(machine, writer, depth, states, initial, start_name):
    """A StateList, led by the Start pseudo-state that names the first real one."""
    if not initial:
        fail('"{}" has states but no "initial" naming the one it starts in'
             .format(start_name.replace('Start', '') or 'the machine'))
    if initial not in machine.state_names:
        fail('"initial" names "{}", which is not a state'.format(initial))
    writer.add(depth, '<StateList>')
    writer.add(depth + 1, '<State ID="{}" Name="{}" Kind="Start">'
               .format(writer.ident(), esc(start_name)))
    writer.add(depth + 2, '<TransitionList>')
    writer.add(depth + 3, '<Transition ID="{}" Kind="Initial" To="{}"/>'
               .format(writer.ident(), machine.ids[initial]))
    writer.add(depth + 2, '</TransitionList>')
    writer.add(depth + 1, '</State>')
    for state in states:
        write_state(machine, writer, depth + 1, state)
    writer.add(depth, '</StateList>')


def declaration_list(machine, writer, depth, tag, entries, emit):
    if not entries:
        return
    writer.add(depth, '<{}>'.format(tag))
    for entry in entries:
        emit(writer, depth + 1, entry)
    writer.add(depth, '</{}>'.format(tag))


def build(spec):
    machine = Machine(spec)
    writer = Writer()

    # Every state, then every attribute, gets its number before anything refers to one.
    reserve(machine, writer, machine.states)
    for attribute in machine.attributes:
        machine.attribute_ids[attribute['name']] = writer.ident()

    writer.add(0, '<?xml version="1.0" encoding="UTF-8" standalone="yes"?>')
    writer.add(0, '<StateMachine FormatVersion="{}">'.format(FORMAT_VERSION))
    writer.add(1, '<Overview ID="{}" Name="{}" Version="{}" Threading="{}">'.format(
        writer.ident(), esc(machine.name), esc(spec.get('version', '1.0.0')),
        esc(spec.get('threading', 'Local'))))
    described(writer, 2, spec)
    writer.add(1, '</Overview>')

    def attribute(w, d, entry):
        w.add(d, '<Attribute ID="{}" Name="{}" DataType="{}" Value="{}">'.format(
            machine.attribute_ids[entry['name']], esc(entry['name']),
            esc(entry.get('type', 'bool')), esc(entry.get('value', 'false'))))
        described(w, d + 1, entry)
        w.add(d, '</Attribute>')

    def simple(tag, extra=None):
        def emit(w, d, entry):
            head = '<{} ID="{}" Name="{}"'.format(tag, w.ident(), esc(entry['name']))
            for key, default in (extra or ()):
                head += ' {}="{}"'.format(key, esc(entry.get(key.lower(), default)))
            if entry.get('description'):
                w.add(d, head + '>')
                described(w, d + 1, entry)
                w.add(d, '</{}>'.format(tag))
            else:
                w.add(d, head + '/>')
        return emit

    def method(kind):
        def emit(w, d, entry):
            head = '<Method ID="{}" Name="{}" MethodType="{}"'.format(
                w.ident(), esc(entry['name']), kind)
            params = entry.get('params') or []
            if not params and not entry.get('description'):
                w.add(d, head + '/>')
                return
            w.add(d, head + '>')
            described(w, d + 1, entry)
            if params:
                w.add(d + 1, '<ParamList>')
                for param in params:
                    if not param.get('name'):
                        fail('every parameter of "{}" needs a name'.format(entry['name']))
                    phead = '<Parameter ID="{}" Name="{}" DataType="{}"'.format(
                        w.ident(), esc(param['name']), esc(param.get('type', 'uint32')))
                    if param.get('description'):
                        w.add(d + 2, phead + '>')
                        described(w, d + 3, param)
                        w.add(d + 2, '</Parameter>')
                    else:
                        w.add(d + 2, phead + '/>')
                w.add(d + 1, '</ParamList>')
            w.add(d, '</Method>')
        return emit

    declaration_list(machine, writer, 1, 'AttributeList', machine.attributes, attribute)
    declaration_list(machine, writer, 1, 'EventList', machine.events, simple('Event'))
    declaration_list(machine, writer, 1, 'TimerList', machine.timers,
                     simple('Timer', (('Timeout', 300), ('Repeat', 1))))

    methods = [(t, 'Trigger') for t in machine.triggers] + \
              [(a, 'Action') for a in machine.actions]
    if methods:
        writer.add(1, '<MethodList>')
        for entry, kind in methods:
            method(kind)(writer, 2, entry)
        writer.add(1, '</MethodList>')

    write_state_list(machine, writer, 1, machine.states, spec.get('initial'), 'Start')
    writer.add(0, '</StateMachine>')
    return writer.text()


EXAMPLE = {
    'name': 'Gate',
    'description': 'Opens to a width, then closes.',
    'attributes': [{'name': 'Width', 'type': 'uint32', 'value': '0'}],
    'timers': [{'name': 'StepTimer', 'timeout': 300}],
    'triggers': [{'name': 'open', 'params': [{'name': 'width', 'type': 'uint32'}]},
                 {'name': 'close'}],
    'actions': [{'name': 'on_opening'}, {'name': 'on_open'}, {'name': 'on_closed'}],
    'initial': 'CLOSED',
    'states': [
        {'name': 'CLOSED',
         'transitions': [{'on': 'open', 'to': 'OPENING', 'set': {'Width': 'param:width'}}]},
        {'name': 'OPENING',
         'entry': ['on_opening', 'start StepTimer'],
         'exit': ['stop StepTimer'],
         'transitions': [{'on': 'StepTimer', 'to': 'OPEN'}]},
        {'name': 'OPEN',
         'entry': ['on_open'],
         'transitions': [{'on': 'close', 'to': 'CLOSED', 'do': ['on_closed']}]},
    ],
}


def main():
    parser = argparse.ArgumentParser(description=__doc__.splitlines()[0])
    parser.add_argument('--spec', help='the JSON description of the machine')
    parser.add_argument('--out', help='the .fsml document to write')
    parser.add_argument('--example', action='store_true',
                        help='print a whole spec to copy, and write nothing')
    parser.add_argument('--force', action='store_true', help='overwrite an existing document')
    args = parser.parse_args()

    if args.example:
        print(json.dumps(EXAMPLE, indent=2))
        return 0
    if not args.spec or not args.out:
        parser.error('--spec and --out are both required, or --example')

    try:
        with open(args.spec, encoding='utf-8') as handle:
            spec = json.load(handle)
    except (OSError, ValueError) as problem:
        fail('cannot read {}: {}'.format(args.spec, problem))

    document = build(spec)
    if os.path.exists(args.out) and not args.force:
        fail('{} exists; pass --force to overwrite it'.format(args.out))
    folder = os.path.dirname(os.path.abspath(args.out))
    if folder:
        os.makedirs(folder, exist_ok=True)
    with open(args.out, 'w', encoding='utf-8') as handle:
        handle.write(document)
    print('wrote {}'.format(args.out))
    print('  {} state(s), {} trigger(s), {} action(s). Add the <Layout> block with '
          'fsml_layout.py'.format(len(spec.get('states') or []),
                                  len(spec.get('triggers') or []),
                                  len(spec.get('actions') or [])))
    return 0


if __name__ == '__main__':
    sys.exit(main())
