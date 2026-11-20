#!/usr/bin/env python3
# ===========================================================================
# Answers what one framework name is, from the public headers under
# framework/areg and framework/aregextend. One name in, its declarations and the
# header out.
#
#   python3 tools/agent/api_help.py start_timer     # every declaration of a member
#   python3 tools/agent/api_help.py Timer --class   # what one class declares
#   python3 tools/agent/api_help.py DataState       # a scoped enumeration
#   python3 tools/agent/api_help.py RequestBusy     # a value, its enumeration, its contract
#   python3 tools/agent/api_help.py --search timer  # names holding a word
#
# A member answer of a few declarations carries their briefs; --full adds them to every
# answer, uncut. Everything printed is read at run time from the headers and from
# docs/agent/api.json, and nothing is written down here.
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
import textwrap

HERE = os.path.dirname(os.path.abspath(__file__))
ROOT = os.path.dirname(os.path.dirname(HERE))
FRAMEWORK = os.path.join(ROOT, 'framework', 'areg')
EXTENSION = os.path.join(ROOT, 'framework', 'aregextend')

sys.path.insert(0, HERE)
from gen_skeleton import CLASSES as DESIGN_CLASSES, CLASS_HEADERS, WORKSHEET, snake  # noqa: E402
import check_contract  # noqa: E402

# The stand-ins api.json spells a generated member name with: request_foo, on_baz_update.
STAND_INS = ('foo', 'bar', 'baz')

BLOCK_COMMENT = re.compile(r'/\*.*?\*/', re.S)
LINE_COMMENT = re.compile(r'//[^\n]*')

# "class AREG_API Timer final : public TimerBase" and "struct Entry".
CLASS_HEAD = re.compile(r'^\s*(?:class|struct)\s+(?:AREG_API\s+)?([A-Za-z_]\w*)'
                        r'(?!\s*;)(?![\w])')

# "enum class DataState : uint8_t".
ENUM_HEAD = re.compile(r'\benum\s+class\s+([A-Za-z_]\w*)')
ENUMERATOR = re.compile(r'^([A-Za-z_]\w*)')

# "    , RequestBusy = 8323 /*0x2083*/  //!< request cannot execute, it is busy.    Bits: ..."
ENUMERATOR_BRIEF = re.compile(r'//!<\s*(.*)$')

# One declaration: an optional return type, the name, and the parameter list.
DECLARATION = re.compile(r'^(.*?\b)([A-Za-z_~]\w*)\s*\((.*)$', re.S)

# "bool operator == (const String& other) const" and "operator bool() const".
OPERATOR = re.compile(r'^(.*?)\boperator\s*(\(\)|\[\]|[^\w\s(]+|[A-Za-z_][\w:\s\*&]*?)'
                      r'\s*\((.*)$', re.S)

# "constexpr CharPos START_POS { areg::FIRST_INDEX };" and "const int SIZE = 8;".
# A constant carries no parameter list, so DECLARATION never matches one.
CONSTANT = re.compile(r'^\s*(?:inline\s+|static\s+)*'
                      r'(?:constexpr|const|constinit)\s+'
                      r'[A-Za-z_][\w:<>,\s\*&]*?'
                      r'\b([A-Za-z_]\w*)\s*(?:\{[^;]*\}|=[^;]*)?\s*;\s*$')

# A deleted constructor, the copy guard and the self() helper are shape, not API.
BOILERPLATE = re.compile(r'=\s*delete|AREG_NOCOPY|\bself\s*\(')

# The owner of a brace that opens a function body: nothing inside one is a declaration.
BODY = '{body}'

# "using Base::Base;" and "using Base::operator =;" inside a class.
USING = re.compile(r'^\s*using\s+(?:[\w:]*::)?(\w+)::(operator\s*=|\w+)\s*;')

NOT_A_NAME = frozenset("""
if for while switch return catch sizeof alignof decltype static_assert throw
noexcept explicit operator delete new
""".split())


MEMBERS = os.path.join(ROOT, 'docs', 'agent', 'members.json')
CONTRACT = os.path.join(ROOT, 'docs', 'agent', 'api.json')

# A member answer of at most this many declarations prints each one's brief, cut to BRIEF_CHARS.
BRIEF_DECLARATIONS = 4
BRIEF_CHARS = 200


def catalogued(name):
    """True when docs/agent/members.json carries the name this reader did not find.

    A name a macro declares has no declaration line to read, so absence here is not
    absence from the framework and must never be reported as one.
    """
    try:
        with open(MEMBERS, encoding='utf-8') as handle:
            document = json.load(handle)
        extension = document.get('extension') or {}
        return name in set(document.get('members', [])) | set(extension.get('members', []))
    except (ValueError, OSError):
        return False


def fail(message):
    # Output already printed is flushed first: stdout is block-buffered into a
    # pipe, so without this the error reaches the reader before the lines it is
    # about.
    sys.stdout.flush()
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


# "template<typename Ch, IfExact<Ch, CharType> = 0>" alone on its line.
TEMPLATE_HEAD = re.compile(r'^\s*(template\s*<.*>)\s*$')

# An attribute alone on its line, between a template head and the declaration.
ATTRIBUTE_LINE = re.compile(r'^\s*\[\[[^\]]*\]\]\s*$')


def template_head(lines, index):
    """The template head written above a declaration and a space, or nothing."""
    cursor = index - 1
    while cursor >= 0 and ATTRIBUTE_LINE.match(lines[cursor]):
        cursor -= 1
    found = TEMPLATE_HEAD.match(lines[cursor]) if cursor >= 0 else None
    return ' '.join(found.group(1).split()) + ' ' if found else ''


# "namespace areg::ext {" or "namespace ext", whose brace may open on the next line.
NAMESPACE_HEAD = re.compile(r'^\s*namespace\s+([A-Za-z_][\w:]*)\s*(?:\{|$)')

# The namespace each class, enumeration and free name is declared in, filled by load().
SCOPE = {}


def namespaces_by_line(lines):
    """The enclosing namespace at the start of each line, "areg" when there is none."""
    found = []
    stack = []
    depth = 0
    pending = None
    for line in lines:
        found.append('::'.join(name for _depth, name in stack) or 'areg')
        head = NAMESPACE_HEAD.match(line)
        if head:
            pending = head.group(1)
        for char in line:
            if char == '{':
                depth += 1
                if pending:
                    stack.append((depth, pending))
                    pending = None
            elif char == '}':
                if stack and stack[-1][0] == depth:
                    stack.pop()
                depth = max(0, depth - 1)
    return found


def qualified(name, header=None):
    """The name with its namespace, "areg::ext::Console" for a class of aregextend."""
    space = (header.scope.get(name) if header else None) or SCOPE.get(name, 'areg')
    return space + '::' + name


def where_of(owner, member, header):
    """The class that owns a declaration with its namespace, or the namespace itself."""
    if owner:
        return qualified(owner)
    return 'namespace ' + header.scope.get(member, 'areg')


class Header(object):
    """One public header, indexed by the names it declares."""

    def __init__(self, path, relative):
        self.path = path
        self.relative = relative
        self.members = []        #!< (owner, signature, doc)
        self.classes = {}        #!< class name -> [signature]
        self.enums = {}          #!< enumeration name -> [enumerator]
        self.enum_briefs = {}    #!< (enumeration name, enumerator) -> its trailing //!< comment
        self.constants = {}      #!< constant name -> (owner, signature, doc)
        self.bases = {}          #!< class name -> [public base class name]
        self.usings = {}         #!< class name -> {'constructors', 'operator='} it takes from a base
        self.scope = {}          #!< class, enumeration or free name -> its namespace
        self.seen = set()
        self._parse()

    def _parse(self):
        with open(self.path, encoding='utf-8', errors='ignore') as handle:
            raw = handle.read()
        text = clean(raw)
        self._spaces = namespaces_by_line(text.splitlines())
        self._parse_enums(raw)
        self._parse_constants(text, raw.splitlines())
        raw_lines = raw.splitlines()
        depth = 0
        owners = []              #!< (depth the class body opened at, name)
        pending = None
        buffer = ''
        start = 0
        lines = text.splitlines()
        for number, line in enumerate(lines):
            in_body = pending == BODY or any(entry[1] == BODY for entry in owners)
            head = CLASS_HEAD.match(line)
            if head and not in_body and '(' not in line.split(head.group(1))[0]:
                pending = head.group(1)
                self.scope.setdefault(pending, self._spaces[number])
                self._bases(pending, line, lines[number + 1:number + 4])
            used = USING.match(line) if not in_body else None
            if used:
                cls = next((entry[1] for entry in reversed(owners) if entry[1] and entry[1] != BODY), None)
                taken = ('constructors' if used.group(1) == used.group(2)
                         else 'operator=' if used.group(2).startswith('operator') else None)
                if cls and taken:
                    self.usings.setdefault(cls, set()).add(taken)
            if not in_body and (buffer or ('(' in line and ';' not in line.split('(')[0])):
                if not buffer:
                    start = number
                buffer += ' ' + line.strip()
                if buffer.count('(') <= buffer.count(')'):
                    statement = buffer.strip()
                    buffer = ''
                    # A constructor whose initializer list, or a function whose body,
                    # starts on the next line.
                    following = lines[number + 1].strip() if number + 1 < len(lines) else ''
                    if following.startswith(':') and not following.startswith('::'):
                        statement += ' {'
                    elif (following.startswith('{') and not statement.startswith('#')
                          and not statement.endswith('\\')):
                        statement += ' {'
                    if '{' in statement and not statement.endswith(';'):
                        pending = BODY
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

    def _bases(self, name, line, following):
        """Records the public base classes a class head names."""
        text = line[CLASS_HEAD.match(line).end():]
        for extra in following:
            if '{' in text or ';' in text:
                break
            text += ' ' + extra
        text = re.sub(r'^final\b', '', text.split('{')[0].split(';')[0].strip()).strip()
        if not text.startswith(':') or text.startswith('::'):
            return
        text = text[1:]
        while '<' in text:
            shorter = re.sub(r'<[^<>]*>', '', text)
            if shorter == text:
                break
            text = shorter
        is_struct = line.lstrip().startswith('struct')
        for part in text.split(','):
            words = part.split()
            if not words or 'private' in words or 'protected' in words:
                continue
            if 'public' not in words and not is_struct:
                continue
            self.bases.setdefault(name, []).append(words[-1].split('::')[-1])

    def _declaration(self, statement, owners, raw_lines, start):
        statement = ' '.join(statement.split())
        if not statement.endswith((';', '{', '}')) and '{' not in statement:
            return
        cut = statement.find('{')
        if cut >= 0:
            statement = statement[:cut].strip()
        statement = statement.rstrip(';').strip()
        if not statement or statement.startswith('#') or '##' in statement:
            return
        found = OPERATOR.match(statement)
        if found:
            symbol = ' '.join(found.group(2).split())
            name = ('operator ' + symbol if symbol[:1].isalpha() or symbol[:1] == '_'
                    else 'operator' + symbol.replace(' ', ''))
        else:
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
        if prefix.endswith(('.', '->', '::', '::~')):
            return
        head = [word for word in prefix.split() if word != 'explicit']
        if head and head[0] in NOT_A_NAME:
            return
        if BOILERPLATE.search(statement) or name.startswith('_'):
            return
        owner = next((entry[1] for entry in reversed(owners) if entry[1] and entry[1] != BODY), None)
        doc = doc_before(raw_lines, start)
        signature = template_head(raw_lines, start) + statement + ';'
        if (name, owner, signature) in self.seen:
            return
        self.seen.add((name, owner, signature))
        if not owner:
            self.scope.setdefault(name, self._spaces[start])
        self.members.append((name, owner, signature, doc))
        if owner:
            self.classes.setdefault(owner, []).append(signature)

    def _parse_constants(self, text, raw_lines):
        """Every constant the header declares, at namespace scope or in a class.

        A constant has no parameter list, so the declaration reader skips it. Leaving
        them out made this tool report an absence for a name it printed itself, in a
        default argument, in the same output.
        """
        depth = 0
        owners = []
        pending = None
        for number, line in enumerate(text.splitlines()):
            head = CLASS_HEAD.match(line)
            if head and '(' not in line.split(head.group(1))[0]:
                pending = head.group(1)
            # A function declaration carries "(" in its head; a constant carries it
            # only inside an initialiser, as in "{ std::numeric_limits<T>::max() }".
            if '(' not in re.split(r'[{=]', line, maxsplit=1)[0]:
                found = CONSTANT.match(line)
                if found:
                    name = found.group(1)
                    if name not in NOT_A_NAME and not name.startswith('_'):
                        owner = next((e[1] for e in reversed(owners) if e[1]), None)
                        if not owner:
                            self.scope.setdefault(name, self._spaces[number])
                        self.constants.setdefault(
                            name, (owner, ' '.join(line.split()), ''))
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
                self.scope.setdefault(head.group(1),
                                      self._spaces[text.count('\n', 0, head.start())])
                self._parse_enum_briefs(head.group(1), text[opening + 1:closing])

    def _parse_enum_briefs(self, enum, body):
        """The //!< comment of each enumerator that has a line of its own, up to its padding."""
        for line in body.splitlines():
            comment = ENUMERATOR_BRIEF.search(line)
            entries = [entry.strip() for entry in clean(line).split(',') if entry.strip()]
            if comment is None or len(entries) != 1:
                continue
            match = ENUMERATOR.match(entries[0])
            brief = re.split(r'\s{3,}', comment.group(1).strip())[0]
            if match and brief:
                self.enum_briefs[(enum, match.group(1))] = brief


def load():
    """Every public header, outside a private folder."""
    headers = []
    if not os.path.isdir(FRAMEWORK):
        fail('framework/areg not found under {}'.format(ROOT))
    roots = [FRAMEWORK] + ([EXTENSION] if os.path.isdir(EXTENSION) else [])
    for root in roots:
        for base, folders, files in os.walk(root):
            folders[:] = [f for f in folders if f != 'private']
            for name in sorted(files):
                if name.endswith(('.hpp', '.h')):
                    path = os.path.join(base, name)
                    headers.append(Header(path,
                                          os.path.relpath(path,
                                                          os.path.join(ROOT, 'framework'))
                                          .replace('\\', '/')))
    for header in headers:
        for name in list(header.classes) + list(header.enums):
            SCOPE.setdefault(name, header.scope.get(name, 'areg'))
    return headers


def declares(headers, name):
    """True while some public header still declares this member name."""
    return any(member == name
               for header in headers for member, _owner, _sig, _doc in header.members)


def answer_member(headers, name, full):
    """Every declaration of one member name, grouped by the class that owns it."""
    groups = {}
    for header in headers:
        for member, owner, signature, doc in header.members:
            if member != name:
                continue
            key = (where_of(owner, member, header), header.relative)
            groups.setdefault(key, []).append((signature, doc))
    if not groups:
        return False
    total = sum(len(v) for v in groups.values())
    briefs = full or total <= BRIEF_DECLARATIONS
    print('{} -- {} declaration(s)'.format(name, total))
    for (owner, path), entries in sorted(groups.items()):
        print('')
        print('{}  ({})'.format(owner, path))
        for signature, doc in entries:
            print('  {}'.format(signature))
            if briefs and doc:
                print('      {}'.format(doc if full else short_brief(doc)))
    if not briefs:
        print('')
        print('--full adds what each one does.')
    return True


def short_brief(doc):
    """The brief cut to BRIEF_CHARS, at the last sentence end inside it when there is one."""
    if len(doc) <= BRIEF_CHARS:
        return doc
    cut = doc.rfind('. ', 0, BRIEF_CHARS)
    return doc[:cut + 1] if cut > 0 else doc[:BRIEF_CHARS].rstrip() + '...'


def answer_constant(headers, name):
    """Every constant of one name, with the header that declares it."""
    found = False
    for header in headers:
        if name not in header.constants:
            continue
        owner, signature, _doc = header.constants[name]
        where = where_of(owner, name, header)
        print('{} -- a constant'.format(name))
        print('')
        print('{}  ({})'.format(where, header.relative))
        print('  {}'.format(signature))
        found = True
    return found


def answer_macro(headers, name):
    """The first definition of a macro of this name, with its header and brief."""
    pattern = re.compile(r'^\s*#\s*define\s+' + re.escape(name) + r'(\([^)]*\))?[ \t]+\S')
    for header in headers:
        with open(header.path, encoding='utf-8', errors='ignore') as handle:
            lines = handle.read().splitlines()
        for number, line in enumerate(lines):
            hit = pattern.match(line)
            if hit is None:
                continue
            brief = ''
            for back in range(number - 1, max(number - 8, -1), -1):
                found = re.search(r'\\brief\s+(.*)', lines[back])
                if found:
                    brief = found.group(1).strip()
                    break
                if not lines[back].strip().startswith(('*', '/**')):
                    break
            print('{}{} -- a macro  ({})'.format(name, hit.group(1) or '', header.relative))
            if brief:
                print('  {}'.format(brief))
            return True
    return False


def role_in(signature, name):
    """What a name is where it appears in a declaration.

    'parameter' names an argument of that declaration, 'value' a default argument,
    'type' anything else: a return type, a parameter's type, a qualifier. A
    parameter name is not a name a caller can use, and saying so is the difference
    between an answer and a wrong instruction.
    """
    hit = re.search(r'(?<!\w)' + re.escape(name) + r'(?![\w])', signature)
    if hit is None:
        return 'type'
    opened = signature.find('(')
    closed = signature.rfind(')')
    if opened < 0 or not opened < hit.start() < closed:
        return 'type'
    before = signature[max(signature.rfind(',', opened, hit.start()), opened) + 1:
                       hit.start()]
    after = signature[hit.end():].lstrip()
    if '=' in before:
        return 'value'
    if after[:1] in (',', ')', '=') and before.strip():
        return 'parameter'
    return 'type'


def mentioned(headers, name):
    """Where a name appears in a declaration this tool already indexed.

    The net under every absence: a name printed in a signature -- a default argument,
    a return type, a parameter -- exists, whatever the index happens to hold. Claiming
    otherwise while printing the same spelling is the one answer this tool must never
    give.
    """
    # A qualified occurrence -- areg::START_POS in a default argument -- is exactly
    # the evidence this net exists for, so ':' must not block the match.
    word = re.compile(r'(?<!\w)' + re.escape(name) + r'(?![\w])')
    for header in headers:
        for member, owner, signature, _doc in header.members:
            if member != name and word.search(signature):
                return (header.relative, owner, member, signature)
        for owner, signatures in header.classes.items():
            for signature in signatures:
                if word.search(signature):
                    return (header.relative, owner, None, signature)
    return None


def lineage(headers, name):
    """The class and every public base above it, nearest first."""
    chain = [name]
    for current in chain:
        for header in headers:
            for base in header.bases.get(current, []):
                if base not in chain:
                    chain.append(base)
    return chain


def usings(headers, name):
    """What a class takes from its base by a using-declaration: 'constructors', 'operator='."""
    return set().union(*(header.usings.get(name, set()) for header in headers))


def inherited(headers, base, below, assignment=False):
    """The names a base declares that a class below it inherits: no constructor, destructor
    or assignment, which every class declares for itself, unless it takes the assignment
    by a using-declaration."""
    hidden = set(below) | {base}
    return [(member, header) for header in headers
            for member, owner, _signature, _doc in header.members
            if owner == base and member not in hidden and member.lstrip('~') not in hidden
            and (assignment or member != 'operator=')]


def answer_class(headers, name, full):
    """What one class declares, in the order the header declares it, then what it
    inherits from each public base."""
    found = False
    for header in headers:
        if name not in header.classes:
            continue
        found = True
        entries = header.classes[name]
        print('{}  ({}) -- {} declaration(s)'
              .format(qualified(name, header), header.relative, len(entries)))
        for signature in entries:
            print('  {}'.format(signature))
    if found:
        chain = lineage(headers, name)
        taken = usings(headers, name)
        if 'constructors' in taken and len(chain) > 1:
            constructors = [signature for header in headers
                            for member, owner, signature, _doc in header.members
                            if owner == chain[1] and member == chain[1] and '~' not in signature
                            and not re.search(r'\(\s*(?:const\s+)?' + chain[1] + r'\s*&', signature)]
            print('constructors inherited from {} -- {} declaration(s), called as {}(...)'
                  .format(qualified(chain[1]), len(constructors), name))
            for signature in constructors:
                print('  {}'.format(signature))
        for index, base in enumerate(chain[1:], 1):
            entries = inherited(headers, base, chain[:index], index == 1 and 'operator=' in taken)
            if not entries:
                continue
            names = list(dict.fromkeys(member for member, _header in entries))
            print('inherited from {}  ({}) -- {} name(s); {}::<name> gives the '
                  'signatures'.format(qualified(base), entries[0][1].relative,
                                      len(names), name))
            print(textwrap.fill(', '.join(names), width=96, initial_indent='  ',
                                subsequent_indent='  ', break_on_hyphens=False))
    return found


def answer_qualified(headers, name, full):
    """Class::member, answered from the class and every public base above it."""
    owner, _, member = name.rpartition('::')
    owner = owner.split('::')[-1]
    if not owner or owner == 'areg':
        return answer_member(headers, member, full)
    if any(owner in header.enums for header in headers):
        return answer_enumerator(headers, member, owner)
    chain = lineage(headers, owner)
    groups = []
    for base in chain:
        for header in headers:
            entries = [(signature, doc) for found, cls, signature, doc in header.members
                       if found == member and cls == base]
            if entries:
                groups.append((base, header.relative, entries))
    if not groups:
        return False
    print('{} -- {} declaration(s)'.format(name, sum(len(g[2]) for g in groups)))
    for base, path, entries in groups:
        print('')
        print('{}  ({}){}'.format(qualified(base), path, '' if base == owner else
                                   ', inherited by ' + qualified(owner)))
        for signature, doc in entries:
            print('  {}'.format(signature))
            if full and doc:
                print('      {}'.format(doc))
    return True


def answer_enum(headers, name):
    """The values one scoped enumeration declares."""
    for header in headers:
        if name in header.enums:
            values = header.enums[name]
            print('enum class {}  ({}) -- {} value(s)'
                  .format(qualified(name, header), header.relative, len(values)))
            print('  {}'.format(', '.join(values)))
            return True
    return False


def contract_of(enum, value):
    """The api.json blocks whose "type" is this enumeration and whose "values" hold the value."""
    try:
        with open(CONTRACT, encoding='utf-8') as handle:
            document = json.load(handle)
    except (ValueError, OSError):
        return []
    return [(key, block) for key, block in document.items()
            if isinstance(block, dict) and block.get('type') == enum
            and isinstance(block.get('values'), dict) and value in block['values']]


def answer_enumerator(headers, name, owner=None):
    """Every scoped enumeration that holds a value of this name, with its api.json contract."""
    found = False
    for header in headers:
        for enum, values in header.enums.items():
            if name not in values or (owner and enum != owner):
                continue
            spelled = qualified(enum, header)
            if found:
                print('')
            print('{} -- a value of enum class {}  ({}); write {}::{}'
                  .format(name, spelled, header.relative, spelled, name))
            brief = header.enum_briefs.get((enum, name))
            if brief:
                print('  {}'.format(brief))
            for key, block in contract_of(spelled, name):
                where = block.get('handler')
                print('')
                print('docs/agent/api.json "{}"{}:'.format(key, ', in ' + where if where else ''))
                for field, text in block['values'][name].items():
                    print('  {}: {}'.format(field, json.dumps(text) if isinstance(text, bool)
                                            else text))
            found = True
    return found


# The functions gen_skeleton.py defines beside main(), which no header declares.
SCAFFOLD_NAMES = {
    'quit_with': 'void quit_with(int code);   // ends this program with that exit code',
    'is_quitting': 'bool is_quitting();         // true once quit_with() has run',
}

# The spellings agents write from memory, and what areg calls each one. These are the
# names 40-base-api.md lists as the ones most often borrowed from std::string and
# std::vector; check_contract.py reports each as B-08. A replacement is printed only
# after the headers are asked whether it still exists.
REPLACED_BY = {
    'get_str': 'as_string',
    'to_std_string': 'as_string',
    'c_str': 'as_string',
    'get_length': 'length',
    'push_back': 'add',
    'empty': 'is_empty',
    'insert': 'insert_at',
}


def noted_shapes(name):
    """The api.json member entries whose note names this word, as (entry, side, spec)."""
    try:
        with open(CONTRACT, encoding='utf-8') as handle:
            entries = json.load(handle).get('members') or []
    except (ValueError, OSError):
        return []
    word = re.compile(r'(?<![\w.]){}\b'.format(re.escape(name)))
    found = []
    for entry in entries:
        for side in ('provider', 'consumer'):
            spec = entry.get(side)
            if isinstance(spec, dict) and word.search(spec.get('note') or ''):
                found.append((entry, side, spec))
    return found


def answer_recalled(headers, name, full):
    """A camelCase name of areg 1.x: what api.json says replaced it, or the snake_case
    member the headers declare. False when neither knows it."""
    if not re.search(r'[a-z0-9][A-Z]', name) or '_' in name:
        return False
    noted = noted_shapes(name)
    if noted:
        declared = dict.fromkeys(entry['declared_as'] for entry, _, _ in noted)
        print('"{}" is gone; areg 1.x spelled members camelCase. docs/agent/api.json "{}" '
              'names what replaces it:'.format(name, '", "'.join(declared)))
        for _, side, spec in noted:
            for line in spec_line(side, spec):
                print(line)
        return True
    for instead in dict.fromkeys((check_contract.LEGACY_ACCESSORS.get(name), snake(name))):
        if instead and (declares(headers, instead) or catalogued(instead)):
            print('"{}" is gone; areg 1.x spelled members camelCase. areg declares "{}":'
                  .format(name, instead))
            print('')
            if not answer_member(headers, instead, full):
                print('docs/agent/members.json lists it; a macro declares it.')
            return True
    instead = snake(name)
    if any(pattern.match(instead) for pattern, _, _, _ in generated_shapes()):
        print('"{}" is gone; areg 1.x spelled members camelCase. Its snake_case form:'
              .format(name))
        print('')
        return answer_generated(instead)
    return False


def search(headers, word):
    """Every member, class, enumeration, enumerator and constant name holding a word.

    "areg::Timer(" and "Timer::start_timer()" are searched as their last name.
    """
    needle = re.sub(r'\(.*$', '', word.strip()).split('::')[-1].strip().lower()
    members, classes, enums, values, constants = set(), set(), set(), set(), set()
    for header in headers:
        for member, _, _, _ in header.members:
            if needle in member.lower():
                members.add(member)
        for owner in header.classes:
            if needle in owner.lower():
                classes.add(owner)
        for enum, enumerators in header.enums.items():
            if needle in enum.lower():
                enums.add(enum)
            values.update(enum + '::' + value for value in enumerators
                          if needle in value.lower())
        for constant in header.constants:
            if needle in constant.lower():
                constants.add(constant)
    for label, names in (('class', classes), ('enum', enums), ('enum value', values),
                         ('constant', constants), ('member', members)):
        if names:
            print('{}: {}'.format(label, ', '.join(sorted(names))))
    return bool(members or classes or enums or values or constants)


def generated_shapes():
    """(pattern, stand-in, spelled shape, entry) of each member name api.json declares.

    The most specific shape comes first, so request_foo_failed is tried before
    request_foo.
    """
    try:
        with open(CONTRACT, encoding='utf-8') as handle:
            entries = json.load(handle).get('members') or []
    except (ValueError, OSError):
        return []
    shapes = {}
    for entry in entries:
        for side in ('provider', 'consumer'):
            spec = entry.get(side)
            if not isinstance(spec, dict):
                continue
            names = spec.get('member')
            for name in [names] if isinstance(names, str) else names or []:
                parts = name.split('_')
                if '<' in name or '(' in name or all(p in STAND_INS for p in parts):
                    continue
                stand_in = next((p for p in parts if p in STAND_INS), None)
                pattern = '^{}$'.format('_'.join(r'(\w+?)' if p in STAND_INS
                                                 else re.escape(p) for p in parts))
                shapes.setdefault((pattern, stand_in, name), []).append((entry, side, spec))
    ordered = sorted(shapes.items(), key=lambda item: -len(item[0][2].replace(
        item[0][1] or '', '')))
    return [(re.compile(pattern), stand_in, name, found)
            for (pattern, stand_in, name), found in ordered]


def spec_line(side, spec):
    """One side of a generated member, as api.json states it."""
    facts = [spec.get('usage', '')]
    if 'required' in spec:
        facts.append('required' if spec['required'] else 'optional')
    if spec.get('pure_virtual'):
        facts.append('pure virtual')
    if spec.get('returns'):
        facts.append('returns ' + spec['returns'])
    if spec.get('parameters'):
        facts.append('parameters ' + ', '.join(spec['parameters']))
    if spec.get('requires_subscription'):
        facts.append('needs ' + spec['requires_subscription'])
    if spec.get('read_accessor'):
        facts.append('read by ' + spec['read_accessor'])
    out = ['  {}: {}'.format(side, '; '.join(fact for fact in facts if fact))]
    if spec.get('note'):
        out.append('    ' + spec['note'])
    return out


def answer_generated(name):
    """What a name the generator writes is, from docs/agent/api.json, and its lines in
    the project's worksheet."""
    for pattern, stand_in, shape, found in generated_shapes():
        match = pattern.match(name)
        if match:
            break
    else:
        return False
    element = ', {} = {}'.format(stand_in, match.group(1)) if stand_in else ''
    declared = dict.fromkeys(entry['declared_as'] for entry, _, _ in found)
    print('"{}" is not in the areg headers: the generator writes it for a service '
          'document. docs/agent/api.json "{}", shape {}{}:'
          .format(name, '", "'.join(declared), shape, element))
    for _, side, spec in found:
        for line in spec_line(side, spec):
            print(line)
    try:
        with open(WORKSHEET, encoding='utf-8') as handle:
            lines = [line.rstrip() for line in handle if name in line]
    except OSError:
        lines = []
    if lines:
        print('')
        print('{} names it:'.format(WORKSHEET))
        for line in lines[:4]:
            print('  ' + line.lstrip('#| ').strip())
    return True


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
        if name in DESIGN_CLASSES:
            cpp = DESIGN_CLASSES[name]
            print('"{}" is the design type; the generated C++ spells it {} ({}).'
                  .format(name, cpp, CLASS_HEADERS[cpp]))
            print('')
            if not answer_class(headers, cpp.split('::')[-1], args.full):
                missing += 1
            continue
        if args.as_class:
            answered = answer_class(headers, name, args.full)
        elif '::' in name and not args.enum:
            answered = answer_qualified(headers, name, args.full)
        elif args.enum:
            answered = answer_enum(headers, name) or answer_enumerator(headers, name)
        else:
            answered = (answer_member(headers, name, args.full)
                        or answer_class(headers, name, args.full)
                        or answer_enum(headers, name)
                        or answer_enumerator(headers, name)
                        or answer_constant(headers, name)
                        or answer_macro(headers, name))
        if not answered:
            if answer_generated(name):
                continue
            if name in SCAFFOLD_NAMES:
                print('"{}" is not in the areg headers: gen_skeleton.py writes it into '
                      'the main.cpp of every program it generates.'.format(name))
                print('')
                print('  {}'.format(SCAFFOLD_NAMES[name]))
                continue
            if catalogued(name):
                print('"{}" is a member of the framework -- docs/agent/members.json '
                      'lists it -- but no header declares it on a line this reader '
                      'can read, so it comes from a macro. It exists; call it.'
                      .format(name))
                continue
            seen = mentioned(headers, name)
            if seen:
                path, owner, member, signature = seen
                role = role_in(signature, name)
                where = qualified(owner) if owner else 'namespace areg'
                if role == 'parameter':
                    print('"{}" is the name of a parameter in the declaration below, '
                          'in {}. A parameter name is not a name to call: it is what '
                          'the declaration calls its own argument. Look up "{}" '
                          'instead.'
                          .format(name, path, member or owner or where))
                    missing += 1
                elif role == 'value':
                    print('"{}" appears in {} as a default argument of the '
                          'declaration below, so it is a value, not a call. Look it '
                          'up as a constant of the type it belongs to.'
                          .format(name, path))
                    missing += 1
                else:
                    print('"{}" is not indexed as a member, a class, an enumeration '
                          'or a constant. It appears in {}, in the declaration below, '
                          'as a type or a qualifier.'.format(name, path))
                print('')
                print('{}  ({})'.format(where, path))
                print('  {}'.format(signature))
                continue
            if answer_recalled(headers, name, args.full):
                missing += 1
                continue
            print('"{}" is not a member, a class, an enumeration, a constant or a macro in the '
                  'public areg headers, and appears in no declaration they carry.'
                  .format(name))
            instead = REPLACED_BY.get(name)
            # A replacement is only offered while the headers still declare it. The
            # map is a memory of what agents write, not a second source of truth.
            if instead and declares(headers, instead):
                print('It is a spelling borrowed from another library. areg calls it '
                      '"{}": look that up.'.format(instead))
            if not search(headers, name):
                print('No name holds it either.')
            print('An operator is listed by its class: api_help.py <Class> --class.')
            missing += 1
    return 1 if missing else 0


if __name__ == '__main__':
    sys.exit(main())
