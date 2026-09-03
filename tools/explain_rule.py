#!/usr/bin/env python3
# ===========================================================================
# Explains a validation rule number reported by the code generator or by Lusan.
#
#   python3 tools/explain_rule.py 27
#   python3 tools/explain_rule.py 127 4 --document fsml
#   python3 tools/explain_rule.py --list --document siml
#   python3 tools/explain_rule.py --search "resolves to nothing"
#
# A finding carries the bare number for an error, the number plus 100 for a
# warning and plus 200 for information, so 4, 104 and 204 are unrelated rules.
# The number is resolved in its band automatically.
#
# The generator does not always print the number, so --search goes the other way:
# give it words from the message and it names the rule.
#
# Exit code 0 when every number was explained, 1 otherwise.
# ===========================================================================
import argparse
import math
import os
import re
import sys
import textwrap
import xml.etree.ElementTree as ET

RULES = os.path.join(os.path.dirname(os.path.abspath(__file__)), 'schema', 'rules.xml')
BANDS = [(0, 'error'), (100, 'warning'), (200, 'information')]

# What to change, one rule at a time. The registry says what is wrong; this says
# what to do about it, which is the half an agent needs and cannot infer.
#
# It lives here and not in schema/rules.xml because that file is mirrored byte for
# byte between this repository and the editor, and a corrective action is advice for
# whoever writes documents from these pages, not part of the shared registry.
FIXES = {
 'RULE_START_STATE':
   'Give the level exactly one state with Kind="Start". If it already has one, a '
   'second state carries the same Kind: change it to Kind="Normal".',
 'RULE_DUPLICATE_ID':
   'Every ID in the document comes from one counter and is never reused. Give the '
   'later element the next free number, then repoint anything that referenced it.',
 'RULE_STATE_NAME':
   'Rename one of the two states. State names are unique across the whole document, '
   'not only inside their level: the generator flattens every level into one C++ '
   'enumeration, so a substate of one region collides with a substate of another. '
   'Prefixing the name with its region, "Idle" as "PumpIdle" and "ValveIdle", is the '
   'rename that keeps both readable.',
 'RULE_DUPLICATE_NAME':
   'Rename the later declaration. A stimulus name is shared by triggers, events and '
   'timers, so a trigger may not carry the name of an event or a timer.',
 'RULE_UNREFERENCED':
   'Either use the declaration or delete it. Generation continues either way; the '
   'note stands so a leftover from an edit is not mistaken for something in use.',
 'RULE_MISSING_VERSION':
   'Add Version to Overview, as MAJOR.MINOR.PATCH. The version reaches the generated '
   'code and tells a consumer which contract it was built against.',
 'RULE_INVALID_IDENTIFIER':
   'Rewrite the name as a C++ identifier: a letter or an underscore first, then '
   'letters, digits or underscores. No spaces, dots or dashes.',
 'RULE_UNRESOLVED_TYPE':
   'Declare the type, or correct its spelling. A type from an included document is '
   'written Space::Type, where Space is that document\'s Overview name. A C++ type '
   'you already own is declared once with Type="Imported".',
 'RULE_TARGET_SIBLING':
   'A transition ends on a state of its own level. To leave a composite state, draw '
   'the transition from the composite itself, not from a state inside it.',
 'RULE_FINAL_STATE':
   'A Final state ends its level: remove its outgoing transitions and its substates. '
   'To continue afterwards, transition out of the composite that owns it.',
 'RULE_START_SUBSTATES':
   'A Start is a pseudo-state and holds nothing: it names where the level begins and '
   'is left in the same step it is entered, so anything nested inside it can never '
   'run. Add a Kind="Normal" state, move the substates into it, and let the Start\'s '
   'outgoing transition target it.',
 'RULE_ARGUMENT_MAPPING':
   'Map every parameter of the called element, once, in the order it declares them, '
   'and remove any argument that maps to nothing. The generator writes a direct call, '
   'so a missing, doubled or reordered mapping is not something it can repair: read '
   'the parameter list of the target and make the argument list match it position for '
   'position.',
 'RULE_NESTED_START':
   'The named submachine level needs its own Kind="Start" state. Every level with '
   'substates has one, not only the root.',
 'RULE_SOURCE_SCOPE':
   'The value named is not visible where it is used. Use a parameter of this '
   'stimulus, an attribute of the machine, or a constant -- not a parameter of '
   'another trigger or a value of another level.',
 'RULE_ARGUMENT_TYPE':
   'Make the two types match, or introduce a conversion of your own. A narrowing '
   'conversion is allowed and generates an explicit cast; an unrelated type is not.',
 'RULE_COMPARE_OPERAND':
   'Compare scalars. A structure or a container has no ordering, and bool, String '
   'and enumerations answer only to == and !=.',
 'RULE_MISSING_DESCRIPTION':
   'Add a Description to the declaration so the generated element carries a comment. '
   'Advisory: generation succeeds without it.',
 'RULE_BAD_LITERAL':
   'Write the value the way its declared type reads: a number for a numeric type, '
   'true or false for bool, and an enumerator this enumeration declares.',
 'RULE_BOOLEAN_OPERAND':
   'A predicate tested on its own is bool, and this operand is not: C++ would convert '
   'it silently and the guard would then be true for every non-zero value. Compare it '
   'with something ("Level > MaxLevel" rather than "Level"), or call a condition whose '
   'declared type is bool.',
 'RULE_ATTRIBUTE_TYPE':
   'Set the attribute from a source of its own type. A source that only narrows is '
   'accepted with a warning and generates an explicit cast.',
 'RULE_STATE_SHAPE':
   'The state carries a property its kind does not have. History and OnFinal belong '
   'to a composite state, a Submachine belongs to neither a Start nor a Final, and a '
   'state is either painted or imported, never both.',
 'RULE_BROKEN_IMPORT':
   'Correct the path in the include. It is spelled from the workspace root, the file '
   'must exist and be readable, and a document may not include itself through a '
   'chain. A data type document may not include another one.',
 'RULE_CONDITION_BODY':
   'Only an Embedded condition carries a body, and its body may not be empty. A '
   'handler and a plain method carry none, and neither carries a Return.',
 'RULE_PARAMETERIZED_COND':
   'A parameterized condition is a left operand only. Give it a right operand to '
   'compare with, or use a plain value source where a value is wanted.',
 'RULE_IMPORT_MAJOR':
   'The imported file is a different major version from the one pinned. Update the '
   'pinned version after checking the import still means the same thing, or point '
   'the include at the version it was written against.',
 'RULE_SOURCE_KIND':
   'The row names a kind of source the format does not define here. Use one the '
   'format allows in this position: a parameter, an attribute, a constant or a '
   'literal.',
 'RULE_SOURCE_EMPTY':
   'Fill the value source in, or delete the row. An empty source generates nothing, so '
   'the assignment it belongs to silently does not happen: nothing fails at build time '
   'and the target keeps whatever it held. It usually marks an edit left half done, so '
   'check the rest of that element before moving on.',
 'RULE_GUARD':
   'The finding is in the guard expression itself: fix the operand, the operator or '
   'the name the message points at. Every guard finding is filed under this rule.',
 'RULE_PSEUDO_START':
   'A Start carries no operations and its initial transition carries no stimulus. '
   'Exactly one transition leaves it, and it names a state of the same level.',
 'RULE_TRANSITION_KIND':
   'Match Kind to the transition: External needs a target, Internal has none and '
   'stays in the state, and Local stays inside the composite it starts in.',
 'RULE_HANDLER_NAME':
   'Two hosted machines generate one handler name, so one implementation would have to '
   'serve both and the second definition will not compile. The name is derived from the '
   'trigger and its parameter types together: rename the trigger in one of the two '
   'machines, or change a parameter type, and the names separate.',
 'RULE_ATTRIBUTE_STIMULUS':
   'Rename the trigger, or the attribute. The generator derives one function name '
   'from both, so the second declaration silently takes the first one over.',
 'RULE_RESERVED_PREFIX':
   'The generated member would land under a prefix the generator owns (request_, '
   'response_, broadcast_, on_, notify_on_, EVENT_). Rename the declaration.',
 'RULE_UNKNOWN_ELEMENT':
   'Remove the element or move it where the format places it. A tag the format does '
   'not know reaches no generated code and is dropped when the document is saved.',
 'RULE_PARAM_SHADOWS':
   'Rename the parameter: it carries the name of its own trigger or condition. '
   'Advisory, and generation succeeds.',
 'RULE_UNBOUND_RESPONSE':
   'Either name this response from a request, with Response="<name>", or delete it. '
   'A response nothing leads to is never sent.',
 'RULE_DEFAULT_ORDER':
   'Move the parameters carrying defaults to the end of the list, exactly as C++ '
   'requires: once one parameter has a default, every parameter after it must have one '
   'too. Either give the following parameters a default of their own, or reorder the '
   'list so the ones with defaults come last. The generated signature follows the '
   'document order literally.',
 'RULE_ACTION_PREFIX':
   'Rename the condition: its name already starts with the prefix the generated '
   'action method carries, so the two read as one. Advisory.',
 'RULE_UNRESOLVED_ELEMENT':
   'The reference names something the document does not declare. Declare it, or '
   'correct the spelling; the message names both the reference and its kind.',
 'RULE_NOT_A_HEADER':
   'A data type document includes C++ headers only. Move the shared types into this '
   'document, or have the host document include both.',
 'RULE_DUPLICATE_ENUM_VALUE':
   'Give the two enumerators different values. An enumerator with no value continues '
   'from the previous one, so an explicit value that repeats an implicit one is the '
   'usual cause.',
 'RULE_DEPRECATED':
   'The declaration is marked deprecated by its author. Move to what its '
   'DeprecateHint names; nothing is blocked.',
 'RULE_EMPTY_TYPE':
   'Add the fields or the enumerators. An empty declaration compiles, so the note '
   'stands only until the first member is added.',
 'RULE_EMPTY_DOCUMENT':
   'The document declares nothing, so the generator writes a header that nothing can '
   'use and every document including it gains no type. Add what it is for -- a data '
   'type, a method, an attribute -- or delete the file together with the Location row '
   'that includes it.',
 'RULE_FILE_NAME_MISMATCH':
   'Rename the file to the declared name, or the declared name to the file. The '
   'generated files follow the declared name, so nothing breaks either way.',
 'RULE_RESPONSE_LINK':
   'Point Response at a method this document declares with MethodType="Response". A '
   'request with no answer omits the attribute entirely.',
 'RULE_UNREACHABLE_STATE':
   'Nothing enters the state. Give it an incoming transition, make it the Start of '
   'its level, or delete it.',
 'RULE_DEAD_END_STATE':
   'The machine never leaves this state. Add an outgoing transition, or mark it '
   'Kind="Final" if it really is the end.',
 'RULE_SHADOWED_TRANSITION':
   'An earlier unconditional transition on the same stimulus always wins. Give the '
   'earlier one a guard, or reorder the two.',
 'RULE_ONE_SIDED_EVENT':
   'Either react to the event or stop sending it. An event with only one side is '
   'usually half of an edit.',
 'RULE_ONE_SIDED_TIMER':
   'Either react to the timer or stop starting it. A timer with only one side never '
   'has an effect.',
 'RULE_EMPTY_INTERNAL':
   'The internal transition changes nothing. Give it an operation or a condition, or '
   'delete it.',
 'RULE_CONSTANT_COMPARE':
   'Both operands are fixed at design time, so the result never changes. Compare '
   'against an attribute or a parameter, or drop the guard.',
 'RULE_UNUSED_HISTORY':
   'Nothing re-enters the state, so its remembered substate is never restored. '
   'Remove History, or add the transition that returns to the state.',
 'RULE_IMPORT_PATCH':
   'The imported file carries a different minor or patch version from the one '
   'pinned. Update the pin once the difference is understood.',
 'RULE_RETIRED_ELEMENT':
   'The format no longer defines this element here. The message names what replaced '
   'it; move the content there.',
 'RULE_TWO_RESPONSES':
   'A request has one answer. Keep one Response and give the other request its own, '
   'or make the second a broadcast.',
 'RULE_SHARED_RESPONSE':
   'One response answers several requests. Legal and sometimes meant; split it only '
   'if a consumer must tell the two answers apart.',
 'RULE_BAD_VALUE':
   'The element or the attribute is allowed here, but what it holds is not. Write a '
   'value of the kind the format names for it.',
 'RULE_UNKNOWN_ATTRIBUTE':
   'Remove the attribute. The format does not define it on this element, so nothing '
   'reads it and it is dropped when the document is saved.',
 'RULE_DROPPED_ELEMENT':
   'The block was kept while the document was open and is lost on save. Fix the '
   'fault reported beside it, then re-add the block.',
}


def load():
    """Reads the registry into a list of rules, in file order."""
    rules = []
    for node in ET.parse(RULES).getroot().findall('.//Rule'):
        rules.append({'number': int(node.get('Number')),
                      'name': node.get('Name') or '',
                      'bands': (node.get('Bands') or '').split(),
                      'documents': (node.get('Documents') or '').split(),
                      'section': node.get('Section') or '',
                      'summary': clean(node.findtext('Summary') or ''),
                      'fix': FIXES.get(node.get('Name') or '', '')})
    return rules


def clean(text):
    """Drops the markup the registry carries for the editor's help viewer."""
    text = text.replace('\\a ', '').replace('\\c ', '').replace('\\b ', '')
    return ' '.join(text.split())


# The generator's own wording for a rule, where it differs from the registry's.
# The two bodies of text are written independently, so a message may share no verb
# with the rule that produced it: "resolves to nothing" is the generator, "answers
# to nothing" is the registry. A phrase here is matched before the words are scored.
# Add an entry when a message is seen that the words alone do not resolve.
PHRASES = {
 'RULE_UNRESOLVED_TYPE':  ['a data type that resolves to nothing'],
 'RULE_RESPONSE_LINK':    ['a response that resolves to nothing'],
 'RULE_BAD_VALUE':        ['a value the format does not allow here'],
 'RULE_DUPLICATE_ID':     ['an element ID that more than one element claims'],
}

# Words that say nothing about which rule a message is about. The registry and the
# generator both write ordinary English, so a match on one of these is noise.
STOP = {'a', 'an', 'the', 'to', 'of', 'is', 'in', 'at', 'by', 'that', 'this', 'it',
        'as', 'and', 'or', 'not', 'no', 'be', 'been', 'was', 'were', 'are', 'has',
        'have', 'had', 'do', 'does', 'did', 'can', 'cannot', 'may', 'must', 'will',
        'more', 'than', 'then', 'one', 'two', 'both', 'any', 'every', 'all', 'so',
        'on', 'for', 'with', 'from', 'into', 'its', 'their', 'them', 'they', 'you',
        'when', 'which', 'while', 'what', 'who', 'whose', 'only',
        'other', 'another', 'same', 'such', 'each', 'but', 'if', 'else', 'error',
        'warning', 'info', 'information', 'rule'}

FIX_WEIGHT = 0.25   #!< a match in the corrective action counts for less than one in the rule


def words_of(text):
    """The words of a message or a rule that carry which rule it is."""
    found = re.findall(r'[a-z0-9]+', text.lower())
    return [word for word in found if word not in STOP and len(word) > 1]


def weights(rules):
    """How much each word says, measured by how few rules use it."""
    counted = {}
    for rule in rules:
        for word in set(words_of(subject_of(rule))):
            counted[word] = counted.get(word, 0) + 1
    total = max(1, len(rules))
    return dict((word, math.log(1.0 + total / float(count)))
                for word, count in counted.items())


def subject_of(rule):
    """What the rule is about: its name and the registry's statement of it."""
    return rule['name'].replace('_', ' ') + ' ' + rule['summary']


def matches_phrase(rule, needle):
    """Whether the generator's wording for this rule is in the message."""
    for phrase in PHRASES.get(rule['name'], []):
        lean = ' '.join(words_of(phrase))
        if lean and lean in ' '.join(words_of(needle)):
            return True
    return False


def score_of(rule, wanted, weight):
    """How much of the message this rule accounts for.

    The rule's own statement counts in full and the corrective action a fraction:
    the action is advice written here, and its ordinary English matches everything.
    """
    subject = set(words_of(subject_of(rule)))
    advice = set(words_of(rule['fix'])) - subject
    total = 0.0
    for word in wanted:
        if word in subject:
            total += weight.get(word, 1.0)
        elif word in advice:
            total += FIX_WEIGHT * weight.get(word, 1.0)
    return total


def search(rules, needle, limit=5):
    """The rules a message can be about, the likeliest first.

    Nothing written the generator's way is guaranteed to be written the registry's
    way, so the phrases the generator is known to use are tried first and the words
    are scored after. A word only the odd rule uses counts for more than one every
    rule uses, and a rule that can be reported as an error settles a tie: a document
    that was refused was refused over an error.
    """
    wanted = set(words_of(needle))
    if not wanted:
        return []
    named = [rule for rule in rules if matches_phrase(rule, needle)]
    if named:
        return named
    weight = weights(rules)
    scored = [(score_of(rule, wanted, weight), rule) for rule in rules]
    scored = [(score, rule) for score, rule in scored if score > 0.0]
    if not scored:
        return []
    scored.sort(key=lambda entry: (-entry[0], 'error' not in entry[1]['bands'],
                                   entry[1]['number']))
    best = scored[0][0]
    kept = [rule for score, rule in scored if score >= best / 2.0]
    return kept[:limit]


def resolve(rules, reported):
    """Finds the rules a reported number can stand for, with the band it was in.

    A number is tried in every band whose offset it can carry, and only rules that
    are actually reported in that band are returned.
    """
    found = []
    for offset, band in BANDS:
        base = reported - offset
        if base <= 0:
            continue
        for rule in rules:
            if rule['number'] == base and band in rule['bands']:
                found.append((band, rule))
    return found


def wrap(text, label):
    """One labelled block, wrapped to 78 columns and indented under its label."""
    body = textwrap.wrap(text, width=78 - len(label) - 4)
    if not body:
        return ''
    pad = ' ' * (len(label) + 4)
    out = '    {} {}'.format(label, body[0])
    for line in body[1:]:
        out += '\n' + pad + line
    return out


def show(band, rule, reported):
    print('{} -- {} ({})'.format(reported, rule['name'], band))
    print(wrap(rule['summary'], 'what: '))
    if rule['fix']:
        print(wrap(rule['fix'], 'fix:  '))
    print('    documents: {}   section: {}'.format(
        ', '.join(rule['documents']) or '-', rule['section'] or '-'))


def main():
    parser = argparse.ArgumentParser(
        description='Explain a validation rule number reported by the generator.')
    parser.add_argument('numbers', nargs='*', type=int,
                        help='the rule numbers as reported')
    parser.add_argument('--document', choices=['siml', 'dtml', 'fsml'],
                        help='keep only rules that apply to this document type')
    parser.add_argument('--list', action='store_true',
                        help='list every rule instead of explaining numbers')
    parser.add_argument('--search', metavar='TEXT',
                        help='find the rule from words in the message, for when '
                             'the generator printed no number')
    args = parser.parse_args()

    rules = load()
    if args.document:
        rules = [r for r in rules if args.document in r['documents']]

    if args.search:
        hits = search(rules, args.search)
        if not hits:
            print('no rule matches: {}'.format(args.search))
            return 1
        for rule in hits:
            for band in rule['bands']:
                offset = dict((b, o) for o, b in BANDS)[band]
                show(band, rule, rule['number'] + offset)
        return 0

    if args.list:
        for rule in sorted(rules, key=lambda r: r['number']):
            print('{:>3}  {:<34} {}'.format(rule['number'], rule['name'],
                                            rule['summary']))
        return 0

    if not args.numbers:
        parser.error('give one or more rule numbers, --search TEXT, or --list')

    missing = 0
    for reported in args.numbers:
        found = resolve(rules, reported)
        if not found:
            print('{} -- no such rule{}'.format(
                reported, ' for a .' + args.document if args.document else ''))
            missing += 1
            continue
        for band, rule in found:
            show(band, rule, reported)

    return 1 if missing else 0


if __name__ == '__main__':
    sys.exit(main())
