# The runtime configuration file

One file, `areg.init`, configures everything the framework does at run time:
logging, the address of `mtrouter`, the address of `logcollector`, socket sizes and
event queue limits. **No application code changes when it changes.** An application
that must run against a router on another machine is a configuration edit, not a
rebuild.

The shipped file, with every key and a comment on each, is
`framework/areg/resources/areg.init`. Copy it into your project and delete what you
do not set.

---

## 1. Where the file must be

The framework reads `config/areg.init` **relative to the executable's own folder**.
For a project that builds into `build/bin/`, that is:

```
build/bin/config/areg.init
```

Not `build/bin/areg.init`. A file placed beside the executable instead of inside its
`config/` subdirectory is never read, and nothing reports it: the application starts
with built-in defaults and logs nothing. Pass a different path as the sixth argument
of `areg::Application::setup()` to read one from somewhere else.

Nothing copies the file into the build output for you. Keep it in the project as
`config/areg.init` and add one line to the `CMakeLists.txt` that builds the
executable, after `include(${AREG_CMAKE})`:

```cmake
configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/config/areg.init"
    "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/config/areg.init"
    COPYONLY)
```

`CMAKE_RUNTIME_OUTPUT_DIRECTORY` is the `bin` directory areg already builds into, so
the file lands where the application looks. Re-run `cmake -B build` after editing it,
or copy it by hand:

```bash
mkdir -p build/bin/config && cp config/areg.init build/bin/config/
```

```bat
mkdir build\bin\config 2>nul & copy config\areg.init build\bin\config\
```

`mtrouter`, `logcollector` and `logobserver` read the same file the same way, and
each also accepts `--load <file>` to read another one.

## 2. The key

```
section::module::property[::position] = value
```

| Part | Meaning |
|---|---|
| `section` | the namespace: one of the six in section 3 |
| `module` | the **executable** name without extension, or `*` for every application |
| `property` | what is being set |
| `value` | the value; `\|` separates the members of a list |

**A key naming your executable beats the `*` key, and a key naming another executable
is ignored.** That is the whole override rule, and it is how the shipped file gives
`logcollector` and `logobserver` different logging from everything else. Write the
common case once with `*` and override per executable only where it differs.

A trailing `;` is allowed and changes nothing. The shipped file writes one on every
scope line; matching that is the tidier habit.

## 3. The six namespaces

| Namespace | Controls | An application normally sets |
|---|---|---|
| `config::` | schema version, default buffer block size, and the dispatcher event queue: `queue::capacity`, `queue::timeout`, `queue::drop` | only `version`; the queue keys are for tuning, and `../wiki/05b-areg-configuration-file.md` explains them |
| `log::` | everything about logging: targets, scopes, file names, layouts | yes, always -- `34-logging.md` is the page for it |
| `service::` | `list`, the remote services an application may connect to (`router \| logger`) | rarely; the default names both |
| `router::` | how to reach `mtrouter`: `service`, `connect`, `enable::tcpip`, `address::tcpip`, `port::tcpip` | yes, for any multi-process application not on one machine |
| `logger::` | how to reach `logcollector`, with the same five properties | yes, when collecting logs of several processes |
| `net::` | socket tuning per service: `sndbuf`, `rcvbuf`, `drain`, `pairs`, `timeout`, `cache` | no; change one only to answer a measurement |

`config::*::version` and `log::*::version` state which schema the file is written
against. The framework reads them back, and assumes its own built-in version when one
is absent -- so a file without them still loads, and still cannot be migrated.

## 4. Pointing an application at another machine

This is the reason the file exists. The default is a router on the same machine, and
these four keys move it:

```ini
config::*::version        = 2.0.0

router::*::service        = mtrouter
router::*::connect        = tcpip
router::*::enable::tcpip  = true
router::*::address::tcpip = 192.168.1.40    # the machine running mtrouter
router::*::port::tcpip    = 8181            # mtrouter's port
```

Every process of the application needs the same address, including the machine that
runs `mtrouter` itself. The collector is the same shape in the `logger::` namespace,
on port **8282**:

```ini
logger::*::service        = logcollector
logger::*::address::tcpip = 192.168.1.40
logger::*::port::tcpip    = 8282
```

**8181 is the router, 8282 is the collector.** They are different services, and
crossing the two ports points the application at the wrong one.

## 5. Never

- **Never ship a file without `config::*::version` and `log::*::version`.** Nothing
  fails at once; nothing can tell later which schema it was written for either.
- **Never put `areg.init` beside the executable.** It goes in `config/`.
- **Never point `router::` at the collector's port, or `logger::` at the router's.**
- **Never set `log::<module>::enable::remote = true` for `mtrouter`, `logcollector`
  or `logobserver`.** They would log through themselves.
- **Never expect an edit to be seen without copying it into `build/bin/config/`.**
  The build copies it at configure time, not at run time.

## 6. Before you move on

- [ ] The file is at `build/bin/config/areg.init`, inside `config/`.
- [ ] `CMakeLists.txt` has the `configure_file` line, or the file was copied by hand.
- [ ] Both `version` keys are present.
- [ ] Every process of a multi-process application has the same `router::` address
      and port.
- [ ] The router port and the collector port are not crossed.
- [ ] Keys meant for one executable name that executable; everything else uses `*`.
