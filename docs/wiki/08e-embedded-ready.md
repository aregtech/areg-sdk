# areg-sdk – Embedded Ready
## What it means, what it costs, and where the line is

> **Scope:** "Embedded ready" in areg-sdk means **Embedded Linux on a 32- or 64-bit CPU**.
> It does not mean bare metal, it does not mean an RTOS, and it does not mean a 16-bit
> microcontroller. Zephyr RTOS support is planned **after version 2.0.0** and does not
> exist today. Every figure below was measured with
> [`tools/footprint.py`](./../../tools/footprint.py) and carries the build it came from.

---

## 1. What "Embedded Ready" Claims

A framework is embedded ready when a developer can answer three questions before
choosing it:

1. **Will it fit?** How much flash does the binary take, and how much RAM does the
   process hold.
2. **Will it scale down?** What can be switched off, and what does switching it off save.
3. **Where does it stop?** Which targets it does not serve, stated plainly rather than
   left to be discovered after integration.

This page answers all three with measured numbers. It does not claim a device class that
has not been measured.

---

## 2. Target Classes

areg-sdk requires a **POSIX environment with a full TCP/IP stack, threads and processes**.
On embedded hardware that means Embedded Linux: Yocto, Buildroot, Debian-based
distributions, or a vendor BSP built on any of them.

| Class | Architecture | Status | Typical silicon |
|-------|--------------|--------|-----------------|
| **64-bit ARM** | `aarch64` / ARM64 | ✅ Supported | NXP i.MX 8/9, TI AM62/AM64, Rockchip RK3399/RK3588, Broadcom BCM2711/2712 (Raspberry Pi 4/5), Allwinner A64/H6, Qualcomm QCS series |
| **32-bit ARM** | `armhf` / ARMv7-A | ✅ Supported | NXP i.MX 6/7, TI AM335x (BeagleBone), STM32MP1, Allwinner H3, Broadcom BCM2837 (Raspberry Pi 3, 32-bit userspace) |
| **64-bit x86** | `x86_64` | ✅ Supported | Intel Atom x6000E / Elkhart Lake, Intel Core embedded, AMD Ryzen Embedded V/R series |
| **32-bit x86** | `i686` | ✅ Supported | Legacy Intel Atom and industrial PC platforms |
| **MIPS** | `mips` / `mips64` | Builds, not measured | MediaTek MT7621 and similar router-class silicon |

The common requirement is not the instruction set. It is the operating system: if the
target runs Linux with sockets and pthreads, areg runs on it.

### 2.1 Where the line is

| Target | Status | Why |
|--------|--------|-----|
| **Cortex-M / bare metal** | ✗ Not supported | No processes, no POSIX sockets, no dynamic loader. The programming model does not degrade to it |
| **Any RTOS, including Zephyr** | ✗ Not supported today | **Planned after version 2.0.0.** Nothing in the current release targets an RTOS, and no figure on this page should be read as one |
| **16-bit CPUs** | ✗ Not supported | Out of scope, and not planned |
| **Internet-facing deployment** | ✗ Not supported | areg targets a trusted local network or a single device. It is not hardened for exposure to the public internet |

---

## 3. Flash: What a Device Stores

Measured with `tools/footprint.py --flash-only`, GNU 15.2.0, `Release`, `x86_64`. The
`text` column is executable code and read-only data; `data` is initialized writable data.
Both live in flash. `bss` is zero-filled at start and costs RAM, not flash.

### 3.1 Shared library, logging on

The default build. Applications link against one shared `libareg.so`, so the framework is
stored once regardless of how many services run on the device.

| Artefact | text | data | bss | file |
|----------|-----:|-----:|----:|-----:|
| `libareg.so` (the framework) | 1 256 922 | 38 139 | 60 360 | 1 687 576 |
| `mtrouter` (the message router) | 285 137 | 12 600 | 36 256 | 396 960 |
| `logobserver` | 72 623 | 2 312 | 2 752 | 100 568 |
| a minimal service provider | 42 764 | 4 200 | 1 024 | 76 256 |
| a minimal service consumer | 64 829 | 7 296 | 1 472 | 109 912 |

**A node stores ~1.3 MB of framework plus tens of kB per service.** A node that also
routes adds 285 kB.

### 3.2 Static library, logging off

The smallest configuration: one self-contained binary per service, no shared library, no
logging subsystem.

```bash
cmake -B ./build -DCMAKE_BUILD_TYPE=Release \
      -DAREG_LIB_TYPE=static -DAREG_LOGGER_LIB_TYPE=static \
      -DAREG_LOGGING=OFF -DAREG_EXTENDED=OFF
```

| Artefact | text | data | bss | file |
|----------|-----:|-----:|----:|-----:|
| a minimal service provider | 525 813 | 24 404 | 37 464 | 723 600 |
| a minimal service consumer | 549 480 | 26 828 | 37 720 | 758 008 |
| a one-process application | 582 264 | 28 100 | 38 136 | 805 720 |
| `mtrouter` | 694 183 | 31 148 | 71 320 | 957 632 |

**A self-contained areg service is ~0.5 MB of code, and a self-contained router is under
1 MB.** No shared library, no runtime dependency beyond libc and libstdc++.

The archive `libareg.a` is 13 MB on disk, and that figure is **not** a deployment number:
the linker keeps only what a binary uses. What reaches the device is the ~0.5 MB above.
`tools/footprint.py` reports archives as `n/a` for this reason.

### 3.3 Choosing between them

| | Shared | Static |
|---|---|---|
| One service on the device | 1.3 MB + 43 kB | 0.5 MB |
| Five services on the device | 1.3 MB + ~250 kB | ~2.6 MB |
| Runtime dependency | `libareg.so` must be deployed and versioned | none |

Static wins for one or two binaries. Shared wins from roughly three services upward, and
it is what the default build produces.

---

## 4. RAM: What a Process Holds

Measured with `tools/footprint.py`, reading `/proc/<pid>/status` every 50 ms and reporting
the peak (`VmHWM`) of the run. `VmHWM` is the figure a device has to hold. The connection
count in every router row was read from `/proc/net/tcp`, so the per-connection cost is
measured, not assumed.

| Process | State | Shared, logging on | Static, logging off |
|---------|-------|-------------------:|--------------------:|
| a local service | at rest | 6 608 kB / 7 threads | 5 608 kB / 6 threads |
| `mtrouter` | 0 clients | 6 536 kB / 9 threads | 5 728 kB / 8 threads |
| `mtrouter` | 10 clients | 6 612 kB / 9 threads | 5 792 kB / 8 threads |
| `mtrouter` | streaming | 7 564 kB / 9 threads | 6 724 kB / 8 threads |

Three things follow from this table.

**A process costs ~5.6-6.6 MB resident.** That is the floor for linking areg, before the
application's own data. It is dominated by thread stacks and the event queues, not by code.

**A connection costs ~7 kB.** The router grows 28 kB for the first client and 76 kB for
ten: ten clients cost under 1% of its idle size. A router's memory is spent on being a
router, not on the clients attached to it.

**Logging costs ~0.8-1.0 MB and one thread.** `-DAREG_LOGGING=OFF` removes the logging
thread and its buffers. On a constrained device that is the first switch to reach for, and
the only one measured here with a material effect.

### 4.1 The one tuning key that moves RAM

`net::*::tcpip::pairs` in [`areg.init`](./05b-areg-configuration-file.md) gives each group
of clients its own send and receive thread pair instead of sharing one pair.

| `pairs` | `mtrouter`, 0 clients | Threads |
|--------:|----------------------:|--------:|
| `0` (shipped default) | 6 536 kB | 9 |
| `16` | 8 344 kB | 41 |

**`pairs = 16` costs 1 808 kB and 32 extra threads before a single client connects**, and
the pool is allocated in full whatever the load turns out to be. The shipped default of
`0` is the right one for a constrained device; raise it only on a machine that has both
the cores and the memory to spend.

Other keys (`queue::capacity`, `cache`, `drain`, `sndbuf`, `rcvbuf`) also move memory and
have not yet been measured. They are listed in the
[configuration reference](./05b-areg-configuration-file.md).

---

## 5. Measuring Your Own Target

`tools/footprint.py` builds nothing. It reads a build directory that already exists, so
the same command serves a native build, a cross build, and a CI job that only wants sizes.

```bash
# the full picture on the device itself
python3 tools/footprint.py

# sizes only, from a cross build, on the development host
python3 tools/footprint.py --build-dir ./build-arm \
        --size-tool arm-linux-gnueabihf-size --flash-only

# the same numbers as data, for a trend or a budget check
python3 tools/footprint.py --json footprint.json
```

Every run prints what produced it first: commit, whether the tree was modified, build
type, compiler and version, machine, and the `AREG_LOGGING`, `AREG_EXTENDED` and
`AREG_NO_EXCEPTIONS` switches. A number without that header is a number without a source.

Full options: [`tools/README.md`, section 13](./../../tools/README.md#13-flash-and-ram-footprint).

### 5.1 Two rules for honest figures

**RAM needs `/proc`.** It is measured on Linux only, and the tool says so elsewhere.

**RAM is never measured under an emulator.** A `qemu-user` run reports the emulator's
memory, not the target's. ARM RAM figures come from an ARM runner or a real board; a cross
build reports flash only.

---

## 6. What Is Measured and What Is Not

Claim boundaries matter more than favourable numbers.

| Figure | Status |
|--------|--------|
| x86_64 flash, shared and static, logging on and off | **Measured**, Section 3 |
| x86_64 RAM, both builds, `pairs = 0` and `16` | **Measured**, Section 4 |
| ARM64 and ARMv7 flash | **Not yet measured.** Cross-compilation is supported and CI builds both; the figures are not published until they are read from a real build |
| ARM RAM | **Not yet measured.** It needs an ARM runner or a board, never an emulator |
| MIPS | Builds; never measured |
| `-Os`, `AREG_NO_EXCEPTIONS=ON` | Supported switches; their effect on size is not yet measured |
| Any RTOS, including Zephyr | **Does not exist.** Planned after 2.0.0 |

An ARM figure is not estimated from an x86_64 one on this page. Code density differs
enough between the two that a scaled number would be a guess wearing a measurement's
clothes.

---

## 7. Related Reading

- [Performance benchmarks](./08b-areg-sdk-performance-benchmarks.md) – latency, message
  rate and data rate, with methodology
- [CMake configuration](./02d-cmake-config.md) – every build switch, including
  `AREG_LIB_TYPE`, `AREG_LOGGING` and `AREG_EXTENDED`
- [Cross-compilation](./01b-cmake-build.md#cross-compilation) – building for ARM targets
- [`areg.init` reference](./05b-areg-configuration-file.md) – the runtime keys, including
  `net::*::tcpip::pairs`
- [`tools/README.md`, section 13](./../../tools/README.md#13-flash-and-ram-footprint) –
  `footprint.py` options
