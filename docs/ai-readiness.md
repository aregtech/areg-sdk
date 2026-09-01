# AI readiness: what is measured, and why

This page is for maintainers of the SDK, not for agents building applications. An
agent building an application reads `AGENTS.md` and stops there.

The repository claims to be usable by an AI agent. That claim was, until now, made
by narrative review: someone read the documentation, tried a task, and wrote down an
impression. An impression cannot be regressed. A later change that quietly removes a
page, doubles the entry toll, or breaks the one example a feature has, does not show
up anywhere.

`tools/ai_score.py` replaces the impression with an instrument. It reads the
repository and prints three scores out of 100. It is deterministic: same tree, same
numbers, on any machine, with no network and no build.

```
python3 tools/ai_score.py            # the three scores
python3 tools/ai_score.py --verbose  # every component, and what failed
python3 tools/ai_score.py --json     # the same, for CI
```

---

## 1. What the numbers are not

The three scores are **not comparable** to the 87 / 84 / 86 recorded in the earlier
narrative review. That review scored a reader's experience of a handful of tasks.
This one scores the system that produces those experiences: coverage, verification,
enforcement, and cost. The instrument is deliberately stricter, so it starts lower
and has somewhere to go.

The instrument also cannot see everything. It never builds, never runs the generator,
and never invokes a model. Where a property can only be established by execution, the
instrument scores the **enforcement** of that property instead: whether CI runs the
check, not whether the check passes today. That is the honest static proxy, and it is
the property that actually protects the repository over time.

## 2. Calibration

A component scores full marks when it is at the target stated in its row, not when it
is merely present. The overall reading:

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
| F2 | Documentation truth | 25 | Every self-claim the documentation makes about the repository holds, checked literally. |
| F3 | Verification coverage | 25 | Every recipe is built and run by CI; every eval task has a reference implementation that CI proves. |
| F4 | Prohibition detectability | 10 | Every prohibition in `docs/agent/api.json` names a rule that `tools/check_contract.py` implements. |
| F5 | Self-claim integrity | 10 | Every documented path resolves, and every agent page is reachable from the task table in `AGENTS.md`. |

F1 splits each feature three ways: 60% the page answers it, 20% an example shows it,
20% an eval grades it. A feature documented but never demonstrated is worth 0.6 of
one, which is the intent: prose alone is the weakest of the three.

## 4. Token Optimization (100) -- is the cost justified

| # | Component | Weight | Target |
|---|---|---|---|
| T1 | Entry toll | 25 | `AGENTS.md` at or below 10 KB. Every agent pays this on every task, so it is the most expensive byte in the repository. |
| T2 | Page budget | 35 | Every page in `docs/agent/` at or below 8 KB, median at or below 6 KB. |
| T3 | Forced off-path reads | 20 | No feature whose only answer is a schema, a generated file, or framework source. |
| T4 | Duplication | 10 | Under 2% of 12-word shingles repeated across three or more pages. Deliberate repetition of the few facts in `tools/check_agent_docs.py` is exempt by being small. |
| T5 | Generated-code avoidance | 10 | The rule against reading and editing generated code is stated on the entry path, and no recipe ships generated code to be read. |

The unit is bytes, not model tokens: bytes are exact, tokenizer-independent, and move
in the same direction. Roughly four bytes make a token for this material.

## 5. AI Readiness (100) -- does it stay true

| # | Component | Weight | Target |
|---|---|---|---|
| R1 | Eval coverage | 25 | Every catalogue feature has a construction task, and the failure classes an agent actually hits have repair tasks. Half the weight is repair: an agent that can only build from nothing is not ready. |
| R2 | CI enforcement | 25 | Seven gates run on every change: documented paths, contract on recipes, recipes build and run, project setup, eval self-check, token budget, and a non-Linux runner. |
| R3 | Tool completeness | 20 | Every tool `AGENTS.md` names exists and answers usefully. A rule explanation that restates the rule name is not an answer. |
| R4 | Observability workflow | 15 | A scenario starts the router, collects logs, and asserts against a `.sqlog` database. This is the debugging loop the SDK exists to support, and it is the one an agent cannot invent. |
| R5 | Portability | 15 | Line endings normalised for executable text, a Windows form for every POSIX command an agent is told to run, and CI proving it on a non-Linux runner. |

## 6. The feature catalogue

F1, T3 and R1 all measure against one list: the things an agent is expected to be
able to do, taken from the scope statement in `AGENTS.md` section 1. The list lives in
`tools/ai_score.py` as `FEATURES`, so it is data, not prose, and cannot drift from
what is measured. Each entry names the page that must answer it, a substring proving
the page really covers it rather than mentioning it, the recipe that demonstrates it,
and whether an eval grades it.

Adding a feature to the framework that an agent is expected to use means adding a row
there. The score drops until the page, the example and the eval exist. That drop is
the point.

## 7. Using it

The instrument exits non-zero when any score falls below a floor:

```
python3 tools/ai_score.py --floor 95
```

CI should hold the floor at the current measured value, so no change can lower it,
and raise it as the work lands. Ratcheting is what makes the number mean something a
month from now.
