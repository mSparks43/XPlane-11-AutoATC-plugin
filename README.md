# XPlane-11-AutoATC-plugin
C/C++ Source code for the AutoATC plugin for X-Plane 11.



# About

A discord server for support and discussion is now available at:
https://discord.gg/vBbESeu

AutoATC provides an ATC "chatbot" for X-Plane 11. It features server-controlled AI airplanes and uses speech recognition and text to speech to interact with the user.

See the [releases page](https://github.com/mSparks43/XPlane-11-AutoATC-plugin/releases) for the latest download.

In order to use ATC services and have a better text to speech output, two companion apps for Android are available, in a basic [freeware version](https://play.google.com/store/apps/details?id=org.zem.atctrans) and a [payware version](https://play.google.com/store/apps/details?id=org.zem.atctranspro) with more features.


# Installing
* Extract "AutoATC" and "AutoATC_java" to "X-Plane/Resources/plugins"

# Building
## Dependencies
* openAL
* [X-Plane SDK](https://developer.x-plane.com/sdk/plugin-sdk-downloads/)
* Make (building for Linux or MacOS) and/or mingw64 (building for Windows)

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
install openJDK
https://adoptopenjdk.net/

In "XPlane-11-AutoATC-plugin":
> make clean

> make OS=MACOS

Output is "mac.xpl" in "build/AutoATC/64/"


* Overwrite the .xpl file in "AutoATC/64/" with the one from the "deploy/XP1X/AutoATC/64/" folder (Where XP1X is either XP10 or XP11 depending on the XP version being targetted)
