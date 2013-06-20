radioRaspberry
======================
= BUILD DEPENDENCIES =
======================
* libbluetooth-dev
* wiiuse (http://sourceforge.net/projects/wiiuse/)
* libmpdclient-dev

====================
= RUN DEPENDENCIES =
====================
* mpd
* libmpdclient
* libbluetooth
* wiiuse

================
= HOW TO BUILD =
================
* Create a build directory, for example : `mkdir target`
* Run ccmake from the newly created directory giving the path to the CMakeLists.txt file, for example : `cd target`; `ccmake ..`
* In ccmake, press `c` to configure
* Press `e` to acknowledge message
* Set variable INSTALL_PREFIX (where the folder containing all exe will be created)
* Set variable TARGET (raspbian or fedora)
* Press `c` to reconfigure
* Press `g` to generate MakeFile
* Build with `make` command
* `make install` will create a 'sandbox' folder containing all file used by the program
