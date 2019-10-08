===========================================================================================
Eclipse project files for generic_nrf52811_xxaa
 - Example
===========================================================================================


 How to open it?
=================

 1. Set up your Eclipse. Instructions can be found at the beginning of this tutorial:
    https://devzone.nordicsemi.com/tutorials/7/
 2. Import this project into your workspace:
     *  File > Import... > General > Projects from Folder or Archive
     *  Select this folder in "Import source" field.
     *  Make sure that this folder is checked on list of folders
     *  Click "Finish"
 3. Build project for the first time, to allow indexer parse all build commands. If you
    still see some problems with unresolved symbols then you can do following:
     *  Run "rebuild" from "Build Targets"
     *  Right click on project > Refresh
     *  Right click on project > Index > Rebuild
    If you still see a problem see "Workaround for Eclipse overriding a settings file".


 Building and flashing
=======================

There are some predefined "Build Targets". To show them select: Window > Show View >
Other... > Build Targets. "default_make_target" will run "make" without parameters.
Other build targets will run make with its name as a parameter.


 Debugger
==========

generic_nrf52811_xxaa.launch is a debugger configuration ready to work with SEGGER J-Link.
To start debugger first flash board (start "flash" build target) and then click:
Run > Debug Configurations... > GDB SEGGER J-Link Debugging > generic_nrf52811_xxaa > Debug
After first run this configuration will be visible at launch history on toolbar.

By default optimizations are enabled, so debugging is not easy. You can disable
optimizations in "Makefile" file. Search for "-O3" option and replace it by "-O0".


 Workaround for Eclipse overriding a settings file
===================================================

Eclipse have a problem with importing "Language Settings Providers". This issue
does not happen always. Following workaround solves this problem:
 1. Create backup copy of a directory with eclipse project files before importing it
 2. Import project into your workspace:
    File > Import... > General > Existing Projects into Workspace
 3. Close project immediately after import
 4. Restore directory from your backup copy overriding all current files
 5. Open project
 6. Rebuild and refresh all:
     *  Right click on project > Refresh
     *  Run "clean" from "Build Targets"
     *  Run "default_make_target" from "Build Targets"
     *  Right click on project > Refresh
     *  Right click on project > Index > Rebuild
