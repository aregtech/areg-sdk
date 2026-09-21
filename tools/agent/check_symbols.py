#!/usr/bin/env python3
"""Check that every areg name the agent documentation writes still exists.

The agent pages and the recipes are read by a model that cannot tell a current
name from one it half remembers. A page that names something the framework no
longer declares is worse than a page that says nothing: the agent writes it,
the compiler rejects it, and the page is trusted again on the next task.

Nothing else in the suite answers this. `check_agent_docs.py` resolves paths,
`check_contract.py` reads application sources, and `check_recipes.py` needs a
compiler. This one reads the framework headers and the pages, and takes a
second.

    python3 tools/agent/check_symbols.py            # every page and every recipe
    python3 tools/agent/check_symbols.py --verbose  # list what resolved, too

Three things are checked:

  1. every `areg::<Name>` written in a page or a recipe is declared under
     framework/, as a type, an alias, a function, a constant or a namespace;
  2. every framework macro a code block invokes has a `#define`;
  3. every `<File.hpp>:<line>` citation names a file that exists, has that line,
     and declares on that line the member the citation is attached to.

Check 3 is what a framework edit rots first. A citation that only has to point
inside the file survives one inserted line at the top of a header, and survives
the member being renamed, because the old name is still written somewhere else in
the tree. So the cited line is read and the member's own name is looked for on it.
A citation attached to no name -- prose rather than a table row -- is still checked
for the file and the line only, and check 1 remains a tree-wide search for the
names no citation covers.

Two more edits leave the name on the line and change what it means, so the line
is read as code rather than as text:

  * comments are stripped before the name is looked for. A member deleted from
    the line and still named by the comment on it resolves against the comment
    otherwise;
  * the parameter list the page prints is compared with the one the line
    declares. A parameter added, removed or made mandatory keeps the name and
    changes every call the page teaches.

Exit code 0 means every name resolved.
"""

import argparse
import os
import re
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
ROOT = os.path.dirname(os.path.dirname(HERE))
FRAMEWORK = os.path.join(ROOT, 'framework')
DOC_DIR = os.path.join(ROOT, 'docs', 'agent')

HEADER_EXT = ('.hpp', '.hxx', '.hh', '.h')
SOURCE_EXT = HEADER_EXT + ('.cpp', '.cxx', '.cc')

QUALIFIED_RE = re.compile(r'\bareg::([A-Za-z_][A-Za-z0-9_]*)')
MACRO_USE_RE = re.compile(
    r'\b((?:AREG|LOG|DEF|BEGIN|END|REGISTER)_[A-Z0-9_]{2,})\s*\(')
# A citation is `File.hpp:120`, or `:120` alone when a row has already named the
# file. Both forms carry the line whose content is read back.
CITATION_RE = re.compile(r'`(?:([A-Za-z0-9_./-]+\.(?:hpp|h|cpp)))?:(\d+)`')
CODE_SPAN_RE = re.compile(r'`([^`]+)`')
FENCE_RE = re.compile(r'```[a-zA-Z]*\n(.*?)```', re.DOTALL)

# A macro that expands to another macro is still a definition, so a plain
# `#define NAME` prefix match is enough.
DEFINE_RE = re.compile(r'^\s*#\s*define\s+([A-Za-z_][A-Za-z0-9_]*)')

# What counts as declaring a name. Deliberately generous: this check exists to
# catch a name that is gone, not to police how a name is introduced.
DECL_RES = (
    re.compile(r'\b(?:class|struct|union|enum(?:\s+class)?|namespace)\s+'
               r'(?:[A-Z_]+_API\s+)?([A-Za-z_][A-Za-z0-9_]*)'),
    re.compile(r'\busing\s+([A-Za-z_][A-Za-z0-9_]*)\s*='),
    re.compile(r'\btypedef\b[^;]*?\b([A-Za-z_][A-Za-z0-9_]*)\s*;'),
    re.compile(r'\bconstexpr\b[^;=()]*?\b([A-Za-z_][A-Za-z0-9_]*)\s*(?:=|\{)'),
    re.compile(r'^\s*(?:[A-Z_]+_API\s+)?(?:const\s+)?'
               r'[A-Za-z_][A-Za-z0-9_:<>,&*\s]*?\b([a-z_][A-Za-z0-9_]*)\s*\('),
    re.compile(r'^\s*([A-Za-z_][A-Za-z0-9_]*)\s*(?:,|=|\})'),   # enumerator
)


def anchor_of(span):
    """The member name a code span is about, or None when it names nothing."""
    span = span.strip().rstrip(';')
    if 'operator' in span:
        return 'operator'
    called = re.search(r'\b([A-Za-z_][A-Za-z0-9_]*)\s*\(', span)
    if called:
        return called.group(1)
    names = re.findall(r'\b([A-Za-z_][A-Za-z0-9_]*)\b', span)
    return names[-1] if names else None


def declares(line, name):
    """Whether a header line declares the named member."""
    if name == 'operator':
        return 'operator' in line
    return re.search(r'\b' + re.escape(name) + r'\b', line) is not None


def code_only(lines):
    """The lines with their comments blanked, so a name in one counts for nothing."""
    out = []
    in_block = False
    for line in lines:
        kept = []
        index = 0
        while index < len(line):
            pair = line[index:index + 2]
            if in_block:
                if pair == '*/':
                    in_block = False
                    index += 2
                else:
                    index += 1
                continue
            if pair == '/*':
                in_block = True
                index += 2
                continue
            if pair == '//':
                break
            kept.append(line[index])
            index += 1
        out.append(''.join(kept))
    return out


def split_params(text):
    """The parameters of a list, splitting on the commas that separate them."""
    parts = []
    current = ''
    depth = 0
    for char in text:
        if char in '(<[{':
            depth += 1
        elif char in ')>]}':
            depth = max(0, depth - 1)
        if (char == ',') and (depth == 0):
            parts.append(current)
            current = ''
        else:
            current += char
    parts.append(current)
    return [part.strip() for part in parts if part.strip()]


def param_list(text, opening):
    """The text between the parenthesis at `opening` and its match, or None."""
    depth = 0
    for index in range(opening, len(text)):
        if text[index] == '(':
            depth += 1
        elif text[index] == ')':
            depth -= 1
            if depth == 0:
                return text[opening + 1:index]
    return None


def arity(text, name):
    """How many parameters `name` takes in `text`, as (required, total).

    Returns None when the list is variadic or is not there to read: a comparison
    that cannot be made is not a finding.
    """
    if name == 'operator':
        found = re.search(r'\boperator\b', text)
        opening = text.find('(', found.end()) if found else -1
    else:
        found = re.search(r'\b' + re.escape(name) + r'\s*\(', text)
        opening = text.index('(', found.start()) if found else -1
    if opening < 0:
        return None
    inner = param_list(text, opening)
    if inner is None:
        return None
    params = split_params(inner)
    if any(part == '...' or part.endswith('...') for part in params):
        return None
    if params == ['void']:
        params = []
    optional = [part for part in params if '=' in part]
    return len(params) - len(optional), len(params)


def declaration_at(lines, index):
    """The declaration starting on a header line, joined until its parentheses close."""
    text = lines[index]
    limit = min(len(lines), index + 6)
    while text.count('(') > text.count(')') and (index + 1) < limit:
        index += 1
        text += ' ' + lines[index]
    return text


def citations(line):
    """Every citation on a documentation line, as (file, number, anchor, span).

    The file carries across a row, so `ArrayList.hpp:172` followed by `:166`
    both resolve to ArrayList.hpp. The anchor is the last code span before the
    citation, which is the signature the row is citing.
    """
    found = []
    current = None
    for hit in CITATION_RE.finditer(line):
        named, number = hit.group(1), hit.group(2)
        if named:
            current = named
        if current is None:
            continue
        before = [s for s in CODE_SPAN_RE.finditer(line) if s.end() <= hit.start()]
        span = before[-1].group(1) if before else None
        anchor = anchor_of(span) if span else None
        found.append((current, int(number), anchor, span))
    return found


def framework_symbols():
    """Every name framework/ declares, its macros, and its files by base name."""
    declared = set()
    defines = set()
    by_name = {}
    for folder, folders, files in os.walk(FRAMEWORK):
        folders[:] = [d for d in folders if d not in ('.git', 'build')]
        for entry in files:
            if not entry.endswith(SOURCE_EXT):
                continue
            path = os.path.join(folder, entry)
            by_name.setdefault(entry, path)
            try:
                with open(path, 'r', encoding='utf-8', errors='replace') as handle:
                    text = handle.read()
            except OSError:
                continue
            for line in text.splitlines():
                found = DEFINE_RE.match(line)
                if found:
                    defines.add(found.group(1))
                    continue
                stripped = line.lstrip()
                if stripped.startswith(('*', '//', '/*')):
                    continue
                for pattern in DECL_RES:
                    hit = pattern.search(line)
                    if hit:
                        declared.add(hit.group(1))
    return declared, defines, by_name


def documents():
    """The pages and recipe sources whose names are checked."""
    found = [os.path.join(ROOT, 'AGENTS.md')]
    for folder, _folders, files in os.walk(DOC_DIR):
        for entry in sorted(files):
            if entry.endswith('.md') or entry.endswith(SOURCE_EXT):
                found.append(os.path.join(folder, entry))
    return [p for p in found if os.path.isfile(p)]


def code_of(path, text):
    """The part of a document that claims to be code."""
    if path.endswith('.md'):
        return '\n'.join(FENCE_RE.findall(text))
    return text


def check(verbose):
    declared, defines, by_name = framework_symbols()
    if not declared:
        print('no framework sources found under %s' % FRAMEWORK, file=sys.stderr)
        return 2

    problems = []
    resolved = 0
    sources = {}
    for path in documents():
        shown = os.path.relpath(path, ROOT)
        with open(path, 'r', encoding='utf-8', errors='replace') as handle:
            text = handle.read()
        code = code_of(path, text)

        for name in sorted(set(QUALIFIED_RE.findall(text))):
            if name in declared:
                resolved += 1
                if verbose:
                    print('  ok   areg::%-28s %s' % (name, shown))
            else:
                problems.append('%s: areg::%s is declared nowhere under framework/'
                                % (shown, name))

        for name in sorted(set(MACRO_USE_RE.findall(code))):
            if name in defines:
                resolved += 1
                if verbose:
                    print('  ok   %-34s %s' % (name, shown))
            else:
                problems.append('%s: macro %s has no #define under framework/'
                                % (shown, name))

        for doc_line in text.splitlines():
            for target, number, anchor, span in citations(doc_line):
                base = os.path.basename(target)
                hit = by_name.get(base)
                if hit is None:
                    continue    # a path citation, not a framework one
                if hit not in sources:
                    with open(hit, 'r', encoding='utf-8', errors='replace') as handle:
                        sources[hit] = code_only(handle.read().splitlines())
                lines = sources[hit]
                if number > len(lines):
                    problems.append('%s: %s:%d is past the end of the file (%d lines)'
                                    % (shown, base, number, len(lines)))
                    continue
                cited = lines[number - 1]
                if anchor is not None and not cited.strip():
                    problems.append(
                        '%s: %s:%d carries no code, only a comment or nothing; %s is '
                        'declared elsewhere or not at all'
                        % (shown, base, number, anchor))
                    continue
                if anchor is not None and not declares(cited, anchor):
                    problems.append(
                        '%s: %s:%d no longer declares %s; the line reads "%s"'
                        % (shown, base, number, anchor, cited.strip()[:70]))
                    continue
                if anchor is not None:
                    written = arity(span, anchor)
                    declared_arity = arity(declaration_at(lines, number - 1),
                                           anchor)
                    if (written is not None) and (declared_arity is not None) \
                            and (written != declared_arity):
                        problems.append(
                            '%s: %s:%d declares %s with %d parameter(s), %d of them '
                            'mandatory; the page prints %d and %d'
                            % (shown, base, number, anchor, declared_arity[1],
                               declared_arity[0], written[1], written[0]))
                        continue
                resolved += 1
                if verbose:
                    print('  ok   %-34s %s' % ('%s:%d' % (base, number), shown))

    for problem in problems:
        print('error: ' + problem)
    print('%d documented names checked, %d unresolved'
          % (resolved + len(problems), len(problems)))
    return 1 if problems else 0


def main():
    parser = argparse.ArgumentParser(
        description='Check documented areg names against the framework.')
    parser.add_argument('--verbose', action='store_true',
                        help='list the names that resolved as well')
    args = parser.parse_args()
    return check(args.verbose)


if __name__ == '__main__':
    sys.exit(main())
