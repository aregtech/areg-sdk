# CONTRIBUTING TO Areg SDK

Thank you for your interest in contributing to Areg SDK.  
We welcome developers, companies, researchers, and hobbyists who want to help improve the framework.

This guide explains how to contribute, how copyrights work, and what you need to do so your contribution can be accepted.

---

## 1. Code Licensing

All contributions are licensed under:

**Apache License, Version 2.0**

By submitting a pull request or commit, you agree that your contribution is provided under Apache 2.0.

---

## 2. Developer Certificate of Origin (DCO)

To contribute, you must confirm that you have the right to submit the code.

Areg SDK uses the **Developer Certificate of Origin (DCO)**. It is a simple and widely adopted alternative to contributor license agreements.

Every commit must include a `Signed-off-by` line:

```

Signed-off-by: Your Name [email@example.com](mailto:email@example.com)

```

Most Git tools can add this automatically:

```

git commit -s

```

By signing off, you state that your contribution is your original work or that you have permission to submit it.  
More information can be found at https://developercertificate.org/

---

## 3. Copyright Headers

You may add a copyright header if you want your name or organization to appear in the source file.

If you prefer not to add a header, the project may include a default header for clarity and consistent licensing.  
This does not affect your ownership or your rights in any way.

Example of an optional header:

```cpp
/************************************************************************
 * This file is part of the Areg SDK core engine.
 * Areg SDK is dual-licensed under Free open source (Apache version 2.0
 * License) and Commercial (with various pricing models) licenses, depending
 * on the nature of the project (commercial, research, academic or free).
 * You should have received a copy of the Areg SDK license description in LICENSE.txt.
 * If not, please contact to info[at]areg.tech
 *
 * \copyright   (c) 2017-2026 Aregtech (Artak Avetyan)
 * \file        path/to/YourFile.hpp
 * \ingroup     Areg SDK
 * \author      Your Full Name or GitHub ID
 * \brief       Brief description
 ************************************************************************/
```

Minor edits such as typo fixes, grammar corrections, formatting cleanup, comment updates, CMake changes, and YAML updates do not require contributor copyright headers.

> [!NOTE]
> Contributors retain copyright in their own contributions. Under the DCO (Section 2) and the license grant in Section 6, maintainers already hold an irrevocable right to use, modify, sublicense, and distribute all contributions, including under commercial terms. Once Aregtech UG is registered, maintainers may assign or license this existing right to Aregtech UG; no further action or additional grant from contributors is required.

---

## 4. Contribution Guidelines

### a. Reporting Bugs

When reporting an issue, please include:

* steps to reproduce
* expected and actual behavior
* platform and compiler information

### b. Submitting Code

Pull requests should follow these rules:

* one clear improvement per pull request
* include tests when possible
* include a `Signed-off-by` line in every commit

### c. Coding Style

All contributions must follow the Areg SDK coding style.

* **Coding style guide:** `./docs/AREG_CODING_STYLE.md` -- the rules with their rationale and examples.
* **Compact rule reference:** `./docs/AREG_AI_CODING_RULES.md` -- the same rules as numbered, normative one-liners. Give this one to an AI assistant working on the framework sources.

Neither applies to an application built *with* Areg. That work is described in `./AGENTS.md`, and an application may use any style it likes.

If your change touches existing code, keep changes consistent with the surrounding file style unless the change is part of a deliberate refactor that updates the whole file/module.

### d. Tool diagnostics

If you change a message that a tool prints, read `./docs/CODEGEN_MESSAGE_FORMAT.md` first. It specifies the diagnostic format of the code generator and states which parts of a message are treated as API by the documentation and by the agent tooling.

### e. Documentation

Improvements to guides, examples, comments, and general documentation are welcome.
Small corrections only require a Signed-off-by line.

### f. Agent documentation

`./AGENTS.md` is the single entry point for an agent building an application on top of Areg, and `./docs/agent/` is the corpus it routes to. Both are **agent-neutral**: no harness is named in them, and no harness gets its own copy of their contents.

Do not add a tracked instruction file for a particular coding agent. One exists already, `./.github/copilot-instructions.md`, and it is permitted only because it holds no guidance of its own: it is three sentences pointing at `AGENTS.md`. Any new redirect must be generated locally and left untracked:

```
python tools/setup_agent_redirect.py --list
python tools/setup_agent_redirect.py --harness claude
python tools/setup_agent_redirect.py --check     # fails if a redirect grew content
```

The reason is drift. Guidance duplicated per harness stops agreeing with `AGENTS.md` within a release or two, and the copies disagree with each other, so an agent's behaviour then depends on which file its harness happened to read.

Two further rules apply to changes in `AGENTS.md` and `docs/agent/`:

- **Every claim must be true of this commit.** A page that names a file, a method, a macro or a rule number is asserting it exists. `tools/check_agent_docs.py` verifies the paths; the rest is on the author. A wrong name costs an agent more than a missing one, because it is followed before it is doubted.
- **The corpus is measured.** `./docs/ai-readiness.md` defines the rubric and `tools/ai_score.py` implements it. Run it before and after your change; a pull request that lowers a score should say why in its description.

### g. Agent session knowledge

Work on this repository runs across many sessions, and each one otherwise begins by rediscovering what the last one established: how the tools are invoked on this machine, which invariants a plausible-looking edit breaks, why a decision was taken the way it was. Two local indexes hold that knowledge:

| File | Holds |
|---|---|
| `product/agent/memory/memory.md` | durable facts, indexing the files beside it |
| `product/agent/lessons/lessons.md` | mistakes made, and the rule that would have prevented each |

`product/copilot.md` is the session bootstrap that points at both; load it at the start of a session. Create the tree with:

```
python tools/setup_agent_memory.py --init
python tools/setup_agent_memory.py --check    # fails if a file is missing or unlisted
```

**The protocol is tracked; the content is not.** `product/*` is in `.gitignore` and nothing under it may enter a commit. The knowledge is local to one machine and one line of work, it would go stale inside a release, and an application author who found it would be misled by it. For the same reason it is **not** linked from `AGENTS.md`: that file is the entry point for building an application on top of Areg, it must resolve for every reader, and a path that exists only on one machine does not.

Update the indexes at the end of any session that established a durable fact or made a mistake worth not repeating. Each index states its own criteria for what belongs in it; the short form is that a fact the repository already states is not a memory, and a general principle nobody got wrong is not a lesson.

---

## 5. Areas Where Help Is Needed

1. C++17 development such as core framework, features, and services
2. Unit tests and example applications
3. Build systems and cross compilation setups
4. UI and UX improvements for the Areg SDK Tools
5. Technical writing including documentation and guides

---

## 6. Dual Licensing and Commercial Use

Areg SDK is licensed under the Apache License 2.0.  
You retain ownership of your contribution, which will always remain available under Apache 2.0.

Project maintainers may also offer Areg SDK under separate commercial terms.  
By contributing under the DCO and Apache 2.0, you grant the maintainers an irrevocable right to use, modify, sublicense, and distribute your contribution as part of both the open-source and any commercial editions of Areg SDK.

---

## 7. Code of Conduct

Always communicate respectfully and constructively.
We strive to maintain a welcoming community for everyone.

---

## 8. Questions

If you have any questions about licensing, rights, or the contribution process, open an issue or start a discussion.

We are happy to help you get started.
