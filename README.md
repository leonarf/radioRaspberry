radioRaspberry
======================
= GOALS =
======================
This project functions are :
* Read webradio using MPD with a raspberry
* Control the "radio" with a wiimote
* Replace the remote control of a projector (or TV)
* Play movies on this projector
* Control everything with the wiimote or a unique remote.

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
* `cpack ..` will create a debian package of the program, to install it where you want
