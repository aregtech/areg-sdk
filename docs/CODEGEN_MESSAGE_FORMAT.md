# Diagnostic format of the code generator

This is a specification for `codegen.jar`, written to be handed to the code generator
repository. It is not on the agent path: an agent building an application on top of
areg never reads it. It exists because the messages the generator prints are the only
thing standing between a broken `.siml` document and a fix, for a human and for an
agent alike.

Nothing here changes what the generator validates. It changes only how a finding is
reported.

---

## 1. What is wrong with the current output

A real refusal today looks like this:

```
Sat Aug 22 21:19:02 CEST 2026 ERROR [ tech.areg.si.CMFServiceInterfaceGenerator ]: Not implemented by this generator: a data type that resolves to nothing -- the parameter [ value ] of the request [ processData ] is declared as [ Float ] at [ TestService.siml:11:66 ]
Sat Aug 22 21:19:02 CEST 2026 ERROR [ tech.areg.si.CMFServiceInterfaceGenerator ]: The service interface [ TestService ] has 1 fault(s) this generator refuses. Nothing is generated: an approximated service interface is worse than none.
Sat Aug 22 21:19:02 CEST 2026 ERROR [ tech.areg.CMFMain ]: Nothing was generated from the document [ product\TestService.siml ]
```

Much of this is right, and section 2 lists what must survive any reformat. Four things
are wrong.

### 1.1 The rule number and rule name are absent

This is the one defect that matters more than the rest together.

The finding above is rule **6**, `RULE_UNRESOLVED_TYPE`, in `tools/schema/rules.xml`.
The message never says so. The registry of 61 rules, the `explain_rule.py` tool that
reads it and the same help text inside Lusan are all keyed by the number, and the
number is exactly what the generator does not print. The user is told a thing is
wrong and given no way to reach the paragraph that explains it.

Matching on the message text instead does not work, and the attempt shows why. The
generator says *a data type that resolves to nothing*. The registry says *a declared
data type that answers to nothing*. The two were written by the same project and still
do not share a verb. `explain_rule.py --search` therefore has to fall back to scoring
words separately, which returns a best guess rather than an answer.

Prose drifts. A number does not. Print the number.

### 1.2 The originating class is noise

`tech.areg.si.CMFServiceInterfaceGenerator` names an implementation detail of the
generator. It tells the reader nothing about their own mistake, it is repeated on
every line, and for an agent it is roughly forty tokens of pure cost per finding.

It belongs in the log file, where it is genuinely useful, and nowhere else.

### 1.3 A full timestamp on every console line

Same cost, plus a second effect: it makes the output non-deterministic, so the
generator's own behaviour cannot be covered by a golden-output test, and two runs of
the same broken document cannot be compared with `diff`.

### 1.4 No remedy

`Float` is a capitalisation mistake for `float`. The generator holds the type registry
and can see that. The message stops at naming the fault, so a reader who does not
already know the type names has to go and find them, and an agent burns a turn
guessing.

---

## 2. What must not be lost

The current output does several things that most code generators do not. Any reformat
has to keep all of them.

| Behaviour | Why it matters |
|---|---|
| `file:line:col` on the finding | It is what makes the fault navigable at all |
| The construct named with its path: parameter `value` of request `processData` | A line number alone is ambiguous inside a nested document |
| The refusal explained: "an approximated service interface is worse than none" | Tells the reader the empty output directory is intended, not a second bug |
| A count of faults | Sets the expectation that fixing one may not be enough |
| A closing line naming the document that produced nothing | The only line that survives in a long multi-document build log |

---

## 3. The console format

```
TestService.siml:11:66: error[6/RULE_UNRESOLVED_TYPE]: data type 'Float' resolves to nothing
  in request 'processData', parameter 'value'
  fix: did you mean 'float'? or declare 'Float' in a .dtml document
TestService.siml: 1 error, 0 warnings -- nothing generated
```

### 3.1 The first line

```
<path>:<line>:<col>: <severity>[<number>/<NAME>]: <what is wrong>
```

`<path>:<line>:<col>: <severity>: <message>` is the diagnostic convention of GCC,
Clang and MSVC. Adopting it is worth more than it costs, because the surrounding
ecosystem already parses it:

- Visual Studio Code, Visual Studio and any editor with a problem matcher turn the
  finding into a clickable entry with no configuration.
- `vim` and `emacs` populate the quickfix and compilation buffers.
- GitHub Actions annotates the pull request through the standard matcher.
- Every agent already knows this shape from every compiler it has ever read.

`<severity>` is `error`, `warning` or `info`.

`<number>` is the number **as reported**, that is with its band offset applied: bare
for an error, plus 100 for a warning, plus 200 for information. It must be the value
that can be pasted straight into `explain_rule.py` with no arithmetic.

`<NAME>` is the `Name` attribute from `rules.xml`. It carries the meaning for a reader
who will not look the number up, and it is greppable across a build log.

`<what is wrong>` is one clause, lower case, no trailing period. Name the offending
text in single quotes.

### 3.2 The context line

```
  in request 'processData', parameter 'value'
```

Two spaces of indent. The path from the nearest named ancestor down to the fault, in
that order. Omit the line entirely when the fault is on a top-level element and the
first line already names it.

### 3.3 The remedy line

```
  fix: did you mean 'float'? or declare 'Float' in a .dtml document
```

Two spaces of indent, prefix `fix:`. This is the highest-value addition after the rule
number. Emit it whenever the generator can compute a candidate:

| Situation | Remedy the generator can compute |
|---|---|
| An unresolved type close to a known one | `did you mean 'float'?` -- edit distance over the type registry, case-insensitive first |
| An unresolved type close to nothing | `declare it in a .dtml document, or use one of: bool, int32, float, String, ...` |
| A duplicate name | `rename this one; the first is at TestService.siml:8:20` |
| An invalid identifier | `an identifier starts with a letter or underscore and continues with letters, digits or underscores` |
| A reference to an element that does not exist | the nearest existing name of the right kind |

Where nothing can be computed, omit the line. A generic remedy is worse than none: it
trains the reader to skip it.

### 3.4 The summary line

```
TestService.siml: 1 error, 0 warnings -- nothing generated
```

One per document, always, including on success:

```
TestService.siml: 0 errors, 0 warnings -- 6 files written
```

Close a run over several documents with a total:

```
3 documents, 1 refused, 1 error, 2 warnings
```

### 3.5 What the console must not print

No timestamp. No originating class. No banner, no version line, no
"generation started" unless `--verbose` was given. A clean run over a valid document
should print its summary line and nothing else.

---

## 4. The console and the log file are different documents

This is the change that removes the verbosity, and it costs almost nothing to
implement: the two sinks get different layouts.

| | Console | Log file |
|---|---|---|
| Timestamp | none | per line, ISO 8601 |
| Originating class | none | yes |
| Layout | the compiler convention of section 3 | free, the current layout is fine |
| Detail | the finding and its remedy | everything, including what was suppressed |
| Determinism | required | not required |

Today both sinks carry the log file's payload. That is the whole reason the output
reads as expensive.

ISO 8601 in the log file, `2026-08-22T21:19:02+02:00`, rather than
`Sat Aug 22 21:19:02 CEST 2026`: sortable, unambiguous across locales, and parsed by
every log tool without configuration.

---

## 5. `--format json`

A machine-readable mode is what makes an agent reliable rather than lucky. Regular
expressions over prose break on the first rewording; a JSON document does not.

```json
{
  "tool": "codegen",
  "version": "2.1.0",
  "rulesFormatVersion": "1.0.0",
  "documents": [
    {
      "path": "product/TestService.siml",
      "generated": false,
      "files": [],
      "findings": [
        {
          "rule": 6,
          "name": "RULE_UNRESOLVED_TYPE",
          "severity": "error",
          "line": 11,
          "column": 66,
          "message": "data type 'Float' resolves to nothing",
          "found": "Float",
          "suggest": ["float"],
          "element": {
            "kind": "parameter",
            "name": "value",
            "parent": { "kind": "request", "name": "processData" }
          }
        }
      ]
    }
  ],
  "summary": { "errors": 1, "warnings": 0, "documents": 1, "refused": 1 }
}
```

Rules for the mode:

- Everything goes to standard output; nothing else goes to standard output. Progress
  and internal errors go to standard error, so the stream always parses.
- `rule` is the **base** number and `severity` is separate. The console concatenates
  them; the JSON does not, because a consumer should not have to subtract 100.
- `suggest` is an array, possibly empty, never absent.
- The document is emitted even when everything succeeded.

Two further modes are cheap once this exists and worth having:

- `--format github` for `::error file=...,line=...,col=...::message`.
- `--format short` for one line per finding and no context or remedy.

---

## 6. Exit codes

| Code | Meaning |
|---|---|
| 0 | Everything generated |
| 1 | Refused by validation; the documents are at fault |
| 2 | Bad usage: an unknown flag, a missing argument, a file that is not there |
| 3 | Internal error; the generator is at fault |

The distinction between 1 and 3 is what tells an agent whether to correct the document
or to report a defect. A single non-zero code for both makes that undecidable, and an
agent that guesses wrong will edit a correct document until it gives up.

---

## 7. Two stability contracts

**Rule numbers are permanent.** Never renumber, never reuse. A rule that is withdrawn
is marked withdrawn and its number stays spent. Numbers appear in log files, in issue
reports, in this repository's documentation and in the memory of every model that has
read it.

**Message text is API.** Once `docs/agent/51-debug.md` keys a lookup table on a
substring, and once agents learn a phrase, rewording it silently breaks them. Change
the wording only with the rule number, and prefer adding to the remedy line over
rewriting the first line.

---

## 8. Discoverability

Two small omissions keep the validator invisible.

**`--help` says nothing about validation.** Nor does the `ReadMe.txt` bundled in the
jar, which documents every flag and never mentions that a document is checked before
it is generated. A reader who has not hit a refusal does not know the feature exists.
Add a short Validation section to both, naming the registry and `explain_rule.py`.

Keep `ReadMe.txt` in the jar; it is the only documentation that travels with the
artifact. Edit it in the code generator repository, though, not in this one, where the
jar is a build output and a hand edit is lost on the next rebuild.

**`--version` should print the rules format version** alongside the generator version.
`rules.xml` in this repository carries `FormatVersion`, and a jar built against a
different revision of the registry is otherwise undetectable.
