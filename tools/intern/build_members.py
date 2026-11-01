#!/usr/bin/env python3
"""Write docs/agent/members.json: every member name the public headers declare.

check_contract.py rule B-08 reports a snake_case call on an areg object whose
name is on no public header. Rule B-09 reports a qualified enumerator an areg
scoped enumeration does not declare. Both lists are generated rather than
curated, so a framework rename cannot leave them behind, and check_corpus.py
regenerates the file and fails when the copy on disk differs.

    python3 tools/intern/build_members.py            # rewrite the file
    python3 tools/intern/build_members.py --check    # exit 1 when it is stale
"""
import argparse
import json
import os
import re
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
ROOT = os.path.dirname(os.path.dirname(HERE))
CORE = os.path.join(ROOT, 'framework', 'areg')
EXTENSION = os.path.join(ROOT, 'framework', 'aregextend')
TARGET = os.path.join(ROOT, 'docs', 'agent', 'members.json')

# A declaration or a definition whose name is snake_case: the shape every public
# framework method has since the rename, after any leading [[attribute]].
DECLARATION = re.compile(r'^\s*(?:\[\[\w+(?:\([^)]*\))?\]\]\s*)*'
                         r'(?:[A-Za-z_][\w:<>,\s\*&]*?\s+)?([a-z][a-z0-9_]*)\s*\(')

# "operator uint32_t (" and "operator bool()" put a type where a name would be. A
# conversion is reached through the type, never through a member call.
CONVERSION = re.compile(r'\boperator\s*$')

# A statement in an inline body has the shape of a declaration: "if (", "return (",
# "switch (". Every one of them is spelled with a reserved word, and so is every
# fundamental type, so the two closed lists below separate them from member names.
KEYWORDS = frozenset("""
alignas alignof and and_eq asm auto bitand bitor bool break case catch char char8_t
char16_t char32_t class compl concept const consteval constexpr constinit const_cast
continue co_await co_return co_yield decltype default delete do double dynamic_cast
else enum explicit export extern false float for friend goto if inline int long
mutable namespace new noexcept not not_eq nullptr operator or or_eq private protected
public register reinterpret_cast requires return short signed sizeof static
static_assert static_cast struct switch template this thread_local throw true try
typedef typeid typename union unsigned using virtual void volatile wchar_t while xor
xor_eq
""".split())

FIXED_WIDTH = frozenset(
    ['size_t', 'ssize_t', 'ptrdiff_t', 'intptr_t', 'uintptr_t', 'intmax_t', 'uintmax_t']
    + ['{}int{}_t'.format(sign, width)
       for sign in ('', 'u')
       for width in ('8', '16', '32', '64',
                     '_least8', '_least16', '_least32', '_least64',
                     '_fast8', '_fast16', '_fast32', '_fast64')])

NOT_A_MEMBER = KEYWORDS | FIXED_WIDTH


def walk(roots):
    """Every directory under each root, skipping private folders."""
    for root in roots:
        for base, dirs, files in os.walk(root):
            dirs[:] = [d for d in dirs if d != 'private']
            yield base, files


def collect(roots):
    """Every snake_case member name declared outside a private folder."""
    names = set()
    headers = 0
    for base, files in walk(roots):
        for name in files:
            if not name.endswith(('.hpp', '.h')):
                continue
            headers += 1
            path = os.path.join(base, name)
            with open(path, encoding='utf-8', errors='ignore') as handle:
                for line in handle:
                    found = DECLARATION.match(line)
                    if not found:
                        continue
                    if found.group(1) in NOT_A_MEMBER:
                        continue
                    if CONVERSION.search(line[:found.start(1)]):
                        continue
                    names.add(found.group(1))
    return headers, sorted(names)


# "enum class Name" and "enum class Name : uint16_t". A forward declaration has no
# body and is skipped by the brace search that follows.
ENUM_HEAD = re.compile(r'\benum\s+class\s+([A-Za-z_]\w*)')

# A comment inside an enumeration body, in either spelling.
ENUM_COMMENT = re.compile(r'//.*?$|/\*.*?\*/', re.S | re.M)

# The name of one enumerator, before any "= value" it carries.
ENUMERATOR = re.compile(r'^([A-Za-z_]\w*)')


def enumerators_of(body):
    """The names one enumeration body declares."""
    found = []
    for entry in ENUM_COMMENT.sub(' ', body).split(','):
        name = ENUMERATOR.match(entry.strip())
        if name:
            found.append(name.group(1))
    return found


def collect_enums(roots):
    """Every enumerator each scoped enumeration declares, outside a private folder.

    An enumeration name that appears in more than one namespace is stored once,
    holding the enumerators of all of them. A name too many can only miss a defect,
    where a name too few reports correct code as wrong.
    """
    enums = {}
    for base, files in walk(roots):
        for name in files:
            if not name.endswith(('.hpp', '.h')):
                continue
            with open(os.path.join(base, name),
                      encoding='utf-8', errors='ignore') as handle:
                text = handle.read()
            for head in ENUM_HEAD.finditer(text):
                opening = text.find('{', head.end())
                closing = text.find('}', opening + 1) if opening >= 0 else -1
                if opening < 0 or closing < 0:
                    continue
                # A body never carries a brace of its own, so the first one closes it.
                if ';' in text[head.end():opening]:
                    continue
                enums.setdefault(head.group(1), set()).update(
                    enumerators_of(text[opening + 1:closing]))
    return dict((name, sorted(values)) for name, values in sorted(enums.items())
                if values)


# "class Name" or "struct Name" opening a definition, with or without the export macro.
CLASS_HEAD = re.compile(r'^\s*(?:class|struct)\s+(?:AREG_\w+\s+)?([A-Z]\w*)\b(?!\s*;)')


def collect_classes(roots):
    """Every class and struct name a public header defines, outside a private folder."""
    names = set()
    for base, files in walk(roots):
        for name in files:
            if not name.endswith(('.hpp', '.h')):
                continue
            with open(os.path.join(base, name),
                      encoding='utf-8', errors='ignore') as handle:
                for line in handle:
                    found = CLASS_HEAD.match(line)
                    if found:
                        names.add(found.group(1))
    return sorted(names)


def build():
    """The inventory of both libraries, and the number of headers it was read from."""
    core_headers, core_names = collect([CORE])
    ext_headers, ext_names = collect([EXTENSION])
    return core_headers + ext_headers, {
        'description': 'Every snake_case name the public areg headers declare or '
                       'call. A named call on an areg object whose name is absent '
                       'here exists nowhere in the framework. Operators are outside '
                       'this list entirely, so absence here says nothing about them: '
                       'read docs/agent/21-data-types.md and 40-base-api.md, and note '
                       'that String and WideString carry + and +=. C++ keywords, '
                       'fundamental and fixed-width type names, and conversion '
                       'operators are not members and are excluded.',
        'source': 'generated by tools/intern/build_members.py, excluding every '
                  'private/ folder: members and enumerations from framework/areg, '
                  'extension from framework/aregextend. A name an inline body calls '
                  'is kept: a name too many can only miss a defect, where a name too '
                  'few reports correct code as wrong.',
        'enums': 'every enumerator each scoped areg enumeration declares. A '
                 'qualified name areg::<Enum>::<Enumerator> whose enumeration is '
                 'listed here and whose enumerator is not exists nowhere in the '
                 'framework. An enumeration declared in more than one namespace is '
                 'listed once, holding the enumerators of all of them.',
        'headers': core_headers,
        'members': core_names,
        'enumerations': collect_enums([CORE]),
        'extension': {
            'library': 'aregextend',
            'description': 'what the aregextend library declares. A class listed '
                           'here derives from the core, so a call on it may name '
                           'a member of either list. A name both libraries declare '
                           'is in both lists.',
            'headers': ext_headers,
            'classes': collect_classes([EXTENSION]),
            'members': ext_names,
            'enumerations': collect_enums([EXTENSION]),
        },
    }


def summary(document):
    """One line naming what the inventory holds per library."""
    ext = document['extension']
    return ('{} names and {} enumeration(s) from {} areg headers; {} names, {} '
            'enumeration(s) and {} classes from {} aregextend headers'
            .format(len(document['members']), len(document['enumerations']),
                    document['headers'], len(ext['members']),
                    len(ext['enumerations']), len(ext['classes']), ext['headers']))


def stale(on_disk, fresh):
    """What differs between the file on disk and a fresh inventory, or None."""
    for label, old, new in (('areg', on_disk, fresh),
                            ('aregextend', on_disk.get('extension') or {},
                             fresh['extension'])):
        if old.get('enumerations') != new['enumerations']:
            return 'the {} enumerations differ'.format(label)
        if old.get('members') != new['members']:
            missing = set(new['members']) - set(old.get('members') or [])
            extra = set(old.get('members') or []) - set(new['members'])
            return '{} {} name(s) added, {} removed'.format(
                label, len(missing), len(extra))
        if 'classes' in new and old.get('classes') != new['classes']:
            return 'the {} classes differ'.format(label)
    return None


def main():
    parser = argparse.ArgumentParser(description=__doc__.splitlines()[0])
    parser.add_argument('--check', action='store_true',
                        help='exit 1 when the file on disk is not what this writes')
    args = parser.parse_args()

    for root in (CORE, EXTENSION):
        if not os.path.isdir(root):
            sys.stderr.write('error: no framework tree at {}\n'.format(root))
            return 2
    headers, fresh = build()
    if not fresh['members'] or not fresh['extension']['members']:
        sys.stderr.write('error: no member names found under framework/\n')
        return 2

    if args.check:
        try:
            with open(TARGET, encoding='utf-8') as handle:
                on_disk = json.load(handle)
        except (OSError, ValueError) as error:
            sys.stderr.write('error: cannot read {}: {}\n'.format(TARGET, error))
            return 1
        problem = stale(on_disk, fresh)
        if problem:
            sys.stderr.write('error: docs/agent/members.json is stale: {}. Run '
                             'tools/intern/build_members.py\n'.format(problem))
            return 1
        print('members.json is current: ' + summary(fresh))
        return 0

    with open(TARGET, 'w', encoding='utf-8', newline='\n') as handle:
        json.dump(fresh, handle, indent=1)
        handle.write('\n')
    print('wrote {}: {}'.format(os.path.relpath(TARGET, ROOT), summary(fresh)))
    return 0


if __name__ == '__main__':
    sys.exit(main())
