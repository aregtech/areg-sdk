# Multicast Router

Multicast Router (*mcrouter*) is part of AREG SDK project. It is a tool, which creates a network and provides communication between Service Provier and Service Consumer nodes. Configure [_areg.init_](https://github.com/aregtech/areg-sdk/blob/master/framework/areg/resources/areg.init) file copied into the binary directory after the build to start IPC messages between applications. Following properties are relevant to `mcrouter` and connected applications:

```
router::*::service          = mcrouter      # The name of the router service (process name)
router::*::connect          = tcpip         # The list of supported communication protocols
router::*::enable::tcpip    = true          # Communication protocol enable / disable flag
router::*::address::tcpip   = 127.0.0.1     # Protocol specific connection IP-address
router::*::port::tcpip      = 8181          # Protocol specific connection port number
```

Multicast Router can run as a console or OS-managed service. To run Multicast Router as a service:
- **On Windows:** 
  - use copied `mcrouter.service.install.bat` file; 
  - make sure that the path to the `mcrouter` is properly set, and run the script;
  - alternatevely, call `mcrouter.exe --install` on command line;
  -  restart the system, make sure that Multicast Router is listed in the service list and runs.
- **On Linux:**
  - use copied `mcrouter.service` file;
  - make sure that the path to `mcrouter` is properly set (see `ExecStart=/usr/local/bin/mcrouter.out --service`);
  - run 'sudo systemctl enable mcrouter.service' in command line to enable Multicast Router;
  - run `sudo systemctl start mcrouter.service` in command line to start Multicast Router.

> [!NOTE]
> *Multicast Router* is necessary only for applications that provide or consume _Public_ services (multiprocess communication). In case of _Local_ services (multithreading communication) Multicast Router can be ignored. 
