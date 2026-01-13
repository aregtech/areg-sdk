# Using Lusan as a Log Viewer (Live and Offline)

This document explains how to use **Lusan** as a centralized **log viewer** for Areg-based applications. Lusan supports both **live log monitoring** and **offline log analysis**, making it suitable for development, debugging, testing, and production diagnostics of distributed systems.

---

## Overview of Logging in Areg SDK

Areg SDK provides a built-in logging infrastructure designed for **distributed and multi-process systems**. Logs can be produced by multiple applications and collected centrally.

Key components involved:
- **Areg-based applications** – generate logs
- **logcollector** – aggregates logs at runtime
- **Lusan** – visualizes, filters, correlates, and analyzes logs

Lusan does not generate logs itself; it visualizes and analyzes logs collected by the logging system.

---

## Why Use Lusan for Log Viewing?

Lusan provides a **single, unified view** of logs across threads, processes, and devices.

Key benefits:
- Real-time visibility into system behavior
- Centralized log aggregation
- Correlation of logs across multiple instances
- Offline analysis of recorded sessions
- Runtime control of logging scopes and priorities

---

## Prerequisites

Before using Lusan as a log viewer, ensure that:

- Lusan is built and available  
  👉 https://github.com/aregtech/areg-sdk-tools
- `logcollector` is built and running
- Areg-based applications are configured to send logs
- Network configuration (`areg.init`) is correctly set

---

## Live Log Viewing

### Step 1: Start `logcollector`

Run `logcollector` as a console application or system service on any reachable machine.

Ensure:
- The `areg.init` file is available
- TCP/IP logging address and port are correctly configured

`logcollector` acts as the central aggregation point for all logs.

---

### Step 2: Start Lusan and Connect to Log Collector

Launch Lusan and connect it to the running `logcollector`.  
Once connected, Lusan begins receiving logs in real time.

<!-- TODO: Insert screenshot of Lusan log connection view -->

---

### Step 3: View Live Logs

As applications run, logs appear in the Lusan log view. Logs may originate from:
- Multiple applications
- Multiple processes
- Multiple devices

Lusan displays logs in a structured and time-ordered format.

<!-- TODO: Insert screenshot of live log view -->

---

### Step 4: Filter and Analyze Logs

Lusan allows developers to:
- Filter logs by scope, priority, or source
- Correlate logs across components
- Trace request flows across distributed services
- Inspect method execution and timing

These capabilities make it easier to debug complex interactions and identify performance bottlenecks.

---

### Step 5: Control Logging at Runtime

Using Lusan, developers can dynamically:
- Enable or disable log scopes
- Adjust log priorities
- Reduce noise or increase verbosity without restarting applications

This is especially useful in long-running or production-like environments.

<!-- TODO: Insert screenshot of logging control panel -->

---

## Offline Log Analysis

### Step 1: Record Logs

Logs can be saved during runtime using:
- `logcollector`
- `logobserver`
- Lusan itself (depending on configuration)

Recorded log files capture complete logging sessions for later analysis.

---

### Step 2: Load Log Files in Lusan

Open Lusan and load previously recorded log files.  
No running applications or log collector are required for offline analysis.

<!-- TODO: Insert screenshot of offline log loading -->

---

### Step 3: Analyze Recorded Sessions

Offline mode provides the same analysis capabilities as live mode:
- Filtering and searching
- Correlation across components
- Request and event tracing
- Performance analysis

Offline analysis is ideal for:
- Post-mortem debugging
- Sharing logs between team members
- Investigating rare or intermittent issues

<!-- TODO: Insert screenshot of offline log analysis view -->

---

## Best Practices

- Use live logging during development and testing
- Record logs for complex or intermittent issues
- Limit log verbosity in production and enable it dynamically when needed
- Combine Lusan with `logobserver` for headless environments

---

## Summary

Lusan provides a powerful and developer-friendly solution for both **live monitoring** and **offline analysis** of logs in Areg-based systems. By centralizing logs, enabling runtime control, and offering deep correlation and analysis, Lusan significantly simplifies debugging and performance tuning of distributed applications.

It turns raw logs into actionable insights across the entire system.
