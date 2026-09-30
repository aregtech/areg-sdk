# areg-sdk – Embedded Ready
## What it means, what it costs, and where the line is

> **Scope:** "Embedded ready" in areg-sdk means **Embedded Linux on a 32- or 64-bit CPU**.
> It does not mean bare metal, it does not mean an RTOS, and it does not mean a 16-bit
> microcontroller. Zephyr RTOS support is planned **after version 2.0.0** and does not
> exist today. Every figure below was measured with
> [`tools/intern/footprint.py`](./../../tools/intern/footprint.py) and carries the build it came from.

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
| **64-bit ARM** | `aarch64` / ARM64 | ✅ Supported, flash measured | NXP i.MX 8/9, TI AM62/AM64, Rockchip RK3399/RK3588, Broadcom BCM2711/2712 (Raspberry Pi 4/5), Allwinner A64/H6, Qualcomm QCS series |
| **32-bit ARM** | `armhf` / ARMv7-A | ✅ Supported, flash measured | NXP i.MX 6/7, TI AM335x (BeagleBone), STM32MP1, Allwinner H3, Broadcom BCM2837 (Raspberry Pi 3, 32-bit userspace) |
| **64-bit x86** | `x86_64` | ✅ Supported, flash and RAM measured | Intel Atom x6000E / Elkhart Lake, Intel Core embedded, AMD Ryzen Embedded V/R series |
| **32-bit x86** | `i686` | ✅ Supported, not measured | Legacy Intel Atom and industrial PC platforms |
| **MIPS** | `mips` / `mips64` | Builds, not measured | MediaTek MT7621 and similar router-class silicon |

The common requirement is not the instruction set. It is the operating system: if the target
runs Linux with sockets and pthreads, areg runs on it. What the instruction set does change is
the size of the image, by a lot -- section 3.3.

### 2.1 Where the line is

| Target | Status | Why |
|--------|--------|-----|
| **Cortex-M / bare metal** | ✗ Not supported | No processes, no POSIX sockets, no dynamic loader. The programming model does not degrade to it |
| **Any RTOS, including Zephyr** | ✗ Not supported today | **Planned after version 2.0.0.** Nothing in the current release targets an RTOS, and no figure on this page should be read as one |
| **16-bit CPUs** | ✗ Not supported | Out of scope, and not planned |
| **Internet-facing deployment** | ✗ Not supported | areg targets a trusted local network or a single device. It is not hardened for exposure to the public internet |

---

## 3. Flash: What a Device Stores

Measured with `tools/intern/footprint.py --flash-only`, GNU 15.2.0 for every target, `Release`,
`AREG_EXTENDED=OFF`. The `text` column is executable code and read-only data; `data` is
initialized writable data. Both live in flash. `bss` is zero-filled at start and costs RAM,
not flash.

Three architectures were built and read: **x86_64** natively, **ARM64** (`aarch64-linux-gnu`)
and **ARMv7** (`arm-linux-gnueabihf`) cross-compiled with the toolchain files the SDK ships in
`conf/toolchains/`. The `size` of each toolchain read its own binaries; a host `size` cannot
read a foreign object.

### 3.1 Shared library, logging on

The default build. Applications link against one shared `libareg.so`, so the framework is
stored once regardless of how many services run on the device. Figures are `text` bytes.

| Artefact | x86_64 | ARM64 | ARMv7 |
|----------|-------:|------:|------:|
| `libareg.so` (the framework) | 1 256 922 | 1 181 021 | **740 761** |
| `mtrouter` (the message router) | 285 137 | 254 888 | 171 238 |
| `logobserver` | 72 623 | 61 721 | 44 628 |
| a minimal service provider | 42 764 | 41 769 | 26 217 |
| a minimal service consumer | 64 829 | 62 956 | 37 424 |

`libareg.so` also carries `data` and `bss`, which are 38 139 / 60 360 on x86_64,
37 771 / 60 176 on ARM64 and **19 187 / 37 664** on ARMv7.

**A node stores 0.7-1.3 MB of framework plus tens of kB per service**, and a node that also
routes adds 171-285 kB.

### 3.2 Static library, logging off

The smallest configuration: one self-contained binary per service, no shared library, no
logging subsystem.

```bash
cmake -B ./build -DCMAKE_BUILD_TYPE=Release \
      -DAREG_LIB_TYPE=static -DAREG_LOGGER_LIB_TYPE=static \
      -DAREG_LOGGING=OFF -DAREG_EXTENDED=OFF
```

| Artefact | x86_64 | ARM64 | ARMv7 |
|----------|-------:|------:|------:|
| a minimal service provider | 525 813 | 486 836 | **287 778** |
| a minimal service consumer | 549 480 | 501 321 | 297 327 |
| a one-process application | 582 264 | 543 166 | 314 613 |
| `mtrouter` | 694 183 | 627 973 | 378 718 |

**A self-contained areg service is 0.29-0.53 MB of code, and a self-contained router is
0.38-0.69 MB.** No shared library, no runtime dependency beyond libc and libstdc++. On ARMv7
the whole provider binary is 484 840 bytes on disk, code, data and symbols together.

The archive `libareg.a` is 13 MB on disk, and that figure is **not** a deployment number: the
linker keeps only what a binary uses. What reaches the device is the figure above.
`tools/intern/footprint.py` reports archives as `n/a` for this reason.

### 3.3 What the instruction set costs

The same source, the same compiler version, the same switches:

| Against x86_64 | ARM64 | ARMv7 |
|----------------|------:|------:|
| the framework, shared | -6.0% | **-41.1%** |
| `mtrouter`, shared | -10.6% | -40.0% |
| a self-contained service, static | -7.4% | **-45.3%** |
| the framework's `bss` | -0.3% | -37.6% |

**ARM64 is within about 10% of x86_64; ARMv7 is close to half the size.** The gap is not
compiler luck: a 32-bit target halves every pointer in every structure the framework holds,
and Thumb-2 encodes much of the code in 16 bits. The `bss` row is the clearest evidence, since
it is pure data layout with no instruction encoding in it.

This is also why no number on this page was ever estimated from another architecture. A single
scaling factor would have been wrong by 35 percentage points depending on which row it was
applied to.

### 3.4 Smaller still: `MinSizeRel` and no exceptions

Measured on ARMv7, static, logging off -- the configuration of section 3.2 -- changing one
thing at a time. Figures are `text` bytes.

| Configuration | a service | `mtrouter` |
|---------------|----------:|-----------:|
| section 3.2, `Release` | 287 778 | 378 718 |
| `Release`, `-DAREG_NO_EXCEPTIONS=ON` | 245 997 (-14.5%) | 326 629 (-13.8%) |
| `-DCMAKE_BUILD_TYPE=MinSizeRel` | 172 278 (-40.1%) | 230 655 (-39.1%) |
| `MinSizeRel` and no exceptions | **143 816 (-50.0%)** | **194 182 (-48.7%)** |

**A complete areg service is 144 kB of code on ARMv7**, with its data and bss adding 10 608
and 23 120 bytes; the whole binary is 358 560 bytes on disk. A router beside it adds 194 kB.
That is the smallest configuration, and it is reached with documented switches only:

```bash
cmake -B ./build -DCMAKE_TOOLCHAIN_FILE=./conf/toolchains/gnu-linux-arm32.cmake \
      -DCMAKE_BUILD_TYPE=MinSizeRel \
      -DAREG_LIB_TYPE=static -DAREG_LOGGER_LIB_TYPE=static \
      -DAREG_LOGGING=OFF -DAREG_EXTENDED=OFF -DAREG_NO_EXCEPTIONS=ON
```

`MinSizeRel` builds at `-Os` and keeps the section, visibility and link time settings of
`Release`, so the dead code is still dropped at the final link. It costs speed: `-Os` declines
the inlining and loop transformations that `-O3` takes, so a build that has to hit a message
rate should stay on `Release` and save its space elsewhere.

`AREG_NO_EXCEPTIONS=ON` removes the exception tables and RTTI. It is a real constraint, not a
free switch: the framework then reports failures by return value only, and any application
code that throws has nothing to unwind it. The containers behave the same either way --
`free_extra()` and `release()` return the memory in both builds, and the unit tests and all
31 examples run in both.

### 3.5 Choosing between shared and static

| | Shared | Static |
|---|---|---|
| One service on the device | framework + tens of kB | 0.29-0.53 MB |
| Five services on the device | framework + ~150-250 kB | ~1.5-2.6 MB |
| Runtime dependency | `libareg.so` must be deployed and versioned | none |

Static wins for one or two binaries. Shared wins from roughly three services upward, and it is
what the default build produces.

---

## 4. RAM: What a Process Holds

Measured with `tools/intern/footprint.py`, reading `/proc/<pid>/status` every 50 ms and reporting
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

`tools/intern/footprint.py` builds nothing. It reads a build directory that already exists, so
the same command serves a native build, a cross build, and a CI job that only wants sizes.

```bash
# the full picture on the device itself
python3 tools/intern/footprint.py

# sizes only, from a cross build, on the development host
python3 tools/intern/footprint.py --build-dir ./build-arm \
        --size-tool arm-linux-gnueabihf-size --flash-only

# the same numbers as data, for a trend or a budget check
python3 tools/intern/footprint.py --json footprint.json
```

Every run prints what produced it first: commit, whether the tree was modified, build type,
compiler and version, the **target** architecture, the **host** that read it, and the
`AREG_LOGGING`, `AREG_EXTENDED` and `AREG_NO_EXCEPTIONS` switches. A number without that header
is a number without a source.

The target comes from the ELF headers of the binaries being measured, never from the machine
running the tool, so a cross build cannot be published under the host's architecture by
accident:

```
  target         arm (32-bit)
  host           Linux x86_64
```

Full options: [`tools/README.md`, section 13](./../../tools/README.md#13-flash-and-ram-footprint).

### 5.1 Two rules for honest figures

**RAM needs `/proc`.** It is measured on Linux only, and the tool says so elsewhere.

**RAM is never measured under an emulator.** A `qemu-user` run reports the emulator's memory,
not the target's. ARM RAM figures come from an ARM runner or a real board; a cross build
reports flash only. The tool enforces this rather than trusting the operator: when the
binaries' architecture is not the host's, it measures flash and refuses RAM, saying so.

---

## 6. What Is Measured and What Is Not

Claim boundaries matter more than favourable numbers.

| Figure | Status |
|--------|--------|
| x86_64 flash, shared and static, logging on and off | **Measured**, Section 3 |
| ARM64 flash, shared and static | **Measured**, Section 3. Cross-built with `conf/toolchains/gnu-linux-arm64.cmake`, read with `aarch64-linux-gnu-size` |
| ARMv7 flash, shared and static | **Measured**, Section 3. Cross-built with `conf/toolchains/gnu-linux-arm32.cmake`, read with `arm-linux-gnueabihf-size` |
| ARMv7 with `MinSizeRel` and `AREG_NO_EXCEPTIONS=ON` | **Measured**, Section 3.4 |
| x86_64 RAM, both builds, `pairs = 0` and `16` | **Measured**, Section 4 |
| **ARM RAM** | **Not measured.** It needs an ARM runner or a board. An emulator reports its own memory, so no figure is published from one |
| x86_64 with `MinSizeRel` | **Measured**: the framework is 892 784 against 1 256 957 at `Release`, 29% smaller. Its no-exceptions variant was not measured |
| ARM64 with `MinSizeRel` or no exceptions | Not measured. Only ARMv7 and x86_64 were |
| MIPS | Builds; never measured |
| The tuning keys other than `pairs` -- `queue::capacity`, `cache`, `drain`, `sndbuf`, `rcvbuf` | Not measured |
| Any RTOS, including Zephyr | **Does not exist.** Planned after 2.0.0 |

No figure on this page is estimated from another architecture, and section 3.3 shows why that
matters: the same source is 6% smaller on ARM64 and 41% smaller on ARMv7, so no single factor
would have been right.

Every table names the build it came from, and `tools/intern/footprint.py` prints the commit,
compiler, target architecture and switches ahead of any number it reports.

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
