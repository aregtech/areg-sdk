# Logger Service

The `logger` service is a log collector that is able to receive the logs from
multiple clients and forward to log observer (log viewer) applications.
The `logger` can run either as a console application, or as a system service,
which is started after system is up and running. By default, the `logger` starts as a console application,
where the user can interact with the application. The `logger` runs in system service mode with
option `--service`:
```bash
logger --service
```

## Install under Linux

Before installing `logger` under the Linux, make sure that the location of the `logger` executable
is properly set in the `logger.service` file.
Check the script `ExecStart=/usr/local/bin/logger.out --service`.
Change the path if you have another location. Make sure that `config/areg.init` is
in the same directory with `logger`.

To install the `logger` under Linux, use `logger.service` file and perform following steps:
1. Copy `logger.service` to the `/etc/systemd/system/` directory. For example, run this script:
```bash
sudo cp ./framework/logger/resources/logger.service /etc/systemd/system/
```
2. Run this script to enable `logger.service`. It may display error that the file already exists:
```bash
sudo systemctl enable logger.service
```
3. Run this script to start `logger.service`:
```bash
sudo systemctl start logger.service
```
4. If a warning message like `Warning: The unit file, source configuration file or drop-ins of logger.service changed on disk.` is
displayed, run this script to fix the problem:
```bash
sudo systemctl daemon-reload
```
5. To stop the `logger.service` use this script:
```bash
sudo systemctl stop logger.service
```

## Install under Windows

Before installing `logger` under Windows, make sure that you have correct 
location of the `logger` executable and `config/areg.init` file. 
To install the `logger` under Windows, use `logger_install.bat` file:
1. Open the Terminal as the Administrator
2. Run `logger_install.bat` script or type in the command line:
```bash
logger --install
```
3. To uninstall the `logger` as a system service, run the script `logger_uinistall.bat`
3. file as the Administrator.

Once you have installed `logger` as a system service, it should be listed in the `Services`
application available in your Windows system.
