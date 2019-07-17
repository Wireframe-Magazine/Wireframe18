To build:

On Linux, run ./build.sh to build and ./a.out to run after building.

On Windows you'll need to download SDL2 and either move the SDL2 folder which contains the include files to the same directory as main.c, or edit build.bat and add -IPATH_TO_SDL2_INCLUDE_FILES. Do the same with SDL2.lib and SDL2main.lib, either move those files to the same directory as main.c or edit build.bat to link to the files in their correct directories. Now open x64 Native Tools Command Prompt for VS2017, navigate to this folder and run build.bat. This will create main.exe which can be run as long as SDL2.dll is in the same directory.

Alternatively you could create a Visual Studio project, add your include/library directories and libs in the project settings, then build through Visual Studio. You'll still need to add SDL2.dll to the project directory.

This code should work on Mac as well but I don't own a Mac so I hope you don't have any trouble figuring it out yourself!
