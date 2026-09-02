# AI readiness: what is measured, and why

This page is for maintainers of the SDK, not for agents building applications. An
agent building an application reads `AGENTS.md` and stops there.

The repository claims to be usable by an AI agent. Left as narrative review --
someone reads the documentation, tries a task, writes down an impression -- that
claim cannot be regressed. A later change that quietly removes a page, doubles the
entry toll, or breaks the one example a feature has shows up nowhere.

`tools/agent/ai_score.py` replaces the impression with an instrument. It reads the
repository and prints three scores out of 100. It is deterministic: same tree, same
numbers, on any machine, with no network and no build.

```
python3 tools/agent/ai_score.py            # the three scores
python3 tools/agent/ai_score.py --verbose  # every component, and what failed
python3 tools/agent/ai_score.py --json     # the same, for CI
```

---

## 1. What the numbers are not

The instrument scores the **system** that produces an agent's experience --
coverage, verification, enforcement and cost -- not the experience itself. A number
from a narrative review is not comparable with one from here.

It also cannot see everything. It never builds, never runs the generator, and never
invokes a model. Where a property can only be established by execution, it scores
the **enforcement** of that property instead: whether CI runs the check, not whether
the check passes today. That is the honest static proxy, and it is the property that
protects the repository over time.

**A score of 100 means every component is at its stated target. It does not mean an
agent succeeds.** Every adversarial cold read so far -- an agent entering through
`AGENTS.md`, scaffolding, building, running, and planting defects to see what stayed
green -- has scored the corpus below what the instrument read. That gap is the
useful number. Closing a point of it means planting the defect first and confirming
the checker goes red; a checker written against a defect nobody planted proves
nothing.

### What the score cannot see

The rubric is written by the same people it grades, so its blind spots are recorded
here rather than left to be discovered:

- **T1 measures the size of `AGENTS.md`, not the bytes an agent reads before it can
  route.** Moving the routing table above the installation and golden-path sections
  cut the bytes read to route from 11.0 KB to 4.2 KB and changed T1 by nothing. The
  file-size proxy is cheap and stable, and it is a proxy.
- **`check_commands.py` resolves every documented command and runs the read-only
  ones.** The rest build, start or change something. A command that resolves and does
  not work is the class of defect that has cost the most time in past audits.
- **No model is in the loop.** `run_evals.py --self-check` grades the reference
  answers, not an agent's. Whether a cold model actually succeeds is measured by
  running one, and nothing here substitutes for that.

## 2. Calibration

A component scores full marks when it is at the target stated in its row, not when
it is merely present.

| Score | Means |
|---|---|
| 95-100 | Every component at target. An agent finds, in one hop, a correct and verified answer to any task in scope, and pays a justified number of tokens for it. |
| 85-94 | Small, named gaps. An agent succeeds at every task in scope but overpays somewhere, or one feature rests on an unverified claim. |
| 70-84 | A feature an agent will reach for has no page, no example, or no test. Recoverable by reading source, at a large token cost. |
| below 70 | An agent is expected to fail or to invent an answer for a task inside the stated scope. |

95 is the target for all three. It is not 100: the last five points are reserved for
properties no static instrument can establish.

## 3. AI Friendliness (100) -- can an agent find a correct answer

| # | Component | Weight | Target |
|---|---|---|---|
| F1 | Feature coverage | 30 | Every feature in the catalogue of section 6 is answered by a page, demonstrated by an example, and graded by an eval. |
| F2 | Documentation truth | 25 | Two halves. Every self-claim the documentation makes about itself holds, checked literally (15). And the documented **instructions** are executed by a checker in CI rather than read (10): the golden path builds a scaffolded project, the cheat sheet's configuration block is run and asserted to log, the recipes compile and print, the router-collector-`.sqlog` path runs, and the diagnostics the pages promise still fire. A block of configuration cannot be checked by reading it. |
| F3 | Verification coverage | 25 | Every recipe is built and run by CI; every eval task has a reference implementation that CI proves. |
| F4 | Prohibition detectability | 10 | Half for every prohibition in `docs/agent/api.json` naming a rule `tools/agent/check_contract.py` implements; half for that rule being **proven to fire** by a repair task that breaks something on purpose. Implemented is not working, and the same hand wrote both lists. |
| F5 | Self-claim integrity | 10 | Every documented path resolves, and every agent page is reachable from the task table in `AGENTS.md`. |

F1 splits each feature three ways: 60% the page answers it, 20% an example shows it,
20% an eval grades it. A feature documented but never demonstrated is worth 0.6 of
one, which is the intent: prose alone is the weakest of the three.

## 4. Token Optimization (100) -- is the cost justified

| # | Component | Weight | Target |
|---|---|---|---|
| T1 | Entry toll | 25 | `AGENTS.md` at or below 10 KB. Every agent pays this on every task, so it is the most expensive byte in the repository. |
| T2 | Page budget | 35 | Every page in `docs/agent/` at or below 8 KB, median at or below 6 KB. A page that is deliberately larger is named in `docs/agent/.budgets` with its reason, and earns 70% of the credit a compliant page earns: an exception is argued, recorded and cheaper than a split, but never free. An entry there for a page that does not exist, or one already under the ceiling, is a defect. |
| T3 | Forced off-path reads | 20 | No feature whose only answer is a schema, a generated file, or framework source. |
| T4 | Duplication | 10 | Under 2% of 12-word shingles repeated across three or more pages. Deliberate repetition of the few facts in `tools/agent/check_agent_docs.py` is exempt by being small. |
| T5 | Generated-code avoidance | 10 | The rule against reading and editing generated code is stated on the entry path, and no recipe ships generated code to be read. |

The unit is bytes, not model tokens: bytes are exact, tokenizer-independent, and
move in the same direction. Roughly four bytes make a token for this material.

## 5. AI Readiness (100) -- does it stay true

| # | Component | Weight | Target |
|---|---|---|---|
| R1 | Eval coverage | 25 | Every catalogue feature has a construction task, and the failure classes an agent actually hits have repair tasks. Half the weight is repair: an agent that can only build from nothing is not ready. A repair task carries the defect and the diagnostic that must report it, so `tools/agent/check_mutations.py` can assert the second without an agent supplying the first. |
| R2 | CI enforcement | 25 | Nine gates run on every change: documented paths, contract on recipes, recipes build and run, project setup, eval self-check, token budget, mutation testing, the observability path, and a non-Linux runner. A gate under `continue-on-error` reports and never fails a run, so it earns 40% of a gate, not all of it: the evidence is produced and readable, and nothing stops. |
| R3 | Tool completeness | 20 | Every tool `AGENTS.md` names exists and answers usefully. For `explain_rule.py` that means every rule says both what is wrong and what to change: the answer measured is what the tool prints, not what one of its inputs holds. |
| R4 | Observability workflow | 15 | A checker starts the router, collects logs with `logcollector`, queries the `.sqlog` database it writes, and runs in CI. This is the debugging loop the SDK exists to support, and the one an agent cannot invent. What is scored is what a script does, never what a page says. The reader is the recipe's own `query_sqlog.py`, which the checker imports: a second copy inside the checker would be the one that goes stale. |
| R5 | Portability | 15 | Line endings normalised for executable text, a Windows form for every POSIX command an agent is told to run, and CI proving it on a non-Linux runner. |

## 6. The feature catalogue

F1, T3 and R1 all measure against one list: the things an agent is expected to be
able to do, taken from the scope statement in `AGENTS.md` section 1. The list lives
in `tools/agent/ai_score.py` as `FEATURES`, so it is data, not prose, and cannot
drift from what is measured. Each entry names the page that must answer it, a
substring proving the page really covers it rather than mentioning it, the recipe
that demonstrates it, and whether an eval grades it.

Adding a feature to the framework that an agent is expected to use means adding a
row there. The score drops until the page, the example and the eval exist. That drop
is the point.

## 7. Using it

The instrument exits non-zero when a score falls below a floor. One number applies
to all three; three, in the printed order, give each score its own:

```
python3 tools/agent/ai_score.py --floor 95
python3 tools/agent/ai_score.py --floor 94,96,62
```

One floor for three scores is not a ratchet: the lowest score sets it, and the other
two can fall a long way unnoticed. So the gate in `.github/workflows/agent-docs.yml`
carries one floor per score, held at the value measured when the line was written.
**Raise a floor when the work that earns it lands; never lower one.**

**A floor is comparable to itself only while the measure is unchanged.** Changing
what a component counts makes today's number incomparable with yesterday's, so a
change to `ai_score.py` says in its pull request which measure moved and why. The
current numbers are whatever `ai_score.py` prints on the current tree; this page
does not carry a copy of them, because a copy goes stale the next time either one
changes.

**Read the number for what it is.** The instrument is static: it scores whether the
machinery is wired, not whether it works. What makes the wiring real is that four
suites execute -- `check_recipes.py`, `check_mutations.py`, `check_observability.py`
and `run_evals.py --self-check` -- and CI runs all four. A score with those failing
would be worthless. It is also self-graded: the same hands wrote the rubric, the
instrument and the corpus, so an independent audit is worth more than another point
here.
