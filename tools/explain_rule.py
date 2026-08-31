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
import os
import sys
import xml.etree.ElementTree as ET

RULES = os.path.join(os.path.dirname(os.path.abspath(__file__)), 'schema', 'rules.xml')
BANDS = [(0, 'error'), (100, 'warning'), (200, 'information')]


def load():
    """Reads the registry into a list of rules, in file order."""
    rules = []
    for node in ET.parse(RULES).getroot().findall('.//Rule'):
        rules.append({'number': int(node.get('Number')),
                      'name': node.get('Name') or '',
                      'bands': (node.get('Bands') or '').split(),
                      'documents': (node.get('Documents') or '').split(),
                      'section': node.get('Section') or '',
                      'summary': clean(node.findtext('Summary') or '')})
    return rules


def clean(text):
    """Drops the markup the registry carries for the editor's help viewer."""
    text = text.replace('\\a ', '').replace('\\c ', '').replace('\\b ', '')
    return ' '.join(text.split())


def search(rules, needle):
    """Rules whose name or summary matches the words of the needle.

    Every word first. Nothing written the generator's way is guaranteed to be
    written the registry's way, so when that finds nothing the words are scored
    separately and the best matches are offered instead.
    """
    stop = {'a', 'an', 'the', 'to', 'of', 'is', 'in', 'at', 'by', 'that',
            'this', 'it', 'as', 'and', 'or', 'not', 'no'}
    words = [w.strip('.,:;[]()"\'').lower() for w in needle.split()]
    words = [w for w in words if w and w not in stop]
    if not words:
        return []
    hits = [r for r in rules
            if all(w in (r['name'] + ' ' + r['summary']).lower() for w in words)]
    if hits:
        return hits
    scored = []
    for rule in rules:
        hay = (rule['name'] + ' ' + rule['summary']).lower()
        score = sum(1 for w in words if w in hay)
        if score:
            scored.append((score, rule))
    if not scored:
        return []
    best = max(score for score, _ in scored)
    return [rule for score, rule in scored if score == best]


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


def show(band, rule, reported):
    print('{} -- {} ({})'.format(reported, rule['name'], band))
    print('    {}'.format(rule['summary']))
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
        for rule in sorted(hits, key=lambda r: r['number']):
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
