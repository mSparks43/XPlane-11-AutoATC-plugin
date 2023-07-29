
![Github All Releases download count](https://img.shields.io/github/downloads/mSparks43/XPlane-11-AutoATC-plugin/total.svg?style=flat)

[Server Status](http://autoatc.zem-solutions.com/Status.html)

# XPlane-11-AutoATC-plugin
C/C++ Source code for the AutoATC plugin for X-Plane 11.

# About

A discord server for support and discussion is now available at:
https://discord.gg/vBbESeu

AutoATC provides an ATC "chatbot" for X-Plane 11. It features server-controlled AI airplanes and uses speech recognition and text to speech to interact with the user.

See the [releases page](https://github.com/mSparks43/XPlane-11-AutoATC-plugin/releases) for the latest download.

In order to use ATC services use [AutoATC Trainer](https://play.google.com/store/apps/details?id=org.zem.atctranspro) for Android

# Installing
* Extract the "AutoATC" folder to "X-Plane/Resources/plugins"

# Building
## Dependencies
* openAL
* [X-Plane SDK](https://developer.x-plane.com/sdk/plugin-sdk-downloads/)
* Make (building for Linux or MacOS) or Visual Studio Community (building for Windows)

## Prerequisites
* The "XPLM", "Wrappers" and "Widgets" folders from the X-Plane SDK must be located in the "SDK\CHeaders" folder, resulting in the following paths:
    * "XPLM": "SDK\CHeaders\XPLM"
    * "Wrappers": "SDK\CHeaders\Wrappers"
    * "Widgets": "SDK\CHeaders\Widgets"
* The JDK for the target operating system must be located in "jdk/win" or "jdk/lin" or "jdk/mac", resulting in the following paths:
    * JDK for Windows: "jdk/win/include"
    * JDK for Linux: "jdk/lin/include"
    * JDK for MacOS: "jdk/mac/Contents/Home/include"

## Compiling
### Windows
Windows users should use the Visual Studio Community project 

### Linux
In "XPlane-11-AutoATC-plugin":
> make clean

> make OS=LINUX XPVER=11

> make OS=LINUX XPVER=10

Output is "lin.xpl" in "deploy/XP1X/AutoATC/64/"

### MacOS
install jdk17
https://developer.ibm.com/languages/java/semeru-runtimes/downloads

install the xcode-select

xcode-select --install


In "XPlane-11-AutoATC-plugin":
> make clean

> make OS=MACOS

Output is "mac.xpl" in "build/AutoATC/64/"


* Overwrite the .xpl file in "AutoATC/64/" with the one from the "deploy/XP1X/AutoATC/64/" folder (Where XP1X is either XP10 or XP11 depending on the XP version being targetted)
