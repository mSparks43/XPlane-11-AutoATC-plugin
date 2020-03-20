# XPlane-11-AutoATC-plugin
C/C++ Source code for the AutoATC plugin for X-Plane

# About

AutoATC provides an ATC "chatbot" for X-Plane. It features server-controlled AI airplanes and uses speech recognition and text to speech to interact with the user.

The complete add-on is hosted on [x-plane.org](https://forums.x-plane.org/index.php?/files/file/45663-main-installation-files-for-autoatc-for-xplane-11/)

To use ATC services, speech recognition and provide better text to speech output, two companion apps for Android are available, in a basic [freeware version](https://play.google.com/store/apps/details?id=org.zem.atctrans) and a [payware version](https://play.google.com/store/apps/details?id=org.zem.atctranspro) with more features.


# Building the plugin
## Dependencies
* openAL
* JDK ( https://www.oracle.com/java/technologies/javase-jdk14-downloads.html )
* X-Plane SDK ( https://developer.x-plane.com/sdk/plugin-sdk-downloads/ )
* Make (Linux/MacOS) or mingw64 (Windows)

## Preparation
* "XPLM", "Wrappers" and "Widgets" folders from the X-Plane SDK in the "SDK\CHeaders" folder
* The JDK for the target operating system must be located in "jdk/win" or "jdk/lin" or "jdk/mac"
    * Target path for Windows: "jdk/win/include"
    * Target path for Linux: "jdk/lin/include"
    * Target path for MacOS: "jdk/mac/Contents/Home/include"

## Compiling
### Windows
In the makefile's parent directory:
    mingw64-make clean
    mingw64-make OS=WINDOWS

Produces "win.xpl" in "build/AutoATC/64/".

### Linux
In the makefile's parent directory:
    make clean
    make OS=LINUX

Produces "lin.xpl" in "build/AutoATC/64/".

### MacOS
In the makefile's parent directory:
    make clean
    make OS=MACOS

Produces "mac.xpl" in "build/AutoATC/64/".

# Installation
* Download the remaining files for the plugin from [x-plane.org](https://forums.x-plane.org/index.php?/files/file/45663-main-installation-files-for-autoatc-for-xplane-11/) (account required)
* Copy "AutoATC" and "java" to "X-Plane/Resources/plugins"
* Install a suitable Java Runtime Environment release for your operating system and configure the path to jvm/libjvm in "java/defaultjvm.txt" or "java/jvmsettings.txt"
* Overwrite the .xpl file in "AutoATC/64/" with the one from the "build/AutoATC/64/" folder.
