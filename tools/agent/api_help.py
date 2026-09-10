#!/usr/bin/env python3
# ===========================================================================
# Answers what one framework name is, from the public headers under
# framework/areg. One name in, its declarations and the header out.
#
#   python3 tools/agent/api_help.py start_timer     # every declaration of a member
#   python3 tools/agent/api_help.py Timer --class   # what one class declares
#   python3 tools/agent/api_help.py DataState       # a scoped enumeration
#   python3 tools/agent/api_help.py --search timer  # names holding a word
#
# --full adds the documentation block a declaration carries. Everything printed
# is read from the headers at run time and nothing is written down here, so an
# answer cannot disagree with the framework it describes.
#
# Reading a header instead costs 20 KB to 60 KB, carried in context and re-sent
# on every later request. This answers in a few lines.
#
# Exit code 0 when every name was answered, 1 otherwise.
# ===========================================================================
import argparse
import json
import os
import re
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
ROOT = os.path.dirname(os.path.dirname(HERE))
FRAMEWORK = os.path.join(ROOT, 'framework', 'areg')

BLOCK_COMMENT = re.compile(r'/\*.*?\*/', re.S)
LINE_COMMENT = re.compile(r'//[^\n]*')

# "class AREG_API Timer final : public TimerBase" and "struct Entry".
CLASS_HEAD = re.compile(r'^\s*(?:class|struct)\s+(?:AREG_API\s+)?([A-Za-z_]\w*)'
                        r'(?!\s*;)(?![\w])')

# "enum class DataState : uint8_t".
ENUM_HEAD = re.compile(r'\benum\s+class\s+([A-Za-z_]\w*)')
ENUMERATOR = re.compile(r'^([A-Za-z_]\w*)')

# One declaration: an optional return type, the name, and the parameter list.
DECLARATION = re.compile(r'^(.*?\b)([A-Za-z_~]\w*)\s*\((.*)$', re.S)

# A deleted constructor, the copy guard and the self() helper are shape, not API.
BOILERPLATE = re.compile(r'=\s*delete|AREG_NOCOPY|\bself\s*\(')

NOT_A_NAME = frozenset("""
if for while switch return catch sizeof alignof decltype static_assert throw
noexcept explicit operator delete new
""".split())


MEMBERS = os.path.join(ROOT, 'docs', 'agent', 'members.json')


def catalogued(name):
    """True when docs/agent/members.json carries the name this reader did not find.

    A name a macro declares has no declaration line to read, so absence here is not
    absence from the framework and must never be reported as one.
    """
    try:
        with open(MEMBERS, encoding='utf-8') as handle:
            return name in set(json.load(handle).get('members', []))
    except (ValueError, OSError):
        return False


def fail(message):
    sys.stderr.write('error: {}\n'.format(message))
    sys.exit(2)


def clean(text):
    """The header without its comments, so a comment cannot look like code."""
    return LINE_COMMENT.sub('', BLOCK_COMMENT.sub(lambda m: '\n' * m.group(0).count('\n'),
                                                  text))


# What may stand between a documentation block and the declaration it describes.
SKIPPED_ABOVE = re.compile(r'^\s*(?:\[\[[^\]]*\]\]|template\s*<.*>|virtual|inline|'
                           r'static|explicit|constexpr|AREG_API)?\s*$')


def doc_before(lines, index):
    """The documentation block that sits above a declaration, as plain sentences."""
    parts = []
    cursor = index - 1
    while cursor >= 0 and SKIPPED_ABOVE.match(lines[cursor]):
        cursor -= 1
    if cursor < 0 or '*/' not in lines[cursor]:
        return ''
    end = cursor
    while cursor >= 0 and '/*' not in lines[cursor]:
        cursor -= 1
    if cursor < 0:
        return ''
    for line in lines[cursor:end + 1]:
        line = line.strip().lstrip('/*').lstrip('*').strip()
        if line.startswith('\\brief'):
            line = line[len('\\brief'):].strip()
        elif line.startswith('\\'):
            # The brief ends where the first \param or \return begins.
            break
        if line and not line.startswith('=='):
            parts.append(line)
    return ' '.join(parts)


class Header(object):
    """One public header, indexed by the names it declares."""

    def __init__(self, path, relative):
        self.path = path
        self.relative = relative
        self.members = []        #!< (owner, signature, doc)
        self.classes = {}        #!< class name -> [signature]
        self.enums = {}          #!< enumeration name -> [enumerator]
        self.seen = set()
        self._parse()

    def _parse(self):
        with open(self.path, encoding='utf-8', errors='ignore') as handle:
            raw = handle.read()
        self._parse_enums(raw)
        text = clean(raw)
        raw_lines = raw.splitlines()
        depth = 0
        owners = []              #!< (depth the class body opened at, name)
        pending = None
        buffer = ''
        start = 0
        for number, line in enumerate(text.splitlines()):
            head = CLASS_HEAD.match(line)
            if head and '(' not in line.split(head.group(1))[0]:
                pending = head.group(1)
            if buffer or ('(' in line and ';' not in line.split('(')[0]):
                if not buffer:
                    start = number
                buffer += ' ' + line.strip()
                if buffer.count('(') <= buffer.count(')'):
                    statement = buffer.strip()
                    buffer = ''
                    self._declaration(statement, owners, raw_lines, start)
            opened = line.count('{')
            closed = line.count('}')
            for _ in range(opened):
                depth += 1
                owners.append((depth, pending))
                pending = None
            for _ in range(closed):
                while owners and owners[-1][0] >= depth:
                    owners.pop()
                depth = max(0, depth - 1)

    def _declaration(self, statement, owners, raw_lines, start):
        statement = ' '.join(statement.split())
        if not statement.endswith((';', '{', '}')) and '{' not in statement:
            return
        cut = statement.find('{')
        if cut >= 0:
            statement = statement[:cut].strip()
        statement = statement.rstrip(';').strip()
        if not statement or statement.startswith('#'):
            return
        found = DECLARATION.match(statement)
        if not found:
            return
        name = found.group(2)
        if name in NOT_A_NAME or found.group(1).rstrip().endswith('operator'):
            return
        prefix = found.group(1).rstrip()
        if '=' in prefix or ')' in prefix:
            return
        # A call on an object, and an out-of-line definition of a member already
        # declared inside its class: neither is a declaration.
        if prefix.endswith(('.', '->', '::')):
            return
        head = prefix.split()
        if head and head[0] in NOT_A_NAME:
            return
        if BOILERPLATE.search(statement) or name.startswith('_'):
            return
        owner = next((entry[1] for entry in reversed(owners) if entry[1]), None)
        doc = doc_before(raw_lines, start)
        signature = statement + ';'
        if (name, owner, signature) in self.seen:
            return
        self.seen.add((name, owner, signature))
        self.members.append((name, owner, signature, doc))
        if owner:
            self.classes.setdefault(owner, []).append(signature)

    def _parse_enums(self, text):
        for head in ENUM_HEAD.finditer(text):
            opening = text.find('{', head.end())
            closing = text.find('}', opening + 1) if opening >= 0 else -1
            if opening < 0 or closing < 0:
                continue
            body = clean(text[opening + 1:closing])
            names = []
            for entry in body.split(','):
                match = ENUMERATOR.match(entry.strip())
                if match:
                    names.append(match.group(1))
            if names:
                self.enums[head.group(1)] = names


def load():
    """Every public header, outside a private folder."""
    headers = []
    if not os.path.isdir(FRAMEWORK):
        fail('framework/areg not found under {}'.format(ROOT))
    for base, folders, files in os.walk(FRAMEWORK):
        folders[:] = [f for f in folders if f != 'private']
        for name in sorted(files):
            if name.endswith(('.hpp', '.h')):
                path = os.path.join(base, name)
                headers.append(Header(path,
                                      os.path.relpath(path, os.path.join(ROOT, 'framework'))
                                      .replace('\\', '/')))
    return headers


def answer_member(headers, name, full):
    """Every declaration of one member name, grouped by the class that owns it."""
    groups = {}
    for header in headers:
        for member, owner, signature, doc in header.members:
            if member != name:
                continue
            key = ('areg::' + owner if owner else 'namespace areg', header.relative)
            groups.setdefault(key, []).append((signature, doc))
    if not groups:
        return False
    total = sum(len(v) for v in groups.values())
    print('{} -- {} declaration(s)'.format(name, total))
    for (owner, path), entries in sorted(groups.items()):
        print('')
        print('{}  ({})'.format(owner, path))
        for signature, doc in entries:
            print('  {}'.format(signature))
            if full and doc:
                print('      {}'.format(doc))
    return True


def answer_class(headers, name, full):
    """What one class declares, in the order the header declares it."""
    found = False
    for header in headers:
        if name not in header.classes:
            continue
        found = True
        entries = header.classes[name]
        print('areg::{}  ({}) -- {} declaration(s)'
              .format(name, header.relative, len(entries)))
        for signature in entries:
            print('  {}'.format(signature))
    return found


def answer_enum(headers, name):
    """The values one scoped enumeration declares."""
    for header in headers:
        if name in header.enums:
            values = header.enums[name]
            print('enum class areg::{}  ({}) -- {} value(s)'
                  .format(name, header.relative, len(values)))
            print('  {}'.format(', '.join(values)))
            return True
    return False


def search(headers, word):
    """Every member, class and enumeration name holding a word."""
    needle = word.lower()
    members, classes, enums = set(), set(), set()
    for header in headers:
        for member, _, _, _ in header.members:
            if needle in member.lower():
                members.add(member)
        for owner in header.classes:
            if needle in owner.lower():
                classes.add(owner)
        for enum in header.enums:
            if needle in enum.lower():
                enums.add(enum)
    for label, names in (('class', classes), ('enum', enums), ('member', members)):
        if names:
            print('{}: {}'.format(label, ', '.join(sorted(names))))
    return bool(members or classes or enums)


def main():
    parser = argparse.ArgumentParser(
        description='What one framework name is, from the public areg headers.')
    parser.add_argument('names', nargs='*',
                        help='the names to answer: a member, a class or an enumeration')
    parser.add_argument('--class', dest='as_class', action='store_true',
                        help='read the name as a class and list what it declares')
    parser.add_argument('--enum', action='store_true',
                        help='read the name as a scoped enumeration')
    parser.add_argument('--full', action='store_true',
                        help='add the documentation block each declaration carries')
    parser.add_argument('--search', help='find a name holding this word')
    args = parser.parse_args()

    if not args.names and not args.search:
        parser.error('give a name, or --search <word>')

    headers = load()
    missing = 0
    if args.search:
        if not search(headers, args.search):
            print('nothing holds "{}"'.format(args.search))
            missing += 1
    for index, name in enumerate(args.names):
        if index:
            print('')
        if args.as_class:
            answered = answer_class(headers, name, args.full)
        elif args.enum:
            answered = answer_enum(headers, name)
        else:
            answered = (answer_member(headers, name, args.full)
                        or answer_class(headers, name, args.full)
                        or answer_enum(headers, name))
        if not answered:
            if catalogued(name):
                print('"{}" is a member of the framework -- docs/agent/members.json '
                      'lists it -- but no header declares it on a line this reader '
                      'can read, so it comes from a macro. It exists; call it.'
                      .format(name))
                continue
            print('no public areg header declares "{}" under that exact spelling.'
                  .format(name))
            print('Operators are reached through the type and are not members, so '
                  'this says nothing about + or ==.')
            if not search(headers, name):
                print('and no name holds it either.')
            missing += 1
    return 1 if missing else 0


if __name__ == '__main__':
    sys.exit(main())
