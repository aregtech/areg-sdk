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
import textwrap
import xml.etree.ElementTree as ET

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import codegen_names  # noqa: E402

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
    # Output already printed is flushed first: stdout is block-buffered into a
    # pipe, so without this the error reaches the reader before the lines it is
    # about.
    sys.stdout.flush()
    sys.stderr.write('error: {}\n'.format(message))
    sys.exit(1)


# Every hole a generated file leaves carries its own name, so the line is unique in
# the file and an Edit can address it without reading the file back.
def marker(slot, what, indent=8):
    """One TODO(you) line, named after the slot it fills."""
    return '{}// TODO(you) {}: {}.'.format(' ' * indent, slot, what)


def runs_on(machine, action, most=4):
    """Where an action runs, as the tail of its marker, or nothing for one place."""
    places = machine.runs_on.get(action) or []
    if len(set(place.split(' in ')[0] for place in places)) < 2:
        return ''
    shown = ', '.join(places[:most])
    more = ' (+{} more)'.format(len(places) - most) if len(places) > most else ''
    return '; it runs on {}{} and cannot tell which'.format(shown, more)


# A line written only so the skeleton runs before any rule is filled in. It belongs to
# the marker above it and goes when that marker is filled: fill_markers.py drops it,
# and an Edit replaces it along with the marker line. Lines after a marker that carry
# no tag are real code and stay.
PLACEHOLDER_TAG = '   // placeholder(you)'


def placeholder(line):
    """One line that stands only until the marker above it is filled."""
    return line + PLACEHOLDER_TAG


MARKER = re.compile(r'//\s*TODO\(you\)\s+([A-Za-z_][\w]*)\s*:\s*(.*?)\s*$')

# The tool that fills every marker of a project in one command, and the two files
# around it. The worksheet is written by the generator: the sections, their order
# and the names each body may call. The bodies file is written by the author and
# is the one the filler reads; the generator never writes it.
FILLER = os.path.join(os.path.dirname(os.path.abspath(__file__)),
                      'fill_markers.py').replace(os.sep, '/')
BUILDER = os.path.join(os.path.dirname(os.path.abspath(__file__)),
                       'build_project.py').replace(os.sep, '/')
WORKSHEET = 'worksheet.txt'
BODIES = 'bodies.txt'


# Every marker of a project is filled in one command, from a file the generator
# writes. A request costs its whole context again, so N requests of one Edit cost
# N times what one worksheet costs.
WORKSHEET_NOTE = (
    '  {total} hole(s) in {files} file(s). {path} is written beside this project:\n'
    '  one section for each, in order, with the function it sits in and every name\n'
    '  a body may call. It is {lines} line(s): read it whole, in one call, and do\n'
    '  not page through it. It is rewritten on every generation, so no body goes\n'
    '  in it: write {bodies} in one call, each "== <marker>" line with its code under\n'
    '  it. Then apply it, build and run the scenarios in one command:\n'
    '\n'
    '    python3 {build} --spec design.json --run\n'
    '\n'
    '  A project with no design.json applies it with python3 {tool} --bodies {bodies}.\n'
    '  A line tagged "// placeholder(you)" under a marker goes when that marker is\n'
    '  filled; a line with no tag is real code. gen_skeleton.py --todos lists the\n'
    '  markers left, each exactly as it stands in its file.')


# The worksheet rewritten by a later generation.
WORKSHEET_AGAIN = (
    '  {total} hole(s) in {files} file(s). {path} is rewritten from this design,\n'
    '  {lines} line(s); {bodies} applies to it as before.')


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
    today is to open the file. A helper is listed with its parameters: a name given
    without them is called with the wrong ones, and that costs a build.
    """
    members, helpers = [], []
    for line in text.splitlines():
        if ' final' in line or '= delete' in line or 'AREG_NOCOPY' in line:
            continue
        # A placeholder stands until its marker is filled, and the line that fills it
        # declares the same name. Listing it as taken tells the reader not to write
        # the one line that marker asks for.
        if PLACEHOLDER_TAG.strip() in line:
            continue
        found = DECLARED.match(line)
        if found and found.group(1) not in members:
            members.append(found.group(1))
            continue
        found = HELPER.match(line)
        if found and found.group(1) not in RESERVED:
            spelt = ' '.join(line.split()).rstrip('{;').strip()
            if not any(h.endswith(spelt) or spelt in h for h in helpers):
                helpers.append(spelt)
    return members, helpers


# The comment a generated header writes above a helper it declares.
DOC_LINE = re.compile(r'^\s*//!\s?(.*?)\s*$')


def helper_docs(text):
    """What each helper of this class does, by the signature the worksheet spells.

    The header documents every helper it declares in one line. Listing the signature
    and dropping that line leaves a name with no behaviour behind it, and a body that
    has to know what it does opens the generated file to find out: one measured run
    spent seven requests there. The line is already written; this carries it.
    """
    found = {}
    pending = []
    for line in text.splitlines():
        doc = DOC_LINE.match(line)
        if doc:
            pending.append(doc.group(1))
            continue
        if not line.strip():
            pending = []
            continue
        hit = HELPER.match(line)
        if hit and hit.group(1) not in RESERVED and pending \
                and ' final' not in line and '= delete' not in line:
            spelt = ' '.join(line.split()).rstrip('{;').strip()
            found.setdefault(spelt, ' '.join(pending))
        pending = []
    return found


def print_todos(produced, out, written, holes=0, scenarios='', first=True):
    """How many holes each generated file leaves, and where the worksheet is.

    The instructions come with the first worksheet only; a rewritten one gets a line.

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
        print('  {} leaves {} hole(s)'.format(scenarios, holes))
    total += holes
    files += 1 if holes else 0
    if not total:
        return
    lines = 0
    try:
        with open(WORKSHEET, encoding='utf-8') as handle:
            lines = sum(1 for _ in handle)
    except OSError:
        pass
    if not first:
        print(WORKSHEET_AGAIN.format(total=total, files=files, path=WORKSHEET,
                                     bodies=BODIES, lines=lines))
        return
    print(WORKSHEET_NOTE.format(total=total, files=files, tool=FILLER, build=BUILDER,
                                path=WORKSHEET, bodies=BODIES, lines=lines))


# ---------------------------------------------------------------------------
# The worksheet: one file carrying every open marker, the names the generated
# classes already carry, and the contract the bodies are written against. It is
# written where the markers are written, so nothing has to be recalled from an
# earlier command or looked up in a second one.
# ---------------------------------------------------------------------------
WORKSHEET_HEAD = """\
#| The worksheet of this project: one section per open marker, in file order.
#| The generator rewrites this file on every generation, so no body goes in it.
#| The bodies go in {bodies}, a file of your own written in one call: for each
#| section, its "==" line and under it the code that replaces that marker. Then
#| apply it, build and run the scenarios in one command:
#|
#|   python3 {build} --spec design.json --run
#|
#| A project with no design.json applies it with python3 {tool} --bodies {bodies}.
#|
#| A line starting with "#|" never reaches a source, so a section copied from here
#| with its notes still applies. Everything else under a "==" line is code, copied
#| as written: a comment in a body is "//", not "#". A section left out, or left
#| with no code, stays open and nothing is written for it, so one pass can fill
#| what it knows and a later pass the rest.
#|
#| A section that needs nothing still takes one line: a "//" comment saying so
#| closes it. This holds for every section, the "*_state" ones included. A marker
#| left open is an error of the final contract check, after the build and the
#| scenarios have already passed.
#|
#| Writing {bodies} and running that command is two requests. Filling it one
#| section at a time is {total} requests instead, and a request is billed for the
#| whole conversation again.
#|
#| {bodies} keeps every body it writes. A body already written stays addressable by
#| the same section: change the section, run the command again, and that body alone
#| is rewritten where it stands. A scenario that fails names the section its check is
#| in, so the section to change is the one the failure printed, and no source file is
#| opened to find it.
#|
#| No source file has to be opened to fill this in: every name a body may call is
#| named below, every place a body belongs is a section below, and each section
#| names the function it sits in. The filler reports the line every body landed on,
#| so no file has to be opened afterwards either.
#|
#| A helper of your own is declared and defined in the "*_state" section, which is
#| the private block of that class's header. Every other section is code inside
#| an existing function body, so an out-of-line definition there does not compile.
#| Nothing else has to be added by hand.
#|
#| A body prints with "std::cout << ... << std::endl;". Every .cpp of this project
#| includes <iostream> already, so a section that prints adds no include.
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
    """Every unfilled hole of a scenario file, as (marker, what the section asks).

    A hole is an expectation or the trigger of a "stop". It is not code, so it is not
    a line of a source file, but it is a decision the run has to make and the
    worksheet is where the decisions live.
    """
    holes = []
    try:
        with open(path, encoding='utf-8') as handle:
            document = json.load(handle)
        scenarios = document['scenarios']
    except (ValueError, OSError, KeyError, TypeError):
        return holes
    for scenario in scenarios:
        label = scenario.get('name', '')
        procs = scenario.get('procs', [])
        for spec in procs:
            for entry in spec.get('expect', []):
                found = MARKER.search('// ' + entry) if entry.startswith('TODO(you)') \
                    else None
                if found:
                    holes.append((found.group(1), 'what "{}" must print in scenario '
                                  '"{}".'.format(proc_label(spec), label)))
        stop = scenario.get('stop')
        after = stop.get('after') if isinstance(stop, dict) else None
        found = MARKER.search('// ' + after) \
            if isinstance(after, str) and after.startswith('TODO(you)') else None
        if found and procs:
            lead = next((spec for spec in procs if spec.get('lead')), procs[-1])
            holes.append((found.group(1), STOP_HINT.format(
                lead=proc_label(lead), scenario=label, target=stop.get('proc'))))
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


# File-scope, not section-scope: it is true of every body that waits for anything,
# and a reader meets it in whichever section their own wait is written in.
ORDER_NOTE = ['A response and an update are two deliveries, not one. A response is',
              'bound to its request and reaches only that caller. An update is bound',
              'to the attribute and reaches every subscriber whenever the value is',
              'set; it answers to no request, response or broadcast. Neither waits',
              'for the other, so test the value already held before waiting for an',
              'update that may have arrived already, or the wait never ends.']


STEPS_NOTE = ['a step_ section runs only while its step is current. fail("why") ends the',
              'run with exit 1, stay() keeps the step for the next arrival, and',
              'go_to(Step::Name) picks the next step. Doing none of them goes on to the',
              'step listed next, and after the last one the run exits 0. Each of the',
              'three takes effect when the body is left, so leaving early by return',
              'changes nothing: what the body called last is what happens. A step with',
              'nothing to check still takes one line: a // comment saying so']


# The same contract as STEPS_NOTE, short enough to repeat on every later step_
# section. A worksheet states it once at the top and a run reads section nine.
STEP_BRIEF = ['falling through the end of this body ends the step and starts the next.',
              'stay() holds it, go_to(Step::Name) redirects, fail("why") stops, and an',
              'early return keeps whichever of them the body already called']


# A response no step awaits gets no step machinery at all, and the generated file is
# where that was read until it was written here.
ANSWER_NOTE = ['no step awaits this answer, so this body sits outside the step',
               'sequence and nothing in it ends a step. complete() moves on only',
               'when no stay() is outstanding: the first complete() after a stay()',
               'clears it and returns. A step that has to react to this answer',
               'awaits it in design.json, which puts its check in a step_ section']


# The section that is a header's private block. Two runs opened a generated header
# to ask where a helper of their own may live; the preamble says it 130 lines above.
STATE_NOTE = ["this section is the private block of that class's header: a helper of",
              'your own is declared and defined here, and nowhere else. Every other',
              'section is code inside an existing function body, where an out-of-line',
              'definition does not compile. The names already taken are listed at the',
              'top of this worksheet, and declaring one again shadows it']


# Sends a section to the bodies quoted at the top rather than to the file they are
# in. It goes on the first section of the file they came from.
DRIVEN_POINTER = ('the generated {} of this file {} quoted whole at the top of this '
                  'worksheet, under "What these do", so no source file is opened to '
                  'see what surrounds a body')


# The generated function every section of a consumer .cpp sits downstream of, too
# long to quote and not covered by any section. A scenario with steps is entered
# from it; a scenario without them sends its first request inside it.
CONNECT_NOTE = ['service_connected() is generated and runs before any body below: it',
                'subscribes to every broadcast and attribute the document declares,',
                'starts the pace timer and enters the first step. It runs again on',
                'every reconnection, and the step the scenario reached is kept']

CONNECT_INSIDE = ['the rest of service_connected() is generated: every broadcast and',
                  'attribute the document declares is subscribed to above this body,',
                  'and the pace timer starts below it. It runs again on every',
                  'reconnection, so this body sends the first request each time']


UPDATE_NOTE = ['an update_ body runs on every arrival, whatever step is current, and',
               'before the step_ check of the same update. That check runs only while',
               'its step is current; an arrival on any other step is dropped there.',
               'Both run inside the check the generated handler makes, so the value is',
               'valid and no test of state is needed:',
               '    if (state == areg::DataState::DataIsOK)',
               '    {',
               '        <the body>']


PEER_LOST_NOTE = ['after this body the reconnect deadline starts: a provider not back within',
                  'cReconnectSeconds ends the run through fail(), exit 1, so the body only',
                  'reports the loss. A reconnect_seconds of 0 waits for ever instead']


REFUSED_NOTE = ['after this body the run ends with quit_with(1)']


# Formatted with the pacing interval, which is declared further down.
PACE_NOTE = ['this body runs once per pace tick, every {} ms, and only then. A request',
             'that has to follow an answer or an update at once is sent from that',
             'answer\'s or update\'s own body: a provider can finish a timed stage in',
             'less than a tick. Call progressed() wherever the scenario advances, or',
             'the stall watchdog ends the run after cStallTicks ticks']


def header_notes(sections, awaited=()):
    """The warnings that belong to the whole worksheet, in the order they are shown.

    A fact stated on a page the build path does not open is a fact a run pays to
    rediscover, so it is written here. A fact about two bodies is written once at the
    top and not under whichever of them happens to come first: the reader meets it
    before any section, and it cannot land on the section they are not filling.

    A step that awaits a response or an update carries its check in a "step_" section
    rather than a "response_" or "update_" one, so the sections alone do not say that
    both deliveries are in play. "awaited" is the kinds the steps wait on, and without
    it a scenario -- the one shape where the race decides the order of the steps --
    is the one worksheet the note is missing from.
    """
    answers = [name for name, _, _, _, _ in sections if name.startswith('response_')]
    updates = [name for name, _, _, _, _ in sections if name.startswith('update_')]
    return ORDER_NOTE if (answers or 'response' in awaited) \
        and (updates or 'update' in awaited) else []


def spelt(names):
    """A list of names as a sentence reads them."""
    return names[0] if len(names) < 2 else \
        ', '.join(names[:-1]) + ' and ' + names[-1]


def connecting(produced):
    """The .cpp files that carry a generated service_connected()."""
    return set(file_name for file_name, body in produced
               if file_name.endswith('.cpp') and '::service_connected(' in body)


def first_sections(sections):
    """The first section of each .cpp, keyed by the class the file is named after."""
    first = {}
    for name, _, _, file_name, _ in sections:
        if file_name.endswith('.cpp'):
            first.setdefault(os.path.basename(file_name)[:-4], (name, file_name))
    return first


def section_notes(sections, driven=(), connected=()):
    """The warnings that belong to one section, keyed by its marker name.

    "driven" is what driven_body() quoted and "connected" the files with a generated
    service_connected(): both name a body no section covers, and the note that points
    at it goes on the first section of its own file.
    """
    notes = {}
    updates = [name for name, _, _, _, _ in sections if name.startswith('update_')]
    if updates:
        notes[updates[0]] = UPDATE_NOTE
    # The brief goes on the first step_ section of each handler, not on all of them:
    # the sections of one handler are read together.
    checks = [(name, signature) for name, _, _, _, signature in sections
              if name.startswith('step_')]
    if checks:
        notes[checks[0][0]] = STEPS_NOTE
    seen = set(signature for _, signature in checks[:1])
    for name, signature in checks[1:]:
        if signature not in seen:
            seen.add(signature)
            notes[name] = STEP_BRIEF
    for name, _, _, _, _ in sections:
        if name.startswith('response_'):
            notes[name] = ANSWER_NOTE
    # The only marker a header carries is its class's private block. Matching the
    # name instead takes "update_machine_state" with it.
    for name, _, _, file_name, _ in sections:
        if file_name.endswith('.hpp'):
            notes[name] = STATE_NOTE
    names = [name for name, _, _, _, _ in sections]
    if 'peer_lost' in names:
        notes['peer_lost'] = PEER_LOST_NOTE
    if 'service_refused' in names:
        notes['service_refused'] = REFUSED_NOTE
    if any(name == 'next_step' for name, _, _, _, _ in sections):
        notes['next_step'] = [line.format(STEP_INTERVAL_MS) for line in PACE_NOTE]
    # Last, and appended: the first section of a file carries whatever note that
    # section earned on its own, and the pointer to the file's own generated bodies
    # after it.
    quoted = {}
    for cls, wanted, _ in driven:
        quoted.setdefault(cls, []).append(wanted + '()')
    for cls, (name, file_name) in first_sections(sections).items():
        said = []
        if cls in quoted:
            said += textwrap.wrap(DRIVEN_POINTER.format(
                spelt(quoted[cls]), 'is' if len(quoted[cls]) < 2 else 'are'), 74)
        if file_name in connected:
            said += CONNECT_NOTE if checks else CONNECT_INSIDE
        if said:
            notes[name] = notes.get(name, []) + said
    return notes


# The generated functions whose behaviour no name gives away. A worksheet lists
# mPace, cStallTicks, mStep and complete() as taken, and a body that has to know when
# the stall watchdog fires, or when mStep moves, cannot read that out of the names:
# one measured run opened seven generated files for it. These bodies are the answer.
# "head" quotes down to the switch, whose cases are the sections of this worksheet.
DRIVEN_BY = (('process_timer', 'all'), ('complete', 'all'), ('begin', 'head'))

# A body longer than this is a generated switch, one case per step, and the worksheet
# already carries a section per case. The limit only ever drops such a body: quoting
# fewer lines than the reader needs is how this facility silently stopped applying to
# any project with a timed step, whose process_timer runs one line past 20.
DRIVEN_LIMIT = 32
DRIVEN_HEAD = ('#| What these do, which no name above says. This is the generated\n'
               '#| code, quoted so no file has to be opened for it:\n#|')


def driven_body(produced):
    """Each generated function of DRIVEN_BY, as (class, name, lines), in file order."""
    found = []
    for file_name, text in produced:
        if not file_name.endswith('.cpp'):
            continue
        lines = text.splitlines()
        for wanted, keep in DRIVEN_BY:
            body = one_driven(lines, wanted, keep)
            if body:
                found.append((os.path.basename(file_name)[:-4], wanted, body))
    return found


def one_driven(lines, wanted, keep):
    """One generated function of a .cpp, quoted whole or down to its switch."""
    for index, line in enumerate(lines):
        if '::' + wanted + '(' not in line or line.startswith(' '):
            continue
        end = index
        while end < len(lines) and lines[end] != '}':
            end += 1
        body = lines[index:end + 1]
        if keep == 'head':
            cut = next((n for n, one in enumerate(body)
                        if one.strip().startswith('switch')), None)
            if cut is None:
                return []
            return body[:cut] + ['    // ... one case per step, each of them a '
                                 'section of this worksheet.', '}']
        # A body that is nothing but its own marker teaches nothing. One that
        # carries generated control flow is quoted whole, the marker included:
        # where the author's own code goes relative to it is the other half of
        # the answer.
        real = [one for one in body[2:-1]
                if one.strip() and not MARKER.search(one)
                and PLACEHOLDER_TAG.strip() not in one]
        return body if real and len(body) <= DRIVEN_LIMIT else []
    return []


def worksheet_lines(produced, out, iface, document, machine, machine_doc,
                    scenarios=None, steps=()):
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
                carried[file_name] = (os.path.basename(file_name)[:-4], members, helpers,
                                      helper_docs(text))

    lines = [WORKSHEET_HEAD.format(tool=FILLER, build=BUILDER, bodies=BODIES,
                                   total=len(sections) + len(holes))]
    if carried:
        lines.append('#| These names are taken already. Declaring one of them again in\n'
                     '#| a section below shadows it, and the compiler points at the line\n'
                     '#| that reads the wrong one, not at the declaration. A helper is\n'
                     '#| listed with what it does, so a body calls it without reading\n'
                     '#| the generated file it is declared in:\n#|')
        for _, (cls, members, helpers, docs) in sorted(carried.items()):
            lines.append('#|   {}'.format(cls))
            if members:
                lines.append('#|     members: ' + ', '.join(members))
            for spelt in helpers:
                lines.append('#|     {}'.format(spelt))
                said = docs.get(spelt)
                if said:
                    lines.append('#|         {}'.format(said))
        lines.append('#|')
    if steps:
        lines.append('#| The steps of the scenario, in order. go_to() takes one of these\n'
                     '#| names; a step with no step_ section below has nothing to check:\n#|')
        width = max(len(step['enum']) for step in steps) + 6
        for step in steps:
            lines.append('#|   {:{}} {}'.format('Step::' + step['enum'], width,
                                               step_said(step)))
        lines.append('#|')
    driven = driven_body(produced)
    if driven:
        lines.append(DRIVEN_HEAD)
        for cls, _, body in driven:
            for line in body:
                lines.append(('#|   ' + line).rstrip())
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
    heading = header_notes(sections, set(step['awaits'][0] for step in steps
                                         if step['awaits']))
    for line in heading:
        lines.append(('#| ' + line).rstrip())
    if heading:
        lines.append('#|')

    notes = section_notes(sections, driven, connecting(produced))
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
        lines.append('#| nothing. A "//" line is a pattern here, not a comment.')
        lines.append('#| The dialect is Python re, searched and not anchored, with')
        lines.append('#| MULTILINE on: ^ and $ meet every line, and a plain substring')
        lines.append('#| is a valid pattern. Escape . ( ) [ ] * + ? that mean themselves.')
        for name, hint in holes:
            lines.append('== {}'.format(name))
            for line in textwrap.wrap(hint, 77):
                lines.append('#| {}'.format(line))
            lines.append('')
    return lines


def write_worksheet(produced, out, iface, document, machine, machine_doc,
                    scenarios=None, steps=()):
    """Write the worksheet. It carries no work, so it is rewritten every time.

    A bodies file holding only notes and empty sections is removed: it is the
    worksheet an earlier generator wrote in its place, and it carries no work either.
    """
    lines = worksheet_lines(produced, out, iface, document, machine, machine_doc,
                            scenarios, steps)
    if not lines:
        return None
    with open(WORKSHEET, 'w', encoding='utf-8', newline='\n') as handle:
        handle.write('\n'.join(lines).rstrip() + '\n')
    if os.path.exists(BODIES) and worksheet_pristine(BODIES):
        os.remove(BODIES)
    return True


def fields_of(declared):
    """The field names of one enumeration declaration, in document order."""
    return [entry.get('Name')
            for entry in declared.findall('./FieldList/EnumEntry')
            if entry.get('Name')]


class Interface:
    """The parts of a .siml document the components have to match."""

    def __init__(self, path):
        self.path = path
        self._generated = None
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
        # The field names of every enumeration in reach, by every spelling of its type.
        self.enum_fields = {}
        for declared in root.findall('./DataTypeList/DataType'):
            kind = (declared.get('Type') or '').lower()
            type_name = declared.get('Name')
            if not type_name:
                continue
            self.declared.add(type_name)
            self.types.append((type_name, kind))
            if kind in ('enumeration', 'enumerate'):
                self.by_value.add(type_name)
                self.enum_fields[type_name] = fields_of(declared)

        self.constants = []
        for constant in root.findall('./ConstantList/Constant'):
            if constant.get('Name'):
                self.constants.append((constant.get('Name'),
                                       constant.get('DataType')))

        self._read_included(root, path)

        self.requests = []
        # The response each request is answered with, by request name.
        self.response_of = {}
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
                if method.get('Response'):
                    self.response_of[method.get('Name')] = method.get('Response')
            elif kind == 'response':
                self.responses.append(entry)
            elif kind == 'broadcast':
                self.broadcasts.append(entry)
            elif kind == 'action':
                self.actions.append(entry)
            elif kind == 'trigger':
                self.triggers.append(entry)

        # Where each action runs: "<stimulus> in <state>", one per transition.
        self.runs_on = {}
        for state in root.iter('State'):
            for move in state.findall('./TransitionList/Transition'):
                if not move.get('Stimulus'):
                    continue
                for call in move.iter('ActionCall'):
                    where = '{} in {}'.format(move.get('Stimulus'), state.get('Name'))
                    self.runs_on.setdefault(call.get('Action'), []).append(where)

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
                    self.enum_fields[spelling] = fields_of(declared)

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

    def generated(self):
        """The names codegen.jar generated for this document."""
        if self._generated is None:
            try:
                self._generated = codegen_names.names_of_one(self.path)
            except codegen_names.CodegenError as error:
                fail(str(error))
        return self._generated

    def spell(self, kind, name, role='call'):
        """The C++ name codegen.jar gave one role of one element of this document."""
        try:
            return self.generated().name(kind, name, role)
        except codegen_names.CodegenError as error:
            fail(str(error))

    def generated_params(self, kind, name, role='call'):
        """The parameter list codegen.jar declared for that role, defaults removed."""
        self.spell(kind, name, role)
        return self.generated().params(kind, name, role)


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
        header.append('    void {}({}) final;'.format(iface.spell('request', name),
                                                     iface.generated_params('request', name)))
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
              '    , {}ProviderBase(static_cast<areg::Component &>(*this))'.format(iface.name),
              '{']
    if iface.attributes:
        source.append('    // An attribute is invalid until it is set once.')
        for attr_name, type_name in iface.attributes:
            source.append('    // {}( value );'.format(
                iface.spell('attribute', attr_name, 'set')))
    source += ['}', '']

    responses = {name for name, _ in iface.responses}
    for name, params in iface.requests:
        source.append('void {}::{}({})'.format(class_name, iface.spell('request', name),
                                               iface.generated_params('request', name)))
        source.append('{')
        source.append('    // TODO: implement the request.')
        if name in responses:
            answer = next(p for n, p in iface.responses if n == name)
            if answer:
                # The answer carries values this stub cannot invent, so the call
                # is written out with its types for the implementation to fill.
                source.append('    // Answer with:')
                source.append('    // {}({});'.format(
                    iface.spell('response', name),
                    iface.generated_params('response', name).strip()))
            else:
                source.append('    {}();'.format(iface.spell('response', name)))
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
        header.append('    void {}({}) final;'.format(iface.spell('response', name),
                                                     iface.generated_params('response', name)))
    for name, params in iface.requests:
        header.append('    //!< The {} request could not be executed.'.format(name))
        header.append('    void {}({}) final;'.format(
            iface.spell('request', name, 'failed'),
            iface.generated_params('request', name, 'failed').strip()))
    for name, params in iface.broadcasts:
        header.append('    //!< Broadcast {}.'.format(name))
        header.append('    void {}({}) final;'.format(iface.spell('broadcast', name),
                                                     iface.generated_params('broadcast', name)))
    for attr_name, type_name in iface.attributes:
        header.append('    //!< New value of the {} attribute.'.format(attr_name))
        header.append('    void {}({}) final;'.format(
            iface.spell('attribute', attr_name, 'on_update'),
            iface.generated_params('attribute', attr_name, 'on_update').strip()))
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
        source.append('            {}(true);'.format(
            iface.spell('attribute', attr_name, 'notify')))
    for name, _ in iface.broadcasts:
        source.append('            {}(true);'.format(iface.spell('broadcast', name, 'notify')))
    source += ['            // TODO: send the first request here.',
               '        }',
               '    }',
               '',
               '    return result;',
               '}',
               '']

    for name, params in iface.responses:
        source += ['void {}::{}({})'.format(class_name, iface.spell('response', name),
                                            iface.generated_params('response', name)),
                   '{', '    // TODO: handle the answer.', '}', '']
    for name, _ in iface.requests:
        source += ['void {}::{}(areg::ResultType /*reason*/)'.format(
                       class_name, iface.spell('request', name, 'failed')),
                   '{', '    // TODO: retry when is_connected(), or report.', '}', '']
    for name, params in iface.broadcasts:
        source += ['void {}::{}({})'.format(class_name, iface.spell('broadcast', name),
                                            iface.generated_params('broadcast', name)),
                   '{', '    // TODO: handle the broadcast.', '}', '']
    for attr_name, type_name in iface.attributes:
        source += ['void {}::{}({})'.format(
                       class_name, iface.spell('attribute', attr_name, 'on_update'),
                       iface.generated_params('attribute', attr_name, 'on_update').strip()),
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
        header.append('    void {}({}) final;'.format(iface.spell('action', name),
                                                     iface.generated_params('action', name)))
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
        source += ['void {}::{}({})'.format(class_name, iface.spell('action', name),
                                            iface.generated_params('action', name)),
                   '{',
                   '    // TODO: perform the effect. Never raise a stimulus from here.',
                   '}',
                   '']
    if iface.triggers:
        source += ['// Stimulus the machine accepts, to be called from a request handler:',
                   '//   ' + '  '.join('mFsm.{}();'.format(iface.spell('trigger', name))
                                       for name, _ in iface.triggers),
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
        # A trigger and an attribute are reached through the machine object; an action
        # and a condition are overrides on the component itself.
        out.append('  the machine object is mFsm in a generated provider: a "call" '
                   'below is mFsm.<name>(...)')
        # Every name below is read from what codegen.jar generated for the document.
        for name, params in iface.triggers:
            out.append('  call     bool {}({})'.format(iface.spell('trigger', name),
                                                     iface.generated_params('trigger', name)))
        for name, params in iface.actions:
            out.append('  override void {}({})'
                       .format(iface.spell('action', name),
                               iface.generated_params('action', name)))
        for name, params, returns in iface.conditions:
            if not iface.generated().has('condition', name):
                continue
            out.append('  override {} {}({})'.format(
                iface.generated().returns('condition', name), iface.spell('condition', name),
                iface.generated_params('condition', name)))
        for name, kind in iface.attributes:
            out.append('  call     {} {}() / void {}({})'
                       .format(iface.cpp_type(kind)[0],
                               iface.spell('attribute', name, 'get'),
                               iface.spell('attribute', name, 'set'),
                               iface.attribute_setter(kind, True)))
        return out + type_lines(iface)
    if document.lower().endswith('.dtml'):
        out.append('classes:   none. {} is the namespace the types below are spelled in'
                   .format(iface.name))
        return out + type_lines(iface)
    out.append('classes:   {n}Provider and {n}Consumer build on the generated {n} base'
               .format(n=iface.name))
    for name, params in iface.requests:
        spelled = iface.spell('request', name)
        out.append('  provider overrides {}({}); consumer calls {}(...) '
                   'to send it'.format(spelled, iface.generated_params('request', name),
                                       spelled))
    for name, params in iface.responses:
        out.append('  provider calls {}({}); consumer overrides it'
                   .format(iface.spell('response', name),
                           iface.generated_params('response', name)))
    for name, params in iface.broadcasts:
        out.append('  provider calls {}({}); consumer subscribes with '
                   '{}(true)'
                   .format(iface.spell('broadcast', name),
                           iface.generated_params('broadcast', name),
                           iface.spell('broadcast', name, 'notify')))
    for name, kind in iface.attributes:
        spelled = iface.spell('attribute', name, 'get')
        cpp = iface.cpp_type(kind)[0]
        read = iface.passed_as(kind)
        out.append('  provider calls {}({}); consumer subscribes with '
                   '{}(true)'
                   .format(iface.spell('attribute', name, 'set'),
                           iface.attribute_setter(kind, False),
                           iface.spell('attribute', name, 'notify')))
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


# The headers a class that declares a timer needs.
TIMER_INCLUDES = ['#include "areg/component/Timer.hpp"',
                  '#include "areg/component/TimerConsumer.hpp"']


def timer_includes(class_lines):
    """TIMER_INCLUDES when the emitted class names an areg timer type, else nothing."""
    return TIMER_INCLUDES if any('areg::Timer' in line for line in class_lines) else []

def provider_class(iface, cls, machine=None, timers=()):
    """The provider component, with every request answered.

    Given a machine, the same component owns it: the action handler is a base, the
    machine is a member, and every action is declared here. A separate host component
    is what the pair used to need, and merging them is what removes it. Given timers,
    it is their consumer: each has a member, a start and a stop helper, and a section
    in process_timer.
    """
    pad = ' ' * (len(cls) + 13)
    lines = ['class {} final : public    areg::Component'.format(cls),
             '{}, protected {}ProviderBase'.format(pad, iface.name)]
    if machine:
        lines.append('{}, protected {}ActionHandler'.format(pad, machine.name))
    if timers:
        lines.append('{}, private   areg::TimerConsumer'.format(pad))
    lines += ['{',
              'public:',
              '    {}(const areg::ComponentEntry & entry, areg::ComponentThread & owner)'.format(cls),
              '        : areg::Component(entry, owner)',
              '        , {}ProviderBase(static_cast<areg::Component &>(*this))'.format(iface.name)]
    if machine:
        lines.append('        , {}ActionHandler()'.format(machine.name))
        lines.append('        , mFsm(static_cast<{}ActionHandler &>(self()))'.format(machine.name))
    if timers:
        lines.append('        , areg::TimerConsumer()')
        for timer in timers:
            lines.append('        , {}(static_cast<areg::TimerConsumer &>(self()), "{}")'
                         .format(timer['member'], timer['name']))
    lines.append('    {')
    if iface.attributes:
        lines.append(marker('initial_values', 'the value each attribute starts with; an '
                            'attribute is invalid until it is set once'))
        for attr_name, type_name in iface.attributes:
            lines.append(placeholder('        {}({});'.format(
                iface.spell('attribute', attr_name, 'set'), default_expr(iface, type_name))))
    lines += ['    }', '', 'protected:']

    started = [timer for timer in timers if timer['autostart']]
    if machine or started or timers:
        lines += ['    void startup_component(areg::ComponentThread & comThread) final',
                  '    {',
                  '        areg::Component::startup_component(comThread);']
        if machine:
            lines.append('        mFsm.init_fsm(&comThread);')
        lines += ['        {}();'.format(timer['start']) for timer in started]
        lines += ['    }',
                  '',
                  '    void shutdown_component(areg::ComponentThread & comThread) final',
                  '    {']
        lines += ['        {}.stop_timer();'.format(timer['member']) for timer in timers]
        if machine:
            lines.append('        mFsm.release_fsm();')
        lines += ['        areg::Component::shutdown_component(comThread);',
                  '    }',
                  '']
    if timers:
        lines += ['    void process_timer(areg::Timer & timer) final',
                  '    {']
        for index, timer in enumerate(timers):
            lines += ['        {}if (&timer == &{})'.format('' if index == 0 else 'else ',
                                                          timer['member']),
                      '        {',
                      marker(timer['slot'], 'what happens each time {} fires'
                             .format(timer['name']), 12),
                      '        }']
        lines += ['    }',
                  '']

    answered = dict((name, params) for name, params in iface.responses)
    if machine and machine.triggers:
        lines.append('    // A request handler converts the call into a stimulus and '
                     'decides nothing:')
        for name, params in machine.triggers:
            lines.append('    //   mFsm.{}({});'.format(
                machine.spell('trigger', name), ', '.join(pname for pname, _ in params)))
    for name, params in iface.requests:
        spelled = iface.spell('request', name)
        lines.append('    void {}({}) final'.format(spelled,
                                                    iface.generated_params('request', name)))
        lines.append('    {')
        lines.append(marker(spelled,
                            'the rule this request carries out'))
        if name in answered:
            args = ', '.join(default_expr(iface, t) for _, t in answered[name])
            lines.append(placeholder('        {}({});'
                                     .format(iface.spell('response', name), args)))
        lines.append('    }')
        lines.append('')

    if machine and machine.conditions:
        lines.append('    // Every condition a guard of the machine asks. Answer it and '
                     'change nothing.')
        for name, params, returns in machine.conditions:
            if not machine.generated().has('condition', name):
                continue
            lines.append('    {} {}({}) final'.format(
                machine.generated().returns('condition', name),
                machine.spell('condition', name),
                machine.generated_params('condition', name)))
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
            spelled = machine.spell('action', name)
            lines.append('    void {}({}) final'.format(spelled,
                                                        machine.generated_params('action', name)))
            lines += ['    {',
                      marker(spelled, 'perform the effect' + runs_on(machine, name)),
                      '    }',
                      '']

    for timer in timers:
        every = 'every {} ms until stopped'.format(timer['timeout']) \
            if timer['count'].endswith('CONTINUOUSLY') \
            else 'once, {} ms after the call'.format(timer['timeout']) \
            if timer['count'] == '1' \
            else '{} times, every {} ms'.format(timer['count'], timer['timeout'])
        lines += ['    //! Starts {} again from now: it fires {}.'.format(timer['name'], every),
                  '    inline void {}()'.format(timer['start']),
                  '    {',
                  '        {}.stop_timer();'.format(timer['member']),
                  '        {0}.start_timer({1}, static_cast<areg::DispatcherThread &>'
                  '(master_thread()), {2});'.format(timer['member'], timer['timeout'],
                                                    timer['count']),
                  '    }',
                  '',
                  '    //! Stops {}; safe whether or not it runs.'.format(timer['name']),
                  '    inline void {}()'.format(timer['stop']),
                  '    {',
                  '        {}.stop_timer();'.format(timer['member']),
                  '    }',
                  '']
    lines += ['private:',
              '    inline {} & self()'.format(cls),
              '    {   return (*this); }',
              '']
    if machine:
        lines += ['    {}FSM  mFsm;    //!< The state machine this component drives.'
                  .format(machine.name), '']
    for timer in timers:
        lines += ['    areg::Timer {};    //!< {}'.format(
            timer['member'], timer['description'] or 'The timer "{}".'.format(timer['name'])),
            '']
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


def pascal(name):
    """order_latte -> OrderLatte; OrderLatte stays as it is."""
    return ''.join(part[:1].upper() + part[1:] for part in name.split('_') if part)


# The document types whose values are written in C++ as a quoted literal.
TEXT_TYPES = ('String', 'WideString')


def cpp_value(value, type_name=None, iface=None, where=''):
    """A value of design.json as the C++ text an argument is written with.

    A design says what a value is, not how C++ spells it, so text given for a String
    parameter is quoted here and a field of an enumeration is qualified with its type.
    Text that already carries its own quotes stands as it is, and "raw:<c++>" or
    "expr:<c++>" passes anything through verbatim.
    """
    if isinstance(value, bool):
        return 'true' if value else 'false'
    text = str(value)
    for prefix in ('raw:', 'expr:'):
        if text.startswith(prefix):
            return text[len(prefix):]
    if type_name in TEXT_TYPES and isinstance(value, str):
        if len(text) > 1 and text.startswith('"') and text.endswith('"'):
            return text
        return '"{}"'.format(text.replace('\\', '\\\\').replace('"', '\\"'))
    if iface is not None and type_name in iface.enum_fields:
        return enum_value(text, type_name, iface, where)
    return text


def enum_value(text, type_name, iface, where):
    """One field of an enumeration as C++, from the field name or a qualified one.

    A bare field name does not compile on its own and the compiler names the call
    site, not the design that wrote it, so a name that is no field of this type is
    refused here instead.
    """
    spelt = iface.cpp_type(type_name)[0]
    fields = iface.enum_fields[type_name]
    given = text.rsplit('::', 1)[-1]
    if given not in fields:
        fail('{} gives "{}" for a "{}", which has no such field. It has: {}'
             .format(where or 'a step', text, type_name, ', '.join(fields) or 'none'))
    if '::' in text and not text.endswith('{}::{}'.format(spelt, given)):
        fail('{} gives "{}" for a "{}", which C++ spells "{}::{}"'
             .format(where or 'a step', text, type_name, spelt, given))
    return '{}::{}'.format(spelt, given)


def driver_of(specs, iface):
    """The driver settings the specs declare for this service, every key present."""
    import gen_docs
    settings = dict(gen_docs.DRIVER_DEFAULTS)
    for path in specs:
        spec, _skipped = gen_docs.load_spec(path)
        for entry in spec.get('interfaces') or []:
            if isinstance(entry, dict) and entry.get('name') == iface.name:
                settings = gen_docs.driver_of(entry)
    return settings


def snake(name):
    """A declared name as the snake_case a generated helper carries."""
    return re.sub(r'(?<=[a-z0-9])([A-Z])', r'_\1', name).lower()


def timers_of(specs, iface):
    """The timers the specs give this service's provider, in declaration order.

    Each is a dict: name, member, slot, start and stop (helper names), timeout (ms),
    count (C++ text) and autostart.
    """
    import gen_docs
    declared = []
    for path in specs:
        spec, _skipped = gen_docs.load_spec(path)
        for entry in spec.get('interfaces') or []:
            if isinstance(entry, dict) and entry.get('name') == iface.name:
                declared = entry.get('timers') or []
    timers = []
    for entry in declared:
        name, repeat = entry['name'], entry.get('repeat', 1)
        timers.append({'name': name, 'member': 'm' + pascal(name),
                       'slot': 'timer_' + snake(name),
                       'start': 'start_' + snake(name), 'stop': 'stop_' + snake(name),
                       'timeout': entry['timeout'],
                       'count': 'areg::TimerBase::CONTINUOUSLY' if repeat == 0
                       else str(repeat),
                       'autostart': bool(entry.get('start')),
                       'description': entry.get('description') or ''})
    return timers


def awaitable(iface):
    """The names a step may await, as the document spells them, by kind."""
    said, shown = [], set()
    for kind, entries in (('response', iface.responses), ('broadcast', iface.broadcasts),
                          ('attribute', iface.attributes)):
        names = [name for name, _ in entries]
        if names:
            said.append('{} {}'.format(kind, ', '.join(names)))
            shown.update(names)
    answered = [name for name in sorted(iface.response_of) if name not in shown]
    if answered:
        said.append('the answer of request {}'.format(', '.join(answered)))
    return '; '.join(said) if said else 'nothing: it declares none'


def steps_of(specs, iface):
    """The steps the specs declare for this service, resolved against its document.

    Each is a dict: name, enum, send (a request or None), args (C++ text in parameter
    order), awaits ((kind, name) or None) and wait (milliseconds, 0 for none).
    """
    import gen_docs
    declared = []
    for path in specs:
        spec, _skipped = gen_docs.load_spec(path)
        for entry in spec.get('interfaces') or []:
            if isinstance(entry, dict) and entry.get('name') == iface.name:
                declared = entry.get('steps') or []
    requests = dict(iface.requests)
    kinds = {}
    for kind, entries in (('update', iface.attributes), ('broadcast', iface.broadcasts),
                          ('response', iface.responses)):
        for name, _ in entries:
            kinds[name] = kind
    steps = []
    for step in declared:
        name, send = step.get('name'), step.get('send')
        where = 'step "{}"'.format(name)
        if send is not None and send not in requests:
            fail('{} sends "{}", which {} does not declare as a request. It sends one '
                 'of: {}'.format(where, send, iface.name,
                                 ', '.join(requests) or 'nothing: it declares none'))
        args = step.get('args') or {}
        values = []
        for param, param_type in requests.get(send, []):
            if param not in args:
                fail('{} gives no value for "{}" of request "{}"'.format(where, param, send))
            values.append(cpp_value(args[param], param_type, iface, where))
        target, wait = step.get('await'), step.get('wait') or 0
        if target is None and send is not None and not wait:
            target = iface.response_of.get(send)
        elif target in iface.response_of:
            target = iface.response_of[target]
        if target is not None and target not in kinds:
            fail('{} awaits "{}", which is no response, broadcast or attribute of {}. '
                 'It awaits one of: {}'
                 .format(where, target, iface.name, awaitable(iface)))
        steps.append({'name': name, 'enum': pascal(name), 'send': send,
                      'call': iface.spell('request', send) if send is not None else None,
                      'args': values,
                      'awaits': (kinds[target], target) if target is not None else None,
                      'wait': wait})
    return steps


STEP_CHECK = {'response': 'check this answer', 'broadcast': 'check this broadcast',
              'update': 'check the new value'}


def step_said(step):
    """What one step sent and what it waits for, in plain words."""
    said = []
    if step['send']:
        said.append('sent {}({})'.format(step['call'], ', '.join(step['args'])))
    if step['awaits']:
        said.append('awaits {} {}'.format(*step['awaits']))
    elif step['wait']:
        said.append('waits {} ms'.format(step['wait']))
    return ' and '.join(said) if said else 'waits for nothing'


def step_detail(step):
    """What one step sent and what it waits for, as a C++ string literal's contents.

    A stall names the step it stopped on. The step alone does not say what the run
    was waiting for, and reading that out of the design costs a request at the moment
    the answer is needed most.
    """
    return step_said(step).replace('\\', '\\\\').replace('"', '\\"')


def step_dispatch(steps, kind, name, indent):
    """The check of every step waiting on this handler, then the end of that step."""
    waiting = [step for step in steps if step['awaits'] == (kind, name)]
    if not waiting:
        return []
    pad = ' ' * indent
    lines = ['' if kind != 'response' else None,
             pad + 'mHeld = false;', pad + 'mJumped = false;',
             pad + 'switch (mStep)', pad + '{']
    lines = [line for line in lines if line is not None]
    # The check is braced: a case body that declares a local and is not braced
    # makes the compiler reject every case label after it. StepEnd is the first
    # thing in that scope, so the step ends however the check leaves it.
    for step in waiting:
        lines += [pad + 'case Step::{}:'.format(step['enum']),
                  pad + '    {',
                  pad + '        StepEnd ending(*this);',
                  marker('step_' + step['name'], STEP_CHECK[kind], indent + 8),
                  pad + '    }',
                  pad + '    break;']
    # A message arriving on a step with no case for it is not an error: most steps
    # ignore most messages. It is remembered rather than reported, because the step
    # that did want it waits for ever and the stall report is where that is answered.
    lines += [pad + 'default:',
              pad + '    dropped("{} {}");'.format(kind, name),
              pad + '    break;', pad + '}']
    return lines


def driver_lines(steps, holds, cls):
    """The helpers that run the steps: begin one, end one, stay in one, jump to one."""
    lines = ['    //! Ends the current step when a check body is left, by falling off',
             '    //! its end, by return, or by break. stay(), go_to() and fail() all',
             '    //! take effect in complete(), so every path reaches it.',
             '    struct StepEnd',
             '    {',
             '        explicit StepEnd({} & owner)'.format(cls),
             '            : mOwner(owner) {}',
             '        ~StepEnd(void)',
             '        {',
             '            mOwner.mRan = true;',
             '            mOwner.complete();',
             '        }',
             '        StepEnd(void) = delete;',
             '        AREG_NOCOPY_NOMOVE(StepEnd);',
             '    private:',
             '        {} & mOwner;'.format(cls),
             '    };',
             '',
             '    //! Begins a step: sends its request or starts its wait. A step that',
             '    //! waits for nothing ends at once.',
             '    void begin(Step step)',
             '    {',
             '        mStep = step;',
             '        mNext = step;',
             '        mJumped = false;',
             '        mHeld = false;',
             '        mRan = false;',
             '        progressed();',
             '        switch (step)',
             '        {']
    for step in steps:
        lines += ['        case Step::{}:'.format(step['enum']),
                  '            std::cout << "step {}" << std::endl;'.format(step['name'])]
        if step['send']:
            lines.append('            {}({});'.format(step['call'],
                                                  ', '.join(step['args'])))
        if step['wait']:
            lines += ['            mHold.stop_timer();',
                      '            mHold.start_timer({}, static_cast<areg::DispatcherThread &>'
                      '(master_thread()),'.format(step['wait']),
                      '                              areg::TimerBase::ONE_TIME);']
        elif step['awaits'] is None:
            lines.append('            complete();')
        lines.append('            break;')
    lines += ['        case Step::Done:',
              '            mDeadline.stop_timer();',
              '            mPace.stop_timer();',
              '            quit_with(0);',
              '            break;',
              '        default:',
              '            break;',
              '        }',
              '    }',
              '',
              '    //! Ends the current step and begins the next, unless its check failed',
              '    //! the run, called stay() or called go_to().',
              '    void complete()',
              '    {',
              '        if (is_quitting() || mHeld)',
              '        {',
              '            mHeld = false;',
              '            return;',
              '        }',
              '',
              '        begin(mJumped ? mNext : static_cast<Step>(static_cast<uint32_t>(mStep) + 1));',
              '    }',
              '',
              '    //! Keeps the current step for the next answer, broadcast or update.',
              '    void stay()',
              '    {   mHeld = true; }',
              '',
              '    //! Makes this the next step instead of the one listed after the current.',
              '    void go_to(Step step)',
              '    {   mNext = step; mJumped = true; }',
              '',
              '    Step  mStep{ Step::Start };   //!< The step the scenario is on.',
              '    Step  mNext{ Step::Start };   //!< The step go_to() chose.',
              '    bool  mJumped{ false };       //!< True once go_to() chose the next step.',
              '    bool  mHeld{ false };         //!< True once stay() kept the step.',
              '    bool  mRan{ false };          //!< True once a check of this step ran.',
              '']
    if holds:
        lines += ['    areg::Timer  mHold;   //!< Ends a step that waits for a time.', '']
    return lines


def consumer_class(iface, cls, steps=(), driver=None):
    """The consumer component, subscribed and handling everything it subscribed to.

    With steps it also carries the driver that runs them: the requests, their order and
    the exit code are generated, and the check a step makes is a marker. The deadlines
    the driver gives up after come from the spec, so no marker asks for one.
    """
    import gen_docs
    driver = dict(gen_docs.DRIVER_DEFAULTS) if driver is None else driver
    stepped = steps_scenario(iface) or bool(steps)
    holds = any(step['wait'] for step in steps)
    pad = ' ' * (len(cls) + 13)
    lines = ['class {} final : public    areg::Component'.format(cls),
             '{}, protected {}ConsumerBase'.format(pad, iface.name),
             '{}, private   areg::TimerConsumer'.format(pad)]
    lines += ['{',
              'public:']
    if steps:
        lines += ['    //! The steps of the scenario, in the order design.json lists them.',
                  '    enum class Step : uint32_t',
                  '    {',
                  '        Start,']
        lines += ['        {},'.format(step['enum']) for step in steps]
        lines += ['        Done',
                  '    };',
                  '']
    lines += ['    {}(const areg::ComponentEntry & entry, areg::ComponentThread & owner)'.format(cls),
              '        : areg::Component(entry, owner)',
              '        , {}ConsumerBase(entry.mDependencyServices[0].mRoleName, owner)'.format(iface.name),
              '        , areg::TimerConsumer()',
              '        , mDeadline(static_cast<areg::TimerConsumer &>(self()), "Deadline")']
    if stepped:
        lines.append('        , mPace(static_cast<areg::TimerConsumer &>(self()), "Pace")')
    if holds:
        lines.append('        , mHold(static_cast<areg::TimerConsumer &>(self()), "Hold")')
    lines += ['    { }',
              '',
              'protected:',
              '    void startup_component(areg::ComponentThread & thread) final',
              '    {',
              '        areg::Component::startup_component(thread);',
              '        arm_deadline(cConnectSeconds);',
              '    }',
              '',
              '    bool service_connected(areg::ServiceConnectionState status, areg::ProxyBase & proxy) final',
              '    {',
              '        bool result{ false };',
              '        if ({}ConsumerBase::service_connected(status, proxy))'.format(iface.name),
              '        {',
              '            result = true;',
              '            if (areg::is_service_connected(status))',
              '            {',
              '                mDeadline.stop_timer();',
              '                mConnected = true;']
    if iface.attributes or iface.broadcasts:
        lines.append('                // Subscriptions are made here, and again after '
                     'every reconnection.')
        for attr_name, _ in iface.attributes:
            lines.append('                {}(true);'.format(
                iface.spell('attribute', attr_name, 'notify')))
        for name, _ in iface.broadcasts:
            lines.append('                {}(true);'.format(
                iface.spell('broadcast', name, 'notify')))
    # A generated application that waits forever is not one that runs as written.
    # The first request that carries a response completes a round trip, and the
    # response handler below quits, so the program starts and ends on its own.
    if steps:
        lines += ['',
                  '                // The stall watchdog ticks from here; the steps begin once.',
                  '                mPace.stop_timer();',
                  '                mPace.start_timer({}, static_cast<areg::DispatcherThread &>'
                  '(master_thread()),'.format(STEP_INTERVAL_MS),
                  '                                  areg::TimerBase::CONTINUOUSLY);',
                  '                if (mStep == Step::Start)',
                  '                {',
                  '                    begin(Step::{});'.format(steps[0]['enum']),
                  '                }']
    else:
        answered_first = next((entry for entry in iface.requests
                               if entry[0] in set(n for n, _ in iface.responses)), None)
        first_request = answered_first or (iface.requests[0] if iface.requests else None)
        lines.append(marker('first_request',
                            'the first request of the scenario', 16))
        if first_request:
            name, params = first_request
            args = ', '.join(default_expr(iface, type_name) for _, type_name in params)
            lines.append(placeholder('                {}({});'
                                     .format(iface.spell('request', name), args)))
        if stepped:
            lines += ['',
                      '                // One step of the scenario per tick.',
                      '                mPace.stop_timer();',
                      '                mPace.start_timer({}, static_cast<areg::DispatcherThread &>'
                      '(master_thread()),'.format(STEP_INTERVAL_MS),
                      '                                  areg::TimerBase::CONTINUOUSLY);']
        # Nothing answers the first request, and no attribute update arrives to end on.
        if not answered_first and (iface.requests or not iface.attributes):
            lines.append('                // placeholder(you): the scenario ends here until '
                         'a later step replaces it.')
            if stepped:
                lines.append(placeholder('                mPace.stop_timer();'))
            lines.append(placeholder('                quit_with(0);'))
    lines += ['            }',
              '            else if ((status == areg::ServiceConnectionState::Disconnected) ||',
              '                     (status == areg::ServiceConnectionState::ConnectionLost))',
              '            {',
              '                // The provider went away. The framework reconnects and',
              '                // calls this again; the reconnect deadline is the exit.',
              '                if (is_quitting() == false)',
              '                {',
              marker('peer_lost',
                     'what losing the provider means to this scenario', 20),
              '                    arm_deadline(cReconnectSeconds);',
              '                }',
              '            }',
              '            else if ((status == areg::ServiceConnectionState::Rejected) ||',
              '                     (status == areg::ServiceConnectionState::Shutdown))',
              '            {',
              '                // Terminal states. The framework does not reconnect',
              '                // from these.',
              marker('service_refused',
                     'what a refused or shut-down service means here', 16),
              '                mDeadline.stop_timer();',
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

    # One process_timer serves every timer of this component, so the timer is told
    # apart by address. A name compared to a literal is a different timer the moment
    # one is renamed.
    lines += ['    void process_timer(areg::Timer & timer) final',
              '    {',
              '        if (&timer == &mDeadline)',
              '        {',
              '            fail(mConnected ? "the provider did not come back within '
              'the reconnect deadline"',
              '                            : "no provider connected within the '
              'connect deadline");',
              '            return;',
              '        }',
              '']
    if holds:
        lines += ['        if (&timer == &mHold)',
                  '        {',
                  '            complete();',
                  '            return;',
                  '        }',
                  '']
    if stepped:
        # The watchdog counts a pace tick before the author's code runs, so a return
        # in that code never stops it counting.
        lines += ['        if ((&timer == &mPace) && (cStallTicks != 0) && (++mIdleTicks >= cStallTicks))',
                  '        {',
                  '            {};'.format('stalled()' if steps
                                           else 'fail("the scenario stopped making '
                                                'progress")'),
                  '            return;',
                  '        }']
        if not steps:
            lines += ['', marker('next_step', 'the next request of the scenario')]
    lines += ['    }',
              '']

    first = True
    for name, params in iface.responses:
        lines.append('    void {}({}) final'.format(iface.spell('response', name),
                                                    iface.generated_params('response', name)))
        lines.append('    {')
        # A step that awaits this answer checks it, so a second marker would be empty.
        if not any(step['awaits'] == ('response', name) for step in steps):
            lines.append(marker(iface.spell('response', name),
                                'what this answer means for the scenario'))
        if first and not steps:
            lines.append('        // placeholder(you): the scenario ends here until a '
                         'later step replaces it.')
            if stepped:
                lines.append(placeholder('        mPace.stop_timer();'))
            lines.append(placeholder('        quit_with(0);'))
            first = False
        lines += step_dispatch(steps, 'response', name, 8)
        lines.append('    }')
        lines.append('')

    for name, _ in iface.requests:
        lines.append('    void {}({}) final'.format(
            iface.spell('request', name, 'failed'),
            iface.generated_params('request', name, 'failed').strip()))
        lines += ['    {',
                  '        std::cerr << "request {} failed, reason " '
                  '<< static_cast<int>(reason) << std::endl;'.format(name)]
        if stepped:
            lines.append('        mPace.stop_timer();')
        lines += ['        quit_with(1);',
                  '    }',
                  '']

    for name, params in iface.broadcasts:
        lines.append('    void {}({}) final'.format(iface.spell('broadcast', name),
                                                    iface.generated_params('broadcast', name)))
        lines += ['    {',
                  marker(iface.spell('broadcast', name),
                         'what this broadcast means in every step' if steps else
                         'what this broadcast means for the scenario')]
        lines += step_dispatch(steps, 'broadcast', name, 8)
        lines += ['    }',
                  '']

    for attr_name, type_name in iface.attributes:
        # The base applies the parameter rule, so anything but a primitive arrives by
        # const reference. An override that disagrees is refused as not virtual.
        lines.append('    void {}({}) final'
                     .format(iface.spell('attribute', attr_name, 'on_update'),
                             iface.generated_params('attribute', attr_name,
                                                    'on_update').strip()))
        lines += ['    {',
                  '        if (state == areg::DataState::DataIsOK)',
                  '        {',
                  marker('update_' + iface.spell('attribute', attr_name, 'get'),
                         'the new value is ready to use', 12)]
        # With no request, the first update the provider's initial value sends ends it.
        if not steps and not iface.requests and attr_name == iface.attributes[0][0]:
            lines.append(placeholder('            quit_with(0);'))
        lines += step_dispatch(steps, 'update', attr_name, 12)
        lines += ['        }',
                  '    }',
                  '']
    lines += ['private:',
              '    inline {} & self()'.format(cls),
              '    {   return (*this); }',
              '']
    if steps:
        lines += ['    //! The worksheet section holding the check of the step the',
                  '    //! scenario is on.',
                  '    const char * step_slot()',
                  '    {',
                  '        switch (mStep)',
                  '        {']
        lines += ['        case Step::{}:  return "step_{}";'.format(step['enum'],
                                                                    step['name'])
                  for step in steps]
        lines += ['        default:  return "no step";',
                  '        }',
                  '    }',
                  '']
        lines += ['    //! What the step the scenario is on sent, and what it waits for.',
                  '    const char * step_detail()',
                  '    {',
                  '        switch (mStep)',
                  '        {']
        lines += ['        case Step::{}:  return "{}";'.format(step['enum'],
                                                                step_detail(step))
                  for step in steps]
        lines += ['        default:  return "waits for nothing";',
                  '        }',
                  '    }',
                  '',
                  '    //! Ends the scenario when no step has advanced, naming what the',
                  '    //! step waits for and every message an earlier step discarded.',
                  '    void stalled()',
                  '    {',
                  '        fail("the scenario stopped making progress");',
                  '        std::cerr << "  " << step_slot() << " " << step_detail()',
                  '                  << (mRan ? ". Its check ran and kept the step."',
                  '                           : ". Nothing arrived.") << std::endl;',
                  '        for (uint32_t kept = 0; kept < mDroppedKept; ++ kept)',
                  '        {',
                  '            std::cerr << "  dropped: " << mDroppedWhat[kept]',
                  '                      << " arrived on " << mDroppedStep[kept]',
                  '                      << ", which has no check for it."',
                  '                      << std::endl;',
                  '        }',
                  '        if (mDroppedCount > mDroppedKept)',
                  '        {',
                  '            std::cerr << "  dropped: and "',
                  '                      << (mDroppedCount - mDroppedKept)',
                  '                      << " more." << std::endl;',
                  '        }',
                  '    }',
                  '',
                  '    //! Remembers a message that arrived on a step with no check',
                  '    //! for it. The stall report names them.',
                  '    void dropped(const char * what)',
                  '    {',
                  '        if (mDroppedKept < cDroppedMost)',
                  '        {',
                  '            mDroppedWhat[mDroppedKept] = what;',
                  '            mDroppedStep[mDroppedKept] = step_slot();',
                  '            ++ mDroppedKept;',
                  '        }',
                  '        ++ mDroppedCount;',
                  '    }',
                  '',
                  '    //! How many discarded messages the stall report names.',
                  '    static constexpr uint32_t cDroppedMost{ 4 };',
                  '    const char * mDroppedWhat[cDroppedMost]{};   //!< What each was.',
                  '    const char * mDroppedStep[cDroppedMost]{};   //!< Where each was.',
                  '    uint32_t     mDroppedKept{ 0 };    //!< How many are remembered.',
                  '    uint32_t     mDroppedCount{ 0 };   //!< How many there were.',
                  '']
    lines += ['    //! Ends the scenario as a failure, naming what went wrong and the',
              '    //! step the scenario was on.',
              '    void fail(const char * why)',
              '    {']
    if steps:
        lines += ['        std::cerr << "FAIL [" << step_slot() << "]: " << why',
                  '                  << std::endl;']
    else:
        lines.append('        std::cerr << "FAIL: " << why << std::endl;')
    lines += ['        mDeadline.stop_timer();']
    if stepped:
        lines.append('        mPace.stop_timer();')
    if holds:
        lines.append('        mHold.stop_timer();')
    lines += ['        quit_with(1);',
              '    }',
              '',
              '    //! Starts the deadline timer for this many seconds. 0 stops it and',
              '    //! waits for ever.',
              '    void arm_deadline(uint32_t seconds)',
              '    {',
              '        mDeadline.stop_timer();',
              '        if (seconds != 0)',
              '        {',
              '            mDeadline.start_timer(seconds * 1000,',
              '                                  static_cast<areg::DispatcherThread &>'
              '(master_thread()),',
              '                                  areg::TimerBase::ONE_TIME);',
              '        }',
              '    }',
              '',
              '    areg::Timer  mDeadline;   //!< Ends the run when no provider is there.',
              '    bool         mConnected{ false };   //!< True once the service '
              'has connected.',
              '',
              '    //! Seconds to wait for the provider to appear. 0 waits for ever.',
              '    static constexpr uint32_t cConnectSeconds{{ {} }};'
              .format(driver['connect_seconds']),
              '    //! Seconds to wait for it to come back. 0 waits for ever.',
              '    static constexpr uint32_t cReconnectSeconds{{ {} }};'
              .format(driver['reconnect_seconds']),
              '']
    if stepped:
        lines += [
                  '    //! Restarts the stall watchdog. Call it wherever the scenario advances.',
                  '    void progressed()',
                  '    {   mIdleTicks = 0; }',
                  '',
                  '    areg::Timer  mPace;   //!< Spaces the requests of the scenario.',
                  '',
                  '    //! Ticks of no progress that end the run, one tick a second.',
                  '    //! 0 leaves the watchdog off.',
                  '    static constexpr uint32_t cStallTicks{{ {} }};'
                  .format(driver['stall_ticks']),
                  '    uint32_t                  mIdleTicks{ 0 };',
                  '']
    lines += driver_lines(steps, holds, cls) if steps else []
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
                    state_hint='the members and helpers your rules need, defined here, '
                               'or one // line saying none is needed'):
    """The .hpp and the .cpp of one component, named after its class."""
    declaration, definitions = split_class(cls, class_lines)
    private = declaration.index('private:')
    declaration.insert(private + 1, marker(state_slot, 'private: ' + state_hint, 4))
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


def app_files(iface, mode, include_root, machine=None, steps=(), driver=None, timers=()):
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

    provider_lines = provider_class(iface, provider_cls, machine, timers)
    consumer_lines = consumer_class(iface, consumer_cls, steps, driver)
    produced = [(PROVIDER_DIR[mode] + name, text) for name, text in component_files(
        provider_cls, 'Provider of the {} service.'.format(iface.name),
        class_includes(iface, machine) + timer_includes(provider_lines) + ['']
        + provider_base,
        provider_lines, 'provider_state')]
    produced += [(CONSUMER_DIR[mode] + name, text) for name, text in component_files(
        consumer_cls, 'Consumer of the {} service.'.format(iface.name),
        class_includes(iface) + timer_includes(consumer_lines) + [''] + consumer_base,
        consumer_lines, 'consumer_state',
        QUIT_DECLARATION,
        'the members and helpers your checks need, defined here, or one // line saying '
        'none is needed; the step the scenario is on is mStep already' if steps else
        'the members and helpers your rules need, defined here, or one // line saying '
        'none is needed, and the one saying what step the scenario is on'
        if steps_scenario(iface) else
        'the members and helpers your rules need, defined here, or one // line saying '
        'none is needed')]

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
SCENARIO_TODO = ('TODO(you) {}: one regular expression per line, each a line this '
                 'process prints that proves a requirement')

# The scenario that proves the generated console quit loop. It expects no output, so
# it holds no hole and passes as written.
QUIT_SCENARIO = 'quit'

# The scenario that proves the generated reconnect deadline: the provider is killed
# while it serves the consumer, and the consumer exits 1. Its one hole is the line
# that marks "while it serves", which only the author of the bodies knows.
PEER_LOST_SCENARIO = 'peer-lost'

# A wait at least this long holds the lead still long enough for the runner, which
# polls every 50 ms, to kill the provider before the step after it begins.
HOLD_MS_MIN = 250


def held_step(steps):
    """The wait step a peer loss can be timed against, or None.

    It has a step after it, so the provider is still needed when it is killed, and a
    later one is preferred over the first, so the provider has already served.
    """
    held = [step for step in steps[:-1] if step['wait'] >= HOLD_MS_MIN]
    later = [step for step in held if step is not steps[0]]
    return (later or held or [None])[0]
STOP_TODO = 'TODO(you) {}: a line the lead prints while the provider serves it'
STOP_HINT = ('one line "{lead}" prints in scenario "{scenario}", matched against the '
             'output of "{lead}" only. "{target}" is killed when it appears, and the '
             'kill lands about 50 ms later: a send that is answered at once finishes '
             'sooner than that. Pick a line printed after the first answer and before '
             'something that takes time. A design with steps holds the lead still by '
             'adding a {{"name": "...", "wait": 300}} step right after the step that '
             'prints it; one without steps picks a line the lead prints while it is '
             'still being served. A regular expression, one line.')


def stop_slot(scenario):
    """The name of a scenario's stop trigger, as a marker."""
    return 'stop_{}'.format(re.sub(r'\W+', '_', scenario).strip('_'))


def proc_label(spec):
    return spec.get('name') or spec['binary']


def expect_slot(scenario, spec):
    """The name of one process's expectations in one scenario, as a marker.

    Named, so the worksheet carries these holes beside the code ones and one command
    fills both. The pair is unique: a scenario names each process once.
    """
    return 'expect_{}_{}'.format(re.sub(r'\W+', '_', scenario).strip('_'),
                                 re.sub(r'\W+', '_', proc_label(spec)).strip('_'))


def update_scenarios(path, mode, iface, steps=(), reconnect=0):
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

    # A consumer that loses its provider exits 1 through the generated reconnect
    # deadline. With no deadline there is no exit to check.
    lost_written = False
    if mode == 'ipc' and len(procs) > 1 and reconnect \
            and not any(s.get('name') == PEER_LOST_SCENARIO or 'stop' in s
                        for s in scenarios):
        lead = procs[-1]
        held = held_step(list(steps))
        trigger = '^step {}$'.format(re.escape(held['name'])) if held \
            else STOP_TODO.format(stop_slot(PEER_LOST_SCENARIO))
        scenarios.append({'name': PEER_LOST_SCENARIO,
                          'timeout': scenarios[0]['timeout'] + reconnect,
                          'router': scenarios[0].get('router', True),
                          'stop': {'proc': proc_label(procs[0]),
                                   'after': trigger,
                                   'signal': 'kill'},
                          'procs': [{'binary': spec['binary'], 'name': proc_label(spec)}
                                    for spec in procs[:-1]] +
                                   [{'binary': lead['binary'], 'name': proc_label(lead),
                                     'lead': True, 'exit': 1}]})
        lost_written = held['name'] if held else True

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
    if lost_written:
        print('  A "{}" scenario was added: {} is killed while it serves, and {} has to'
              .format(PEER_LOST_SCENARIO, proc_label(procs[0]), proc_label(procs[-1])))
        if lost_written is True:
            print('  exit 1 within reconnect_seconds. Its one hole, {}, is the line that'
                  .format(stop_slot(PEER_LOST_SCENARIO)))
            print('  starts the loss, and it is a section of the worksheet.')
        else:
            print('  exit 1 within reconnect_seconds. The loss starts with step "{}",'
                  .format(lost_written))
            print('  whose wait holds the consumer still, and it needs nothing from you.')


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
                                path=BODIES))
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
    parser.add_argument('--spec', action='append', default=[],
                        help='with --app: a design.json. The "steps" it declares for '
                             'this service become the consumer\'s driver')
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
        steps = steps_of(args.spec, iface)
        produced = app_files(iface, args.mode, include_root, machine, steps,
                             driver_of(args.spec, iface), timers_of(args.spec, iface))
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
        update_scenarios(args.scenarios, args.mode, iface, steps,
                         driver_of(args.spec, iface)['reconnect_seconds'])
        first = not os.path.isfile(WORKSHEET)
        written = write_worksheet(retained, args.out, iface, args.doc,
                                  machine, args.machine, args.scenarios, steps)
        print_todos(retained, args.out, written,
                    len(scenario_holes(args.scenarios)), args.scenarios, first)
        if first:
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
