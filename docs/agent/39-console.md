# A console application on areg

`areg` has no console: an application may be a service, a GUI, a library or a console
program. This page is for the last: reading a command typed at the console, and
printing a result. Diagnostics stay in the log (`34-logging.md`).

---

## 1. Reading a quit command

A thread of its own reads the console; no component thread blocks on input. Both parts
are worksheet sections of the program's `main.cpp`, so they live in `bodies.txt`:

```text
== main_top_station
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <string>
#include <thread>

//! Reads the console: "quit" ends the program; end of input keeps it running.
void read_console()
{
    std::string line;
    while (std::getline(std::cin, line))
    {
        if (line == "quit")
        {
            quit_with(0);
            return;
        }
    }
}

== main_run_station
std::thread(read_console).detach();
std::atexit([]() { std::fflush(stdout); std::_Exit(areg::Application::stored_element(_exitCode).valInt.mElement); });
```

The command is the application's choice; one program per section pair. `quit_with()` and
`_exitCode` come from the generated `main.cpp`.

- **End of input is not a quit request.** A process started without a terminal is handed
  a stream nothing is ever written to, or one already at its end. A loop that quits
  when `getline` returns false shuts the service down within milliseconds, and every
  peer waits for a provider that has already gone, with no error to read.
- **The `atexit` handler ends the process with its exit code** once `main()` returns,
  while the reading thread may still be blocked in `getline`.

---

## 2. Printing a result

One line, one call. Each `<<` is a write of its own, so `std::cout << "bay " << n` from
two threads mixes their lines and a test matches neither. `std::printf("bay %u ready\n", n);` is one
call.

`std::printf` needs no include in a generated file: `areg/base/String.hpp` brings
`<cstdio>`. Output to a pipe is buffered; `std::fflush(stdout)` after a line a test waits for.

---

## 3. Testing it

A scenario (`53-scenarios.md`) writes the command with `stdin`, on the lead of a
scenario of its own; on any other process the command is read at once and that process
quits before its peers are served. The scenario is a block of the bodies call, and a
regeneration keeps it:

```text
== scenarios.json
<<<<<<< SEARCH
  "scenarios": [
=======
  "scenarios": [
    { "name": "quit", "router": true, "timeout": 30,
      "procs": [ { "binary": "myprovider", "lead": true, "stdin": ["quit"], "exit": 0 } ] },
>>>>>>> REPLACE
```

`expect` matches what a process prints as well as what it logs. A process with no
`stdin` key gets its own standard input, which nothing writes to or closes.

---

## 4. A richer console: aregextend

`areg::ext::Console` (`aregextend/console/Console.hpp`, linked into every application)
prints at a position and reads input with a prompt (`wait_for_input`), thread safe.
Check what it does at end of input before a test runs it without a terminal.
