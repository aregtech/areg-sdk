# 18_locwatchdog Project Overview

The **18_locwatchdog** project showcases the use of a **Watchdog** in a **Local Service** environment, demonstrating how the AREG Framework can monitor and maintain thread activity. The watchdog sets a timeout for each thread, automatically restarting any that fail to respond within the designated time. Notifications of thread termination and restart events are sent to all **Service Providers** and **Service Consumers** to maintain system stability.

## Key Concepts

- **Watchdog for Thread Monitoring**: The Watchdog ensures that threads are actively responding. If a thread becomes unresponsive, it is automatically restarted, enhancing fault tolerance.
- **Local Service with Multithreading**: The project features a Local Service that operates entirely within the same process, emphasizing multithreading without external network communication.
- **Service Interface**: The project uses the AREG SDK's Service Interface and code generator to automate the creation of **Object Remote Procedure Call (Object RPC)** messages, streamlining communication between service components within the application.

## Project Structure

1. **18_generated**:
   - This library contains code generated from the [HelloWatchdog.siml](./res/HelloWatchdog.siml) service interface document during CMake configuration or as a pre-build action in Visual Studio. The generated files provide the necessary infrastructure to define and manage the watchdog and its related services.

2. **[18_locservice](./locservice/)**:
   - This sub-project implements the **Local Service** application, which is multithreaded and operates entirely within the same process. It includes:
     - A **Service Provider** that responds to requests and monitors the threads.
     - A **Service Consumer** that interacts with the Service Provider and logs the activity of the watchdog.
   - The logs provide detailed insights into the watchdog's behavior, including any thread restarts triggered by the watchdog.

## Communication

- **Multithreading Communication**: All interactions within the project occur via multithreading communication. The **Watchdog** monitors threads, restarting them if they exceed the defined timeout, ensuring that services remain responsive without requiring external communication.

## Key Features

- **Thread Monitoring and Restart**: The **Watchdog** automatically monitors threads within the process. If a thread becomes unresponsive, the watchdog terminates and restarts the thread, ensuring continuous service operation. If needed, the Watchdog may escape monitoring a thread.
- **Multithreaded Local Service**: The project demonstrates a multithreaded architecture where both the **Service Provider** and **Service Consumer** run on separate threads within the same application.
- **Fault Tolerance**: By monitoring thread health and automatically restarting unresponsive threads, the system remains fault-tolerant and reliable.
- **Detailed Logging**: Logs track watchdog activity, including thread terminations and restarts, providing valuable information for debugging and system monitoring.

## Use Cases

- **Fault-Tolerant Systems**: This project is ideal for scenarios requiring continuous monitoring and recovery of threads, ensuring system stability in real-time applications.
- **Multithreaded Local Services**: Learn how to implement a multithreaded Local Service where multiple threads interact within the same process, without the need for external communication.
- **Watchdog Monitoring**: Gain insights into how a watchdog can be used to monitor and restart unresponsive threads, making applications more resilient to failures.

## Conclusion

The **18_locwatchdog** project provides a practical demonstration of using a watchdog to monitor and maintain thread health in a **multithreading** environment. By automatically restarting unresponsive threads and notifying relevant services, the system remains stable and fault-tolerant. With the AREG Framework's support for **Object RPC**, the project illustrates how to build reliable, multithreaded applications with robust internal communication.
