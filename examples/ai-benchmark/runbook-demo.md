# Runbook, benchmark part

The procedure is `docs/agent/01-runbook.md` in the SDK, and it is the same for a
measured run as for any other project. This page holds only what a measured run adds
to it, so every run is scored on the procedure a user's own agent follows.

- **Never commit anything, never run a git command, and never write a ReadMe.**
- **Never open `CONTRIBUTING.md`**, the SDK's maintainer file. It is for developing
  the framework itself and says nothing about building on it.
- **The fix bound is the runbook's section 8**, unless the prompt names another
  number. Then that number replaces 3 wherever the runbook says 3, and nothing else
  about the bound changes.
- **The report is the task file's table, and nothing else.** Fill it from what you
  already know and measure nothing to fill it in. `check_contract.py . --strict` is the
  checker to name in its findings row, and the acceptance count comes from the
  scenario output, not from a second run.
- **A path a page names that does not resolve** goes in the report, and the run
  carries on without it.
- **Every task here is a console program.** The quit command the task names is a line
  typed at the console, read as `docs/agent/39-console.md` shows (open it in the request
  that opens `05-design.md`), in each program the task says accepts it, and proven by a
  scenario of its own with `"stdin"`. A line the task says
  a program prints is logged with `GLOBAL_INFO`: a scenario's `expect` matches it, and
  `--verbose` shows it.
