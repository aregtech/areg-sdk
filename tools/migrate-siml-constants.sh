#!/bin/sh
#
# Migrates <Constant> elements to the attribute form for Value and DeprecateHint.
#
# Two changes, applied to every .siml, .fsml and .dtml file under the chosen root:
#
#   1. Inside a <Constant> element, a <Value>text</Value> child becomes a Value="text"
#      attribute on the <Constant> start tag.
#   2. Inside a <Constant> element, a <DeprecateHint>text</DeprecateHint> child becomes a
#      DeprecateHint="text" attribute on the <Constant> start tag.
#
# Only <Constant> is touched. The other elements that carry a deprecation hint convert with
# their own editor page, so that each change ships with the code that reads and writes it.
#
# <Description> is never touched: it is free-form, possibly multi-line prose, and XML
# attribute-value normalization would collapse its newlines. <Value> children of Field,
# Parameter and EnumEntry elements are left alone; those carry an IsDefault attribute of their
# own, so they are structural and are handled separately.
#
# The rewrite is line oriented, not a parse-and-serialize round trip. Only the lines that
# actually change are rewritten, so the XML declaration, the indentation, the line endings and
# every untouched byte of the file survive exactly as they were. Files that need no change are
# not written at all, so the script is safe to run twice.
#
# migrate-siml-constants.ps1 beside this file applies the same transformation and produces
# byte-identical results.
#
# Usage:
#   ./tools/migrate-siml-constants.sh [--root DIR] [--dry-run]
#
# With no --root the script asks on the console and offers the parent of its own directory,
# which is the repository root when the script sits in tools/.
#
# Requires only a POSIX shell, awk and find.

set -eu

root=""
pretend=0

usage()
{
    cat <<'USAGE'
Migrates <Constant> elements of .siml, .fsml and .dtml documents to the attribute form:
a <Value> child becomes Value="...", a <DeprecateHint> child becomes DeprecateHint="...".

Usage: migrate-siml-constants.sh [--root DIR] [--dry-run]

  --root DIR   directory to search. Without it the script asks on the console and offers
               the parent of its own directory.
  --dry-run    report what would change and write nothing. -n does the same.
  --help       this text.
USAGE
    exit "${1:-0}"
}

while [ $# -gt 0 ]
do
    case "$1" in
        --root)
            [ $# -ge 2 ] || { echo "Missing argument for --root" >&2; exit 2; }
            root="$2"
            shift 2
            ;;
        --root=*)
            root="${1#--root=}"
            shift
            ;;
        --dry-run|-n)
            pretend=1
            shift
            ;;
        -h|--help)
            usage 0
            ;;
        *)
            echo "Unknown argument: $1" >&2
            usage 2
            ;;
    esac
done

script_dir=$(CDPATH= cd -- "$(dirname -- "$0")" && pwd)
default_root=$(CDPATH= cd -- "$script_dir/.." && pwd)

if [ -z "$root" ]
then
    if [ -t 0 ]
    then
        printf 'Root directory of the project [%s]: ' "$default_root"
        IFS= read -r answer || answer=""
    else
        answer=""
    fi

    if [ -z "$answer" ]
    then
        root="$default_root"
    else
        root="$answer"
    fi
fi

if [ ! -d "$root" ]
then
    echo "Root not found or not a directory: $root" >&2
    exit 1
fi

root=$(CDPATH= cd -- "$root" && pwd)
echo "Root: $root"

tmp_dir=$(mktemp -d 2>/dev/null || mktemp -d -t migsiml)
trap 'rm -rf "$tmp_dir"' EXIT INT TERM

out_file="$tmp_dir/out"
stats_file="$tmp_dir/stats"

# The awk program. Byte oriented, so LC_ALL=C below keeps multibyte locales from reinterpreting
# the input.
awk_program='
function trim(s)
{
    sub(/^[ \t\r]+/, "", s)
    sub(/[ \t\r]+$/, "", s)
    return s
}

function escape_attr(s)
{
    # The captured text is already escaped for element content, so & and < are intact. An
    # attribute delimited with double quotes needs the quote escaped as well, and a tab escaped
    # or attribute-value normalization would silently turn it into a space.
    gsub(/"/, "\\&quot;", s)
    gsub(/\t/, "\\&#9;", s)
    return s
}

function tag_end(s, start,    i, c, q, len)
{
    # Index of the ">" that closes a start tag beginning at start, or 0 while it is unclosed.
    # Quoted attribute values may contain ">", so track the quote state.
    len = length(s)
    q = ""
    for (i = start; i <= len; i++)
    {
        c = substr(s, i, 1)
        if (q != "")
        {
            if (c == q) { q = "" }
            continue
        }
        if (c == "\"" || c == "'"'"'") { q = c; continue }
        if (c == ">") { return i }
    }
    return 0
}

function emit(s)
{
    res[++m] = s
}

function warn(text)
{
    printf "%s: %s\n", path, text > "/dev/stderr"
    warnings++
}

{ line[++n] = $0 }

END {
    i = 1
    m = 0
    value_moved = 0
    hint_moved = 0
    warnings = 0

    while (i <= n)
    {
        t = trim(line[i])

        # The trailing class keeps <ConstantList> from reading as a <Constant>.
        if (t !~ /^<Constant([ \t\/>]|$)/)
        {
            if (line[i] ~ /<Constant[ \t\/>]/)
            {
                warn("line " i ": a <Constant> start tag that does not begin its line was left alone")
            }
            emit(line[i])
            i++
            continue
        }

        # Collect the start tag, which may in principle span lines.
        joined = ""
        j = i
        te = 0
        while (j <= n)
        {
            joined = (j == i) ? line[j] : joined "\n" line[j]
            te = tag_end(joined, index(joined, "<Constant"))
            if (te > 0) { break }
            j++
        }

        if (te == 0)
        {
            warn("line " i ": unterminated <Constant> start tag, left alone")
            emit(line[i])
            i++
            continue
        }

        if (substr(joined, te - 1, 1) == "/")
        {
            # Self-closing: no children to move.
            for (k = i; k <= j; k++) { emit(line[k]) }
            i = j + 1
            continue
        }

        # Walk the body to the matching close tag, noting where the two children sit. Every line
        # is kept, so any path that decides not to convert can emit the element untouched.
        body_count = 0
        value_index = 0
        hint_index = 0
        value_text = ""
        hint_text = ""
        duplicate = ""
        k = j + 1
        closed = 0
        malformed = 0

        while (k <= n)
        {
            t = trim(line[k])

            if (t == "</Constant>") { closed = 1; break }
            if (t ~ /^<Constant([ \t\/>]|$)/) { malformed = 1; break }

            captured = 0

            if (t ~ /^<Value>.*<\/Value>$/)
            {
                if (value_index > 0) { duplicate = "Value" }
                value_text = substr(t, 8, length(t) - 15)
                value_index = body_count + 1
                captured = 1
            }
            else if (t ~ /^<Value[ \t]*\/>$/)
            {
                if (value_index > 0) { duplicate = "Value" }
                value_text = ""
                value_index = body_count + 1
                captured = 1
            }
            else if (t ~ /^<DeprecateHint>.*<\/DeprecateHint>$/)
            {
                if (hint_index > 0) { duplicate = "DeprecateHint" }
                hint_text = substr(t, 16, length(t) - 31)
                hint_index = body_count + 1
                captured = 1
            }
            else if (t ~ /^<DeprecateHint[ \t]*\/>$/)
            {
                if (hint_index > 0) { duplicate = "DeprecateHint" }
                hint_text = ""
                hint_index = body_count + 1
                captured = 1
            }

            if (!captured)
            {
                if (t ~ /^<Value[ \t>]/ || t == "<Value")
                {
                    # Opens here and closes on a later line. Converting it would fold a newline
                    # into an attribute, where normalization turns it into a space. Report, do
                    # not guess.
                    warn("line " k ": a multi-line <Value> was left alone")
                }
                else if (t ~ /^<DeprecateHint[ \t>]/ || t == "<DeprecateHint")
                {
                    warn("line " k ": a multi-line <DeprecateHint> was left alone")
                }
            }

            body[++body_count] = line[k]
            k++
        }

        if (malformed || !closed)
        {
            warn("line " i ": <Constant> without a matching close tag, left alone")
            for (p = i; p <= j; p++) { emit(line[p]) }
            i = j + 1
            continue
        }

        start_tag = substr(joined, 1, te - 1)
        refuse = ""
        if (duplicate != "")
        {
            refuse = "carries more than one <" duplicate "> child"
        }
        else if (value_index > 0 && start_tag ~ /[ \t\n]Value[ \t]*=/)
        {
            refuse = "carries both a Value attribute and a <Value> child"
        }
        else if (hint_index > 0 && start_tag ~ /[ \t\n]DeprecateHint[ \t]*=/)
        {
            refuse = "carries both a DeprecateHint attribute and a <DeprecateHint> child"
        }

        if (refuse != "")
        {
            warn("line " i ": <Constant> " refuse ", left alone")
        }

        if (refuse != "" || (value_index == 0 && hint_index == 0))
        {
            for (p = i; p <= j; p++) { emit(line[p]) }
            for (p = 1; p <= body_count; p++) { emit(body[p]) }
            emit(line[k])
            i = k + 1
            continue
        }

        attributes = ""
        if (value_index > 0)
        {
            attributes = attributes " Value=\"" escape_attr(value_text) "\""
            value_moved++
        }
        if (hint_index > 0)
        {
            attributes = attributes " DeprecateHint=\"" escape_attr(hint_text) "\""
            hint_moved++
        }

        rewritten = substr(joined, 1, te - 1) attributes substr(joined, te)
        count = split(rewritten, parts, "\n")
        for (p = 1; p <= count; p++) { emit(parts[p]) }
        for (p = 1; p <= body_count; p++)
        {
            if (p == value_index || p == hint_index) { continue }
            emit(body[p])
        }
        emit(line[k])
        i = k + 1
    }

    for (p = 1; p <= m; p++)
    {
        if (p < m || final_nl == 1) { printf "%s\n", res[p] } else { printf "%s", res[p] }
    }

    printf "%d %d %d\n", value_moved, hint_moved, warnings > stats
}
'

scanned=0
changed=0
total_value=0
total_hint=0
total_warnings=0

# Generated trees hold copies of the sources; rewriting those would be pointless and slow.
files=$(find "$root" \
        \( -name .git -o -name build -o -name product -o -name out -o -name node_modules \) -prune \
        -o -type f \( -name '*.siml' -o -name '*.fsml' -o -name '*.dtml' \) -print | LC_ALL=C sort)

# A newline-separated list; document paths in this tree never contain one.
IFS='
'
for file in $files
do
    scanned=$((scanned + 1))

    # awk drops the record separator, so tell it whether to put the last newline back.
    if [ -s "$file" ] && [ "$(tail -c 1 -- "$file" | od -An -tu1 | tr -d ' \n')" = "10" ]
    then
        final_nl=1
    else
        final_nl=0
    fi

    # Fed on stdin, so a path holding '=' cannot be mistaken for a variable assignment.
    # BINMODE stops the Windows build of gawk from silently dropping the CR of every CRLF
    # line. Every other awk ignores the variable.
    LC_ALL=C awk -v BINMODE=3 -v path="$file" -v stats="$stats_file" -v final_nl="$final_nl" \
        "$awk_program" < "$file" > "$out_file"

    # IFS is a newline for the file loop; the stats line is space separated.
    IFS=' ' read -r v h w < "$stats_file"
    total_warnings=$((total_warnings + w))

    if [ "$((v + h))" -eq 0 ]
    then
        continue
    fi

    changed=$((changed + 1))
    total_value=$((total_value + v))
    total_hint=$((total_hint + h))

    if [ "$pretend" -eq 1 ]
    then
        printf 'would change  %s  (Value: %d, DeprecateHint: %d)\n' "$file" "$v" "$h"
        continue
    fi

    printf 'changed  %s  (Value: %d, DeprecateHint: %d)\n' "$file" "$v" "$h"
    cat "$out_file" > "$file"
done
unset IFS

echo ''
echo "Files scanned: $scanned"
echo "Files changed: $changed"
echo "Value attributes written: $total_value"
echo "DeprecateHint attributes written: $total_hint"
if [ "$total_warnings" -gt 0 ]
then
    echo "Warnings: $total_warnings"
fi

if [ "$pretend" -eq 1 ]
then
    echo ''
    echo '*** DRY RUN: nothing was written. Run again without --dry-run to apply. ***'
fi
