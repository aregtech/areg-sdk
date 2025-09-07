# AREG Framework Overview

This directory contains the source codes for AREG Framework, which consists of following AREG SDK modules:
- **[AREG Communication Engine (*areg*)](./areg/):** Core framework enabling communication between software components.
- **[Multitarget Router (*mcrouter*)](./mcrouter/):** Facilitates message routing between services.
- **[Log Collector (*logcollector*)](./logcollector/):** Collects logs from applications and forwards them to log observers.
- **[Log observer Library (*areglogger*)](./areglogger/):** Library for receiving logs from the log collector service.
- **[Log observer (*logobserver*)](./logobserver/):** In real-time mode monitors, saves and dynamically controls logs.
- **[AREG Extended Library (*aregextend*)](./aregextend/):** Offers additional objects with extended features.
