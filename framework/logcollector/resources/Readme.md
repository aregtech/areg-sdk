# Log Collector Service

The `logcollector` service is a log collector that is able to receive the logs from
multiple clients and forward to log observer (log viewer) applications.
The `logcollector` can run either as a console application, or as a OS-managed service,
which is started after system is up and running. By default, the `logcollector` starts as a console application,
where the user can interact with the application. The `logcollector` runs in system service mode with
option `--service`:
```bash
logcollector --service
```

## Install under Linux

Before installing `logcollector` under the Linux, make sure that the location of the `logcollector` executable
is properly set in the `logcollector.service` file.
Check the script `ExecStart=/usr/local/bin/logcollector.elf --service`.
Change the path if you have another location. Make sure that `config/areg.init` is
in the same directory with `logcollector`.

To install and run the `logcollector` under Linux, use `logcollector.service` file and perform following steps:
1. Copy `logcollector.service` to the `/etc/systemd/system/` directory. For example, run this script:
```bash
sudo cp ./framework/logcollector/resources/logcollector.service /etc/systemd/system/
```
2. Run this script to enable `logcollector.service`. It may display error that the file already exists:
```bash
sudo systemctl enable logcollector.service
```
3. Run this script to start `logcollector.service`:
```bash
sudo systemctl start logcollector.service
```
4. If a warning message like `Warning: The unit file, source configuration file or drop-ins of logcollector.service changed on disk.` is
displayed, run this script to fix the problem:
```bash
sudo systemctl daemon-reload
```
5. To stop the `logcollector.service` use this script:
```bash
sudo systemctl stop logcollector.service
```

## Install under Windows

Before installing `logcollector` under Windows, make sure that you have correct 
location of the `logcollector` executable and `config/areg.init` file. 
To install the `logcollector` under Windows, use `logcollector_install.bat` file:
1. Open the Terminal as the Administrator
2. Run `logcollector_install.bat` script or type in the command line:
```bash
logcollector --install
```
3. To uninstall the `logcollector` as a system service, run the script `logcollector_uinistall.bat`
3. file as the Administrator.

Once you have installed `logcollector` as a system service, it should be listed in the `Services`
application available in your Windows system.
