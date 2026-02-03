# Log Collector Service

The `logcollector` service is a log collector that is able to receive the logs from
multiple clients and forward to log observer (log viewer) applications.
The `logcollector` can run either as a console application, or as an OS-managed service,
which is started after system is up and running. By default, the `logcollector` starts as a console application,
where the user can interact with the application. The `logcollector` runs in system service mode with
option `--service`:
```bash
logcollector --service
```

## Install under Linux

Before installing `logcollector` under Linux, make sure that the location of the `logcollector` executable
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

## Install under macOS

macOS uses `launchd` instead of `systemd` for managing system services. The service configuration is stored in a `.plist` file.

Before installing `logcollector` under macOS, make sure that the location of the `logcollector` executable is properly set in the `tech.areg.logcollector.plist` file. The default path is `/usr/local/bin/logcollector.mac`.

To install `logcollector` under macOS, use `tech.areg.logcollector.plist` file and perform the following steps:

1. Create the log directory:
```bash
sudo mkdir -p /var/log/areg
sudo chmod 755 /var/log/areg
```

2. Copy the plist file to `/Library/LaunchDaemons/` directory:
```bash
sudo cp ./framework/logcollector/resources/tech.areg.logcollector.plist /Library/LaunchDaemons/
sudo chown root:wheel /Library/LaunchDaemons/tech.areg.logcollector.plist
sudo chmod 644 /Library/LaunchDaemons/tech.areg.logcollector.plist
```

3. Load and start the service:
```bash
sudo launchctl load -w /Library/LaunchDaemons/tech.areg.logcollector.plist
```

4. To check if the service is running:
```bash
sudo launchctl list | grep tech.areg
```

5. To stop and unload the service:
```bash
sudo launchctl unload -w /Library/LaunchDaemons/tech.areg.logcollector.plist
```

6. View service logs:
```bash
tail -f /var/log/areg/logcollector.log
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
3. To uninstall the `logcollector` as a system service, run the script `logcollector_uninstall.bat` as the Administrator.

Once you have installed `logcollector` as a system service, it should be listed in the `Services`
application available in your Windows system.
