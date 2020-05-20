# XPlane-11-AutoATC-plugin
C/C++ Source code for the AutoATC plugin for X-Plane 11.



# About
AutoATC provides an ATC "chatbot" for X-Plane 11. It features server-controlled AI airplanes and uses speech recognition and text to speech to interact with the user.

The complete plugin is required for a complete installation and is hosted on [x-plane.org](https://forums.x-plane.org/index.php?/files/file/45663-main-installation-files-for-autoatc-for-xplane-11/) (requires an account do download).

In order to use ATC services and have a better text to speech output, two companion apps for Android are available, in a basic [freeware version](https://play.google.com/store/apps/details?id=org.zem.atctrans) and a [payware version](https://play.google.com/store/apps/details?id=org.zem.atctranspro) with more features.



# Building
## Dependencies
* openAL
* [Java Development Kit](https://www.oracle.com/java/technologies/javase-jdk14-downloads.html)
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
N.B. With the current Catalina issue a MacOS XP11 build will follow once the latest version for Windows/Linux is tested and deployed
In "XPlane-11-AutoATC-plugin":
> make clean

> make OS=MACOS

Output is "mac.xpl" in "build/AutoATC/64/"



# Installing
* Download the remaining files for the plugin from [x-plane.org](https://forums.x-plane.org/index.php?/files/file/45663-main-installation-files-for-autoatc-for-xplane-11/) (requires an account to download)
* Copy "AutoATC" and "java" to "X-Plane/Resources/plugins"
* Install a suitable Java Runtime Environment release for your operating system and configure the path to jvm/libjvm in "java/defaultjvm.txt" or "java/jvmsettings.txt"
* Overwrite the .xpl file in "AutoATC/64/" with the one from the "build/AutoATC/64/" folder
