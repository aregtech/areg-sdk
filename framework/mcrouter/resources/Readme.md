# MCRouter Service

The `mcrouter` service is a message communication router application to route messages in the network. The `mcrouter` can run either as a console application, or as a system service, which is started after system is up and running. By default, the `mcrouter` starts as a console application, where the user can interact with the application. The `mcrouter` runs in system service mode with option `--service`:
```bash
mcrouter --service
```

## Install under Linux

Before installing `mcrouter` under the Linux, make sure that the location of the `mcrouter` executable is properly set in the `mcrouter.service` file. Check the script `ExecStart=/usr/local/bin/mcrouter.elf --service`. Change the path if you have another location. Make sure that `config/areg.init` is in the same directory with `mcrouter`.

To install the `mcrouter` under Linux, use `mcrouter.service` file and perform following steps:
1. Copy `mcrouter.service` to the `/etc/systemd/system/` directory. For example, run this script:
```bash
sudo cp ./framework/mcrouter/resources/mcrouter.service /etc/systemd/system/
```
2. Run this script to enable `mcrouter.service`. It may display error that the file already exists:
```bash
sudo systemctl enable mcrouter.service
```
3. Run this script to start `mcrouter.service`:
```bash
sudo systemctl start mcrouter.service
```
4. If a warning message like `Warning: The unit file, source configuration file or drop-ins of mcrouter.service changed on disk.` is displayed, run this script to fix the problem:
```bash
sudo systemctl daemon-reload
```
5. To stop the `mcrouter.service` use this script:
```bash
sudo systemctl stop mcrouter.service
```

## Install under Windows

Before installing `mcrouter` under Windows, make sure that you have correct location of the `mcrouter` executable and `config/areg.init` file. To install the `mcrouter` under Windows, use `mcrouter_install.bat` file:
1. Open the Terminal as the Administrator
2. Run `mcrouter_install.bat` script or type in the command line:
```bash
mcrouter --install
```
3. To uninstall the `mcrouter` as a system service, run the script `mcrouter_uinistall.bat`
3. file as the Administrator.

Once you have installed `mcrouter` as a system service, it should be listed in the `Services` application available in your Windows system.
