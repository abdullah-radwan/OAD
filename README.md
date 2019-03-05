# Orbiter Addons Manager
Orbiter Addons Manager is a useful tool to organize your Orbiter add-ons.
Allows you to install, uninstall, enable and disable add-ons with just one click! Uses an expandable database which supports any add-on, Can create snapshots and import it like a restore point.

Read the manual for full step-by-step explanation.

## Installation
Go to releases and download the latest release. Unzip the file and open OrbiterAddonsManager.exe.

The available builds are for Windows only.

## Build
You need to have Qt 5.11 and Quazip installed.

```
git clone https://github.com/abdullah-radwan/OAD.git ~/OAD
cd ~/OAD
qmake OAD.pro
make
make clean
chmod u+x OAD
./OAD
```

To build on Windows, open the project with Qt Creator and open src.pro file.
 You need to edit Zlib and Quazip path:
 
 ```
LIBS += -lz
LIBS += -lquazip
 ```

To the folder where you installed them. In my case, they are in D:/Qt/zlib-1.2.11 and D:/Qt/zlib-1.2.11:

```
INCLUDEPATH += D:/Qt/zlib-1.2.11
LIBS += -LD:/Qt/zlib-1.2.11 -lz
INCLUDEPATH += D:/Qt/quazip-0.7.3/quazip
LIBS += -LD:/Qt/quazip-0.7.3/quazip/release -lquazip
```

## About
The program is based on C++, Qt 5.11 and Quazip. Any suggestions, bugs and contributes are welcomed.

Should you have any inquiries, don't hesitate to contact me on [abbodmar@gmail.com](mailto:abbodmar@gmail.com?subject=Orbiter%20Addons%20Manager).

Copyright © Abdullah Radwan
