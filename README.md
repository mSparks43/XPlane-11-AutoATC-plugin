# XPlane-11-AutoATC-plugin
C/C++ Source code for Xplane AutoATC plugin

Requires the rest of the plugin available from
https://forum.thresholdx.net/clubs/21-autoatc

2 android companion applications are available.
### Free:
https://play.google.com/store/apps/details?id=org.zem.atctrans
### Paid:
https://play.google.com/store/apps/details?id=org.zem.atctranspro

makefile for Linux/Windows and Mac

# Building
depends on: 
- openAL
- JDK
* X Plane SDK (XPLM) https://developer.x-plane.com/sdk/plugin-sdk-downloads/
* Expects the X Plane SDK in the SDK/

n.b. the makefile still contains some absolute pointers to these on my machine, which will need editing. Pull prequests for an improved makefile welcome/requested.

## Windows
* I'm building the windows plugin from linux with mingw64
* JDK header files for windows are in the include/ directory
* set/uncomment OS=WINDOWS64 in the makefile
from the same directory:
mingw64-make clean
mingw64-make

* win.xpl will be in the build/AutoATC/64/win.xpl directory.

## Linux
* set/uncomment OS=LINUX in the makefile
make clean
make

* lin.xpl will be in the build/AutoATC/64/lin.xpl directory.

## MacOS
Very similar to linux, but dependancies are provided as "frameworks"
* install the JDK and openAL frameworks
* set/uncomment OS=MACOS in the makefile
make clean
make

* mac.xpl will be in the build/AutoATC/64/mac.xpl directory.
