# Directory Overview: Areg Logger Library

This directory contains the source code for the `areglogger` library, designed to facilitate log observation and management. It serves as a client for the `logcollector` service, which aggregates logs from multiple applications and forwards them to log observers. The `areglogger` library simplifies the connection and initialization of log observers, enabling them to receive logs seamlessly. It also includes C-style methods, making it suitable for use in managed code environments.

### Directory Contents

- **[client](./client)**
  Contains the source code for the log collector client and its API, enabling communication with the `logcollector` service.

- **[resources](./resources)**
  Includes resources specific to the `areglogger` library.
