# Orbiter Addons Manager
Orbiter Addons Manager is a useful tool to organize your Orbiter add-ons.
Allows you to install, uninstall, enable and disable add-ons with just one click! Uses an expandable database which supports any add-on, Can create snapshots and import it like a restore point.

Read the manual for full step-by-step explanation.

## Installation
Go to releases and download the latest release. Unzip the file and open OrbiterAddonsManager.exe.

The available builds are for Windows only.

## Build
You need to have Qt 5.11 and libarchive installed.

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
 You need to edit libarchive path `LIBS += -larchive`
 to the folder where you installed it. In my case, it's in D:/Qt/libarchive-3.3.3/libarchive:

```
INCLUDEPATH += D:/Qt/libarchive-3.3.3/libarchive
LIBS += -LD:/Qt/libarchive-3.3.3/libarchive -larchive
```
## About
The program is based on C++, Qt 5.11 and QArchive. Any suggestions, bugs and contributes are welcomed.

Should you have any inquiries, don't hesitate to contact me on [abbodmar@gmail.com](mailto:abbodmar@gmail.com?subject=Orbiter%20Addons%20Manager).

Copyright © Abdullah Radwan
