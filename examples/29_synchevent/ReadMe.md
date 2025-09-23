# 29_synchevent

🧵 **Type:** Multithreading / Single Process

## Overview

This demo shows how Areg’s **`SynchEvent`** keeps signals safe — no spurious wakeups, no missed notifications. Two auto-reset events are signaled in different threads, proving that once an event is set, it stays signaled until a thread consumes it.  

Unlike `std::condition_variable`, you don’t need extra boilerplate or worry about losing signals.

## Concepts Shown
- **Auto-reset events** — one signal wakes one thread, then resets.  
- **Signal persistence** — an event stays signaled until taken.  
- **Thread coordination** — simple lock/unlock with `lock()`.  
- **Cross-platform consistency** — works the same on Windows and Linux.  
- **Cleaner than STL** — no spurious wakeups, no manual re-check loops.  

## How It Works
1. Two worker threads share a `std::string data`.  
2. The main thread checks both events start **non-signaled**.  
3. Each worker sets its event **before** calling `lock()`.  
4. `lock()` consumes the signal and resets the event.  
5. The result: signals are never lost, even if the wait happens later.  

## Use Cases
- Reliable **thread handshakes**.  
- Portable **sync primitives** without OS-specific APIs.  
- Avoiding race conditions in **shared state updates**.  

## Takeaway
**`SynchEvent`** is a drop-in, cross-platform event primitive that delivers **guaranteed signals, no spurious wakeups, and simpler code** than `std::condition_variable`.
