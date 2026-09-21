#!/usr/bin/env python3
"""Write docs/agent/members.json: every member name the public headers declare.

check_contract.py rule B-08 reports a snake_case call on an areg object whose
name is on no public header. Rule B-09 reports a qualified enumerator an areg
scoped enumeration does not declare. Both lists are generated rather than
curated, so a framework rename cannot leave them behind, and check_corpus.py
regenerates the file and fails when the copy on disk differs.

    python3 tools/agent/build_members.py            # rewrite the file
    python3 tools/agent/build_members.py --check    # exit 1 when it is stale
"""
import argparse
import json
import os
import re
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
ROOT = os.path.dirname(os.path.dirname(HERE))
FRAMEWORK = os.path.join(ROOT, 'framework', 'areg')
TARGET = os.path.join(ROOT, 'docs', 'agent', 'members.json')

# A declaration or a definition whose name is snake_case: the shape every public
# framework method has since the rename.
DECLARATION = re.compile(r'^\s*(?:[A-Za-z_][\w:<>,\s\*&]*?\s+)?([a-z][a-z0-9_]*)\s*\(')

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


def collect(framework):
    """Every snake_case member name declared outside a private folder."""
    names = set()
    headers = 0
    for base, dirs, files in os.walk(framework):
        dirs[:] = [d for d in dirs if d != 'private']
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


def collect_enums(framework):
    """Every enumerator each scoped enumeration declares, outside a private folder.

    An enumeration name that appears in more than one namespace is stored once,
    holding the enumerators of all of them. A name too many can only miss a defect,
    where a name too few reports correct code as wrong.
    """
    enums = {}
    for base, dirs, files in os.walk(framework):
        dirs[:] = [d for d in dirs if d != 'private']
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


def document(headers, names, enums):
    return {
        'description': 'Every snake_case name the public areg headers declare or '
                       'call. A named call on an areg object whose name is absent '
                       'here exists nowhere in the framework. Operators are outside '
                       'this list entirely, so absence here says nothing about them: '
                       'read docs/agent/21-data-types.md and 40-base-api.md, and note '
                       'that String and WideString carry + and +=. C++ keywords, '
                       'fundamental and fixed-width type names, and conversion '
                       'operators are not members and are excluded.',
        'source': 'generated by tools/agent/build_members.py from framework/areg, '
                  'excluding every private/ folder. A name an inline body calls is '
                  'kept: a name too many can only miss a defect, where a name too '
                  'few reports correct code as wrong.',
        'enums': 'every enumerator each scoped areg enumeration declares. A '
                 'qualified name areg::<Enum>::<Enumerator> whose enumeration is '
                 'listed here and whose enumerator is not exists nowhere in the '
                 'framework. An enumeration declared in more than one namespace is '
                 'listed once, holding the enumerators of all of them.',
        'headers': headers,
        'members': names,
        'enumerations': enums,
    }


def main():
    parser = argparse.ArgumentParser(description=__doc__.splitlines()[0])
    parser.add_argument('--check', action='store_true',
                        help='exit 1 when the file on disk is not what this writes')
    args = parser.parse_args()

    if not os.path.isdir(FRAMEWORK):
        sys.stderr.write('error: no framework tree at {}\n'.format(FRAMEWORK))
        return 2
    headers, names = collect(FRAMEWORK)
    if not names:
        sys.stderr.write('error: no member names found under {}\n'.format(FRAMEWORK))
        return 2
    fresh = document(headers, names, collect_enums(FRAMEWORK))

    if args.check:
        try:
            with open(TARGET, encoding='utf-8') as handle:
                on_disk = json.load(handle)
        except (OSError, ValueError) as error:
            sys.stderr.write('error: cannot read {}: {}\n'.format(TARGET, error))
            return 1
        if on_disk.get('enumerations') != fresh['enumerations']:
            sys.stderr.write('error: docs/agent/members.json is stale: the '
                             'enumerations differ. Run '
                             'tools/agent/build_members.py\n')
            return 1
        if on_disk.get('members') != fresh['members']:
            missing = sorted(set(fresh['members']) - set(on_disk.get('members') or []))
            extra = sorted(set(on_disk.get('members') or []) - set(fresh['members']))
            sys.stderr.write('error: docs/agent/members.json is stale: '
                             '{} name(s) added, {} removed. Run '
                             'tools/agent/build_members.py\n'
                             .format(len(missing), len(extra)))
            return 1
        print('members.json is current: {} names and {} enumeration(s) from {} '
              'headers'.format(len(fresh['members']),
                               len(fresh['enumerations']), headers))
        return 0

    with open(TARGET, 'w', encoding='utf-8') as handle:
        json.dump(fresh, handle, indent=1)
        handle.write('\n')
    print('wrote {}: {} names and {} enumeration(s) from {} headers'
          .format(os.path.relpath(TARGET, ROOT), len(names),
                  len(fresh['enumerations']), headers))
    return 0


if __name__ == '__main__':
    sys.exit(main())
