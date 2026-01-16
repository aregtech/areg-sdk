# Lusan Live Log Viewer: Real-Time Monitoring for AREG Applications

This document explains how to use **Lusan** as a **live log viewer** for Areg-based applications. Live log viewing allows developers to monitor, filter, and analyze logs from distributed systems in real time.

It is ideal for development, debugging, system integration, and runtime diagnostics.

---

## Overview

Areg SDK includes a built-in logging infrastructure designed for **distributed, multi-process, and multi-threaded systems**. Logs produced by multiple applications can be collected centrally and visualized in real time.

Lusan's live log viewer enables real-time monitoring and analysis of distributed AREG-based applications. This centralized logging solution aggregates logs from multiple threads, processes, and devices into a single, unified interface, providing immediate visibility into system behavior and facilitating rapid debugging.

**Use Cases:**
- Development and debugging sessions
- Real-time performance monitoring
- Cross-service interaction tracing
- Dynamic troubleshooting without application restarts

**Key components:**
- **Areg-based applications**: generate logs
- **logcollector**: aggregates logs at runtime
- **Lusan**: connects to `logcollector` and visualizes logs live

Lusan does not generate logs. It connects to the logging infrastructure and provides visualization, filtering, and runtime control.

---

## Why Use Live Log Viewing?

Live log viewing with Lusan enables:
- Real-time visibility into system behavior
- Centralized logs from multiple applications and machines
- Correlation of events across processes and threads
- Runtime debugging without restarting applications
- Dynamic control of logging scopes and priorities

---

## Prerequisites

Before starting live log monitoring, ensure you have:

### Required Components
- **Lusan application** installed and built from [areg-sdk-tools repository](https://github.com/aregtech/areg-sdk-tools)
- **logcollector service** built from AREG SDK
- **AREG-based applications** configured for logging
- **`areg.init` and `lusan.init` configuration files** for `logcollector` and `Lusan` properly configured

Your `areg.init` and `lusan.init` files must define the log service endpoint:
```plaintext
logger::*::service          = logcollector  # Log Collector process name
logger::*::connect          = tcpip         # Communication protocol
logger::*::enable::tcpip    = true          # Enable TCP/IP
logger::*::address::tcpip   = 172.23.96.1   # IP address (default: 127.0.0.1)
logger::*::port::tcpip      = 8282          # Port number (default: 8282)
```

> [!NOTE]
> All applications and `Lusan` must use the same configuration to communicate with `logcollector`.

---

## Step-by-Step Guide to Live Log Viewing

### Step 1: Launch the Log Collector Service

Run `logcollector` as a console application or system service on a reachable machine.

Ensure that:
- The `areg.init` file is accessible
- TCP/IP address and port for logging are correctly configured

`logcollector` acts as the central aggregation point for all runtime logs.

**Option A: Console Mode (Development)**
```bash
./logcollector
```

**Option B: Service Mode (Testing/Production)**  
  On Linux (systemd):
  ```bash
  sudo systemctl enable logcollector.service
  sudo systemctl start logcollector.service
  ```
  On Windows:  
  ```powershell
  .\logcollector.exe --install
   net start logger.service
  ```

> [!TIP]
> More details of running `logcollector` as a service you can find in [Areg SDK Log Collector Service](./04d-logcollector.md) document.

**Verification:**
- Check console output for successful startup
- Confirm the service is listening on the configured port (default: 8282)
- Ensure the `areg.init` file is accessible to `logcollector`

**Common Issues:**
- Port already in use: Change the port in `areg.init`
- Permission denied: Run with appropriate privileges
- Configuration not found: Ensure `./config/areg.init` is in the working directory or specify its path

### Step 2: Start Lusan to Log Collector

Launch `Lusan` and establish connection to the running logcollector service.

**Connection Process:**
1. Start `Lusan` application
2. Either in menu `File` select `New Live Logs` or click appropriate `New Live Logs` button on toolbar.
3. You can as well switch to `Live Logs` tab in the Navigation pane and click `Connect to Log Collector` button.
4. Lusan automatically connects using settings from `./config/lusan.init`
5. Verify connection status in the Lusan log viewer

<div align="center"><a href="../img/lusan-live-log-connect.png"><img src="../img/lusan-live-log-connect.png" alt="Lusan Live Log to Log Collector" style="width:50%;height:50%"/></a></div>

**Connection Indicators:** you see the message `Log observer connected to log collector service.`.

### Step 3: Start Your AREG Applications

With `logcollector` running and `Lusan` connected, launch your Areg-based applications (or the application may run before).

**Application Configuration:**
- Applications must include the `areg.init` file
- Logging must be enabled in the application configuration
- Applications automatically connect to `logcollector` at startup and send logs.

**Multi-Source Logging:**
Lusan can simultaneously monitor logs from:
- Multiple applications on the same machine
- Distributed processes across different machines
- Various threads within each process
- Different devices in your network

**Expected Behavior:**
As each application starts and connects, you'll see initialization logs appear in `Lusan`, indicating successful connection, and the tree of log scopes in the left Navigation pane.

<div align="center"><a href="../img/lusan-live-log-connected-apps.png"><img src="../img/lusan-live-log-connected-apps.png" alt="Lusan Live Log and connected applications" style="width:80%;height:80%"/></a></div>

Logs are displayed in a structured, time-ordered format, making it easy to follow system activity as it happens.  
The log scopes are displayed in the left `Navigation` pane as an expandable tree structure, allowing you to browse scope nodes like modules and filter logs.  
The  active priorities of log scopes are highlighted as *selected* (buttons in blue rectangles).

### Step 4: Monitor and Analyze Real-Time Logs

Once applications are connected, `Lusan` displays logs in real-time with rich contextual information.

The user can select which information columns to display in the log viewer, such as:
- **Priority:** Log priority such as FATAL, ERROR, WARN, INFO, DEBUG, TRACE
- **Created Timestamp:** time when the log was generated
- **Received Timestamp:** time when the log was received by `Lusan`
- **Duration:** Time in **microseconds** passed when scope message activated
- **Source:** application name and unique ID within `logcollector` system
- **Source ID:** application unique identifier within `logcollector` system
- **Thread:** thread name in application
- **Thread ID:** thread unique identified within source application
- **Scope ID:** scope unique identifier within source application
- **Message:** detailed log message with context

<div align="center"><a href="../img/lusan-live-log-columns.png"><img src="../img/lusan-live-log-columns.png" alt="Lusan Live Log Columns" style="width:80%;height:80%"/></a></div>

All columns, except `Message`, can be shown or hidden based on user preference. The shown columns are highlighted and can be reordered via drag-and-drop.

> [!TIP]
> If declare scope message at the beginning of the method, the Duration value of `Exit Scope` messages will indicate the time in **microseconds** passed to execute the method as it is highlighted in the screenshot bellow.

<div align="center"><a href="../img/lusan-live-log-duration.png"><img src="../img/lusan-live-log-duration.png" alt="Lusan Live Log Duration" style="width:80%;height:80%"/></a></div>

**Visual Features:**
- **Color-coded priorities:** Quick identification of critical issues
- **Time-ordered display:** Chronological view of system events
- **Duration measurement:** Time spent in for performance analysis
- **Source and thread identification:** Easy tracking of multi-application and multi-threading systems
- **Scope highlighting:** Visual effect for marking logs related to the same scope (see screenshot above)
- **Auto-scrolling:** Follows new logs in real-time.
- **Search highlighting:** Easy identification of search results in the log viewer.

### Step 5: Filter and Search Logs

`Lusan` provides powerful filtering capabilities to focus on relevant information.

**Available Filters:**

**By Priority Level:**  
1. The filtering by scope priority level can be done via selecting/deselecting priority buttons on the `Navigation` pane or via context menu when selecting a scope node and right clicking on it. In this case the filtering is done on source application side, which generates the logs of enabled priorities only and sent to `logcollector` service. The logs of disabled priorities are not generated at all.
  <div align="center"><a href="../img/lusan-live-log-scope-log-prio.png"><img src="../img/lusan-live-log-scope-log-prio.png" alt="Lusan Live Set Scope Priority" style="width:80%;height:80%"/></a></div>

  > [!NOTE]
  > The scope priority level instructs the logging system to generate and send logs of the selected and higher priorities only. For example, if selected priory level `DEBUG`, all logs, except `Scope Enter` and `Scope Exit` messages will be displayed. If selected priority level is `ERROR` and `SCOPE`, all scope `Enter` and `Exit` messages will be generated and log message with priority `ERROR` and `FATAl`. The rest of logs is not generated by the system.

2. The filtering by message priority level can be done in the log message window to filter already received logs. In this case the filtering is done on `Lusan` side and the logs of priorities, which are not selected will be hidden from the log viewer.
  <div align="center"><a href="../img/lusan-live-log-message-prio.png"><img src="../img/lusan-live-log-message-prio.png" alt="Lusan Live Log Filter Priorities" style="width:80%;height:80%"/></a></div>

  It is possible to select more than one filtering criteria at the same time. Filtering by message priority level allows to filter already received logs without requesting source application to change the scope priority level. The filtering is possible to do by clicking on arrow on the column. Currently the filtering by the following criteria is supported:
  - **Filtering by Priority Level:** show the messages only of selected priority levels.
  - **Filtering by Duration:** show the messages only with duration value more or equal the specified value.
  - **Filtering by Source:** show the messages only of specified source name.
  - **Filtering by Source ID:** show the messages only of specified source ID.
  - **Filtering by Thread:** show the messages only of specified thread name.
  - **Filtering by Thread ID:** show the messages only of specified Thread ID.
  - **Filtering by Text Phrase:** show the messages only containing specified text phrase in the log messages. This filtering supports wildcard.

3. The filtering in Output Log Window can be done by selecting and double clicking on a message. This will mark a message of the session of selected scope and display the message in the `Output Log Window` at the bottom of the application (tab `Scope Analyzes`). Use following radio buttons to display filtered log messages:
  - **Session Logs:** show only messages of the selected scope session.
  - **Session Sublogs:** show only messages of the selected scope session and all its child scopes.
  - **Scope Logs:** show only messages of the selected scope, regardless of session.
  - **Thread Logs:** show only messages of the selected thread, regardless of scope and session.
  - **Process Logs:** show only messages of the selected process, regardless of thread, scope and session.
  <div align="center"><a href="../img/lusan-live-log-scope-analyzes.png"><img src="../img/lusan-live-log-scope-analyzes.png" alt="Lusan Live Log Scope Analyzes" style="width:80%;height:80%"/></a></div>

**Filtering Best Practices:**
- Start broad, then narrow filters progressively
- Combine multiple filters for precise investigation

### Step 7: Runtime Logging Control

Lusan enables dynamic control of logging behavior without restarting applications.

**Dynamic Controls:**

**Enable/Disable Log Scopes:**
- Turn specific components' logging on or off, or change logging priority
- Reduce noise from verbose components
- Enable detailed logging for components under investigation

**Adjust Priority Levels:**
- Increase verbosity (e.g., `INFO` → `DEBUG` → `SCOPE`)
- Reduce verbosity to minimize overhead
- Per-scope node (component) priority control

**Application-Wide Settings:**
- Enable/disable all logging for an application
- Bulk scope management
- Quick toggles for emergency situations

**Save Configuration:**
- Save current logging configuration (IP-address and Port Number of `logcollector`)
- Save logging priority of scopes of source application, so that on next start applications use the priority.

  <div align="center"><a href="../img/lusan-live-log-config.png"><img src="../img/lusan-live-log-config.png" alt="Lusan Live Log, Save Scope Configuration" style="width:80%;height:80%"/></a></div>

To save current configuration press `Save log settings` tool-button on `Live Logs` Navigation window. `Lusan` will collect the scope priority information of each source application and send the request to change the scopes and save current configuration. On next start the applications will use the saved information to filter and generate logs. Scopes with disabled logging will not generate logs at all.

> [!NOTE]
> The logs are generated if lower level logging is enabled. For example, if a scope has `INFO` level logging enabled, all logs with priority `INFO` and higher (`WARN`, `ERROR`, `FATAL`) will be generated. This does not reflect on `SCOPE` priority. Disabling `SCOPE` priority should be directly pointed. This means that the user can disable any log priority on the scope, but still can enable `SCOPE` priority, so that the system will generate `Enter Scope` and `Exit Scope` logs. If `SCOPE` priority is not enabled, no scope message is generated, as it is shown on the screenshot.

**Benefits of Runtime Control:**
- **No downtime:** Adjust logging without service interruption
- **Performance optimization:** Disable verbose logging in production
- **Targeted debugging:** Enable detailed logs only where needed
- **Production diagnostics:** Safely investigate issues in live systems

**Best Practices:**
- Keep production logging at `INFO` or even `WARN` level by default
- Enable `DEBUG`and/or`SCOPE` only when actively investigating
- Disable `SCOPE` logging after capturing necessary information
- Document scope purposes to guide effective filtering

---

## Troubleshooting Live Logging

### Logs Not Appearing

**Check logcollector:** Verify `logcollector` is running  
  On Linux
  ```bash
  ps aux | grep logcollector
  ```
  On Windows
  ```powershell
  tasklist | findstr logcollector
  ```

**Verify network connectivity:** Test connection to `logcollector` service port
```bash
# Using telnet
$ telnet 127.0.0.1 8282
# Or use nc (netcat)
$ nc -zv 127.0.0.1 8282
```

**Confirm application configuration:**
- Ensure `config/areg.init` for `logcollectot` and Areg-based applications is present and readable
- Ensure `config/lusan.init` for `Lusan` is present and readable`
- Verify logging is enabled in configuration settings
- Check that log service address matches `logcollector`

### Performance Issues

**High log volume:**
- Increase filter specificity
- Disable unnecessary scopes
- Reduce priority verbosity
- Consider offline analysis for intensive logging

**Network latency:**
- Ensure `logcollector` runs on the same network segment
- Check for network congestion
- Consider local `logcollector` instances for remote sites

### Connection Drops

**Symptoms:** Intermittent log gaps or disconnection messages

**Solutions:**
- Check network stability
- Verify `logcollector` resource availability (CPU, memory)
- Review firewall rules and network policies
- Ensure `logcollector` service hasn't crashed

---

## Performance Considerations

### System Impact

**Lusan Client:**
- Minimal CPU usage during normal operation
- Memory usage scales with log volume and log buffer size
- Network bandwidth depends on log volume

**Best Practices:**
- Use filtering to reduce processed log volume
- Clear old logs periodically during long sessions

### Production Environments

**Recommendations:**
- Use `INFO` or `WARN`, and disable `SCOPE` as default priority in production
- Enable `DEBUG` and/or `SCOPE` selectively and temporarily
- Monitor logcollector resource usage
- Consider dedicated logging infrastructure for high-volume systems

---

## Summary

Lusan's live log viewer transforms distributed logging from a challenge into a powerful development and debugging tool. By centralizing logs, providing real-time visibility, and enabling dynamic control, Lusan significantly reduces the time required to understand and diagnose issues in complex AREG-based systems.

**Key Takeaways:**
- ✅ Centralized view of all distributed logs
- ✅ Real-time monitoring with minimal latency
- ✅ Powerful filtering and correlation capabilities
- ✅ Runtime control without application restarts
- ✅ Essential tool for distributed system and multithreading application development

**Next Step:** explore [Offline Log Analysis](./09f-use-offline-logging.md) for post-mortem debugging
