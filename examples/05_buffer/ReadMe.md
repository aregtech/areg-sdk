# 05_buffer

🧵 **Type:** Multithreading / Single Process

## Overview
The **05_buffer** example demonstrates how to manage a **Shared Buffer** in a **multithreaded environment** with AREG.  
It shows how to safely transfer copies of data between threads without relying on shared memory synchronization.  

This addresses common issues in multithreaded applications: avoiding race conditions, ensuring thread-safe data transfer, and achieving efficient inter-thread communication.

> **Note:** `SharedBuffer` is not for synchronized concurrent access. Simultaneous writes from multiple threads can produce unpredictable results. Its purpose is to transfer copies of data between threads safely.

## Concepts Shown
- **Shared Buffer**: Create and manage buffers that transfer copies of data between threads without locking.  
- **Multithreading**: Move primitive and object data efficiently across threads.  
- **Thread Management with `IEThreadConsumer`**: Control thread lifecycle and coordinate buffer operations safely.

## How It Works
- The **`SharedBuffer` class** holds copies of data for each thread and uses reference counting to prevent premature release.  
- **Threads** are created using the `Thread` class and implement `IEThreadConsumer` to manage execution.  
- **Data transfer** is serialized into the buffer by the writing thread and deserialized by reading threads, enabling predictable inter-thread communication.  
- No locking is performed; each thread works on its own copy of the data, ensuring thread safety without synchronization overhead.

## Use Cases
- Implement **inter-thread data streaming** in multithreaded applications.  
- Safely transfer **primitive and object data** without shared memory conflicts.  
- Support **real-time processing** scenarios where multiple threads consume data efficiently.

## Takeaway
A practical example showing **safe, efficient inter-thread data transfer** using AREG’s `SharedBuffer`, avoiding synchronization while maintaining predictable data flow.
