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
# A finding prints its number in brackets, as error[6/RULE_UNRESOLVED_TYPE], so the
# number is the usual argument. --search goes the other way, for a message quoted
# without it: give it words from the message and it names the rule.
#
# Both texts come from schema/rules.xml and nothing is hard coded here: Summary says
# what is wrong, Fix says what to change. The code generator answers the same number
# with the same words through its own --explain option.
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


def load():
    """Reads the registry into a list of rules, in file order.

    The registry is the only source of a rule. A checkout without it cannot answer
    anything, so it says so rather than answering half.
    """
    try:
        root = ET.parse(RULES).getroot()
    except (ET.ParseError, OSError) as failure:
        raise SystemExit('cannot read the rule registry {}: {}\n'
                         'It is the only place a rule is written down. Restore it '
                         'from the repository.'.format(RULES, failure))

    rules = []
    for node in root.findall('.//Rule'):
        rules.append({'number': int(node.get('Number')),
                      'name': node.get('Name') or '',
                      'bands': (node.get('Bands') or '').split(),
                      'documents': (node.get('Documents') or '').split(),
                      'section': node.get('Section') or '',
                      'summary': clean(node.findtext('Summary') or ''),
                      'fix': clean(node.findtext('Fix') or '')})
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
 'RULE_FORMAT_VERSION':   ['a newer format than this generator reads',
                           'an older major of the format'],
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
    body = textwrap.wrap(text, width=78 - len(label) - 4, break_on_hyphens=False)
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
                        help='find the rule from words in the message, for a '
                             'message quoted without its number')
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
