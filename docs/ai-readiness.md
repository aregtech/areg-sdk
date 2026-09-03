# The agent corpus: the rules it must hold

This page is for maintainers of the SDK, not for agents building applications. An
agent building an application reads `AGENTS.md` and stops there.

The repository claims to be usable by an AI agent. Left as narrative review --
someone reads the documentation, tries a task, writes down an impression -- that
claim cannot be regressed. A later change that quietly removes a page, doubles the
entry toll, or breaks the one example a feature has shows up nowhere.

`tools/agent/check_corpus.py` replaces the impression with a set of rules. It reads
the repository and reports which rules do not hold. It is deterministic: same tree,
same findings, on any machine, with no network and no build.

```
python3 tools/agent/check_corpus.py             # the findings
python3 tools/agent/check_corpus.py --verbose   # the rules that hold, as well
python3 tools/agent/check_corpus.py --json      # the same, for CI
python3 tools/agent/check_corpus.py --strict    # a warning fails the run too
```

Three severities. `FAIL` is a rule that must hold and does not, and it fails the
run. `WARN` is a measurement past a declared target; it fails the run only under
`--strict`. `NOTE` is a recorded exception or an observation, always printed and
never failing, so an argued exception stays visible instead of disappearing.

---

## 1. Why there is no score

An earlier version of this instrument summed these rules into three numbers out of
100. It no longer does, for three reasons, and the reasons are worth keeping:

- **A total needs weights, and a weight is a judgement wearing an instrument's
  clothes.** Whether feature coverage is worth 30 and prohibition detectability 10
  is not derived from anything. The measurements underneath are facts; the ratio
  between them was an opinion, and printing it as a number lent it an authority it
  had not earned.
- **Partial credit hid the finding.** A page over its target still earned most of
  the points for that rule, and the result read as success. The overage is the fact;
  the fraction of a point deducted for it was invented.
- **A moved total says something broke without saying what.** A total that drops
  sends a maintainer looking. `FAIL budget docs/agent/51-debug.md is over the
  ceiling` sends them to the page.

The catalogue in section 5 is still taken from the scope statement of `AGENTS.md`,
so these rules still grade the repository against its own declaration of what it
set out to do. That is a real limit and no rule here removes it: **a corpus can
satisfy every rule below and still fail an agent.** What closes that gap is section
2, not another rule.

## 2. What these checks cannot see

The rules are written by the same people they grade, so the blind spots are recorded
here rather than left to be discovered:

- **No model is in the loop.** `run_evals.py --self-check` grades the reference
  answers, not an agent's. Whether a cold model actually succeeds is measured by
  running one against `tools/agent/evals/tasks.json`, and nothing here substitutes
  for that. It is slow, costs money and is not deterministic, so it is never a gate
  -- and it is the only measure not graded by the hand that wrote the corpus.
- **Nothing is built.** Where a property can only be established by execution, what
  is checked is whether CI runs the check, not whether the check passes today. That
  is the honest static proxy, and it is the property that protects the repository
  over time.
- **The entry toll is measured as a file size, not as the bytes read before an agent
  can route.** Moving the routing table above the installation and golden-path
  sections cut the bytes an agent reads before it can route, and changed the
  measurement by nothing. The proxy is cheap and stable, and it is a proxy.
- **`check_commands.py` resolves every documented command, runs only the read-only
  ones, and checks every flag against what the tool's own `--help` advertises.** The
  rest build, start or change something. A flag the tool no longer accepts is now
  caught whether the command runs or not, but a command that resolves, carries only
  known flags and still does not work is the class of defect that has cost the most
  time in past audits.
- **The include check reads names, not a compiler.** It asks whether every `areg::`
  name and macro a page's code uses is reachable from an include that page shows. It
  cannot tell whether the code would otherwise compile.

## 3. Can an agent find a correct answer

| Check | Rule | Severity |
|---|---|---|
| `coverage` | Every feature in the catalogue of section 5 is answered by a page. | `FAIL` |
| `coverage` | ... and demonstrated by a recipe or an example, and graded by an eval task. | `WARN` |
| `claim` | Every self-claim the documentation makes about itself holds, checked literally against the tree. | `FAIL` |
| `paths` | Every documented path resolves (`check_agent_docs.py` exits zero). | `FAIL` |
| `reachable` | Every page in `docs/agent/` is named by the task table in `AGENTS.md`, so something routes an agent to it. | `FAIL` |
| `includes` | Every `areg::` name and macro a page's C++ uses is reachable from an include that page shows. A page whose code cannot compile as written is followed before it is doubted. A block that is a signature list or a contrast rather than code to copy opts out with `// areg-check: ignore` on its first line. | `FAIL` |
| `members` | Every generated member a page names -- `request_*`, `response_*`, `broadcast_*`, `notify_on_*`, `on_*_update` -- matches a shape `docs/agent/api.json` declares, in prose as well as in code. `check_symbols.py` anchors `areg::` names to a header; these carry no namespace and exist in no header until a build runs, so nothing else answers for them. | `FAIL` |
| `data-types` | `21-data-types.md` gives every predefined type in `tools/schema/datatype.xml` its C++ spelling. Four are not named the way the document names them -- `BinaryBuffer` is `areg::SharedBuffer`, `Array` is `areg::ArrayList`, `Map` is `areg::OrderedMap`, `Pair` is `areg::KeyValuePair` -- and every override signature an agent writes needs the right one. | `FAIL` |
| `stated` | Every measure a document states -- the page ceiling, the entry target, the median target, the duplication target, the size of the rule registry -- matches the file that decides it. A number copied into a page cannot be edited by whoever changes the value, so it is the first thing to go stale, and a page that misstates a threshold is read as the rule. | `FAIL` |
| `shipped` | The tools an installation puts in `tools/areg/` are the tools the pages name, and no others. A named tool the install omits is a dead instruction; a maintenance script that ships is one more thing an agent must rule out. | `FAIL` |
| `executed` | The documented **instructions** are executed by a checker in CI rather than read: the golden path builds a scaffolded project, the cheat sheet's configuration block is run and asserted to log, the recipes compile and print, the router-collector-`.sqlog` path runs, the diagnostics the pages promise still fire, and a recipe is compiled by the Windows toolchain. A block of configuration cannot be checked by reading it. | `WARN` |
| `recipe` | Every recipe's output is asserted by something, and CI builds and runs them. Built but never run is not verification. | `FAIL` |
| `eval` | Every eval task has a reference implementation, so the harness can prove the task is solvable. | `WARN` |
| `eval` | The bank holds repair tasks. An agent that can only build from nothing is not ready: most of the work is repair. | `FAIL` |
| `prohibition` | Every prohibition in `docs/agent/api.json` names a rule `check_contract.py` implements, **and** a repair task proves that rule still fires. Implemented is not working, and the same hand wrote both lists. | `FAIL` |

## 4. Is the cost justified

| Check | Rule | Severity |
|---|---|---|
| `entry-toll` | `AGENTS.md` at or below 10 KB. Every agent pays this on every task, so it is the most expensive byte in the repository. | `WARN` |
| `budget` | Every page in `docs/agent/` at or below 8 KB. | `FAIL` |
| `budget` | A page deliberately larger is named in `docs/agent/.budgets` with its reason. Recorded, so the exception is argued rather than forgotten. | `NOTE` |
| `budget` | An entry in `.budgets` naming a page that does not exist, or one already under the ceiling, is stale: it would quietly cover a page that grew into it later. | `FAIL` |
| `budget` | A page within 1% of the ceiling is tuned to the rule, not written to it: the next one-word edit trips CI. | `NOTE` |
| `budget` | Median page at or below 6 KB. | `WARN` |
| `duplication` | Under 2% of 12-word runs repeated across three or more pages. | `WARN` |
| `generated` | The rule against editing generated code is stated on the entry path, and no recipe ships generated code to be read. | `FAIL` |

## 5. Does it stay true

| Check | Rule | Severity |
|---|---|---|
| `ci-gate` | Nine gates run on every change: documented paths, contract on recipes, recipes build and run, project setup, eval self-check, the corpus check, mutation testing, the observability path, and a non-Linux runner. A gate under `continue-on-error` reports and never fails a run, which is not a gate. | `FAIL` |
| `sdk-paths` | `conf/cmake/setup.cmake` writes `build/areg-sdk.paths`, and `setup_project.py` names every key it writes. The SDK lands in a different place for a clone, a fetch and an installed package; this file is the one answer, and a key renamed on one side alone is silent until an agent reads for something absent. | `FAIL` |
| `fetch-ref` | Every `GIT_TAG` in the twelve recipes and in `10-new-project.md`, and the fallback of `setup_project.py`, equals `sdk.fetch_ref` in `docs/agent/api.json`, and the page that writes the pin states the supported range. A recipe copied by hand has to build with no lookup, so the ref is a literal in fourteen places; they all change on the day a release is tagged, and one missed is a project fetching an API the pages do not describe. | `FAIL` |
| `tool` | Every tool `AGENTS.md` names exists. A named tool that is absent is a dead instruction, and an agent follows it before it discovers that. | `FAIL` |
| `tool` | `explain_rule.py` reads its registry, and every rule says both what is wrong and what to change. What is checked is what the tool prints, not what one of its inputs holds. | `WARN` |
| `observability` | A checker starts the router, collects logs with `logcollector`, queries the `.sqlog` database it writes, and runs in CI. This is the debugging loop the SDK exists to support, and the one an agent cannot invent. What is checked is what a script does, never what a page says. | `FAIL` |
| `portability` | Line endings normalised for executable text, a Windows form for every POSIX command an agent is told to run, and CI proving it on a non-Linux runner. | `FAIL` |

## 6. The feature catalogue

The coverage and eval rules measure against one list: the things an agent is
expected to be able to do, taken from the scope statement in `AGENTS.md` section 1.
The list lives in `tools/agent/check_corpus.py` as `FEATURES`, so it is data, not
prose, and cannot drift from what is checked. Each entry names the page that must
answer it, a substring proving the page really covers it rather than mentioning it,
the recipe that demonstrates it, and whether an eval grades it.

Adding a feature to the framework that an agent is expected to use means adding a
row there. The check fails until the page exists, and warns until the example and
the eval do. That failure is the point.

## 7. Using it

CI runs `check_corpus.py --verbose` and fails on any `FAIL`. There is no floor to
raise or lower: a rule holds or it does not.

**Adding a rule is how this ratchets.** A rule written against a defect nobody
planted proves nothing, so the way to add one is to plant the defect first, confirm
the check goes red, then fix it. A pull request that adds a `.budgets` entry, or an
`// areg-check: ignore` marker, says in its description why the exception is cheaper
than compliance.

**What makes the wiring real** is that four suites execute -- `check_recipes.py`,
`check_mutations.py`, `check_observability.py` and `run_evals.py --self-check` --
and CI runs all four. These rules holding while those fail would be worthless.
