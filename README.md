# Orbiter Addons Manager
A useful tool to organize your Orbiter add-ons. Shows the currently installed add-ons and allows you to uninstall it in one click, backs up your original files on install and restore it on uninstall.

Uses an expandable database which supports any add-ons. Includes an install features which will add the add-on to the database and back up existing files. Default database includes 31 add-ons.

Includes also an enable and disable feature so you can disable your add-ons without uninstalling it.

Read the manual for full step-by-step explanation.

## Installation
Go to releases and donwload the last releases. Unzip the file and open OrbiterAddonsManager.exe. Make sure to read the manual.

The available builds for Windows only. It should work on Wine for Linux. Build the program only if you want a native version.

## Build
You need to have latest Qt 5 version installed: `sudo apt install qt5-default`

```
git clone https://github.com/abdullah-radwan/OAD.git ~/OAD
cd ~/OAD
qmake OAD.pro
make
make clean
chmod u+x OAD
./OAD
```

## About
The program based on C++ and Qt using Qt Creator. This is my first C++ application. Any suggesstions, bugs and contributes are welcomed.

Should you have any inquires, don't hesitate to contact me on [abbodmar@gmail.com](mailto:abbodmar@gmail.com?subject=Orbiter%20Addons%20Manager).

Copyright © Abdullah Radwan
