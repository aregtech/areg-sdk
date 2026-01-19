# 28_stlsync

🧵 **Type:** Multithreading / Single Process

## Overview
The **28_stlsync** project demonstrates how Areg’s **event synchronization object (`SyncEvent`)** can be used in STL-based applications. It adapts the [std::condition_variable](https://en.cppreference.com/w/cpp/thread/condition_variable.html) demo to use `SyncEvent` with `std::thread`, showing event waiting and signaling with sleep-based timing.

## Concepts Shown
- **Thread Synchronization** — coordinate access to shared resources with `SyncEvent`.
- **Locking Styles** — demonstrate automatic RAII lock (`Lock`) and manual lock handling.
- **Auto-reset Events** — `SyncEvent` resets automatically after being signaled; requires explicit `setEvent()` to trigger.
- **Conditional Waiting** — threads block until the event is signaled.
- **Cross-Platform Consistency** — unified behavior across Windows and Linux.

## How It Works
Two threads share a `std::string data`. The main thread signals when data is ready, and the worker waits on the event, processes the data, and signals completion. Both use `std::this_thread::sleep_for()` to simulate work and control timing, demonstrating event-based conditional waiting.

## Use Cases
- Build **multithreaded apps** requiring precise thread coordination.
- Provide **portable synchronization** across Windows and Linux.
- Safely manage shared resources while preventing race conditions.

## Takeaway
The **28_stlsync** example shows how to use Areg’s **synchronization event object** in pure STL-based applications. It demonstrates that Areg’s synchronization primitives work cross-platform, independent of the Areg service framework.
