# mtrouter Service

The `mtrouter` service is a message communication router application to route messages in the network. The `mtrouter` can run either as a console application, or as a system service, which is started after system is up and running. By default, the `mtrouter` starts as a console application, where the user can interact with the application. The `mtrouter` runs in system service mode with option `--service`:
```bash
mtrouter --service
```

## Install under Linux

Before installing `mtrouter` under the Linux, make sure that the location of the `mtrouter` executable is properly set in the `mtrouter.service` file. Check the script `ExecStart=/usr/local/bin/mtrouter.elf --service`. Change the path if you have another location. Make sure that `config/areg.init` is in the same directory with `mtrouter`.

To install the `mtrouter` under Linux, use `mtrouter.service` file and perform following steps:
1. Copy `mtrouter.service` to the `/etc/systemd/system/` directory. For example, run this script:
```bash
sudo cp ./framework/mtrouter/resources/mtrouter.service /etc/systemd/system/
```
2. Run this script to enable `mtrouter.service`. It may display error that the file already exists:
```bash
sudo systemctl enable mtrouter.service
```
3. Run this script to start `mtrouter.service`:
```bash
sudo systemctl start mtrouter.service
```
4. If a warning message like `Warning: The unit file, source configuration file or drop-ins of mtrouter.service changed on disk.` is displayed, run this script to fix the problem:
```bash
sudo systemctl daemon-reload
```
5. To stop the `mtrouter.service` use this script:
```bash
sudo systemctl stop mtrouter.service
```

## Install under Windows

Before installing `mtrouter` under Windows, make sure that you have correct location of the `mtrouter` executable and `config/areg.init` file. To install the `mtrouter` under Windows, use `mtrouter_install.bat` file:
1. Open the Terminal as the Administrator
2. Run `mtrouter_install.bat` script or type in the command line:
```bash
mtrouter --install
```
3. To uninstall the `mtrouter` as a system service, run the script `mtrouter_uinistall.bat`
3. file as the Administrator.

Once you have installed `mtrouter` as a system service, it should be listed in the `Services` application available in your Windows system.
