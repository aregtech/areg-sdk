# Runbook, benchmark part

The procedure is `docs/agent/01-runbook.md` in the SDK, and it is the same for a
measured run as for any other project. This page holds only what a measured run adds
to it, so every run is scored on the procedure a user's own agent follows.

- **Never commit anything, never run a git command, and never write a ReadMe.**
- **Never open the SDK's maintainer files**, `CONTRIBUTING.md` and `CLAUDE.md`. They
  are for developing the framework itself and say nothing about building on it.
- **The fix bound is the runbook's section 8**, unless the prompt names another
  number. Then that number replaces 3 wherever the runbook says 3, and nothing else
  about the bound changes.
- **The report is the task file's table, and nothing else.** Fill it from what you
  already know and measure nothing to fill it in. `check_contract.py . --strict` is the
  checker to name in its findings row, and the acceptance count comes from the
  scenario output, not from a second run.
- **A path a page names that does not resolve** goes in the report, and the run
  carries on without it.
