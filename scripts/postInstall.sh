#!/bin/sh
# prerm script
#
# Removes all files installed by: ./setup.py install
echo "post installation script"
ldconfig
update-rc.d @PROJECT_NAME@ defaults



#sudo cp -f webradio.m3u /var/lib/mpd/playlists/
#mpc clear
#mpc load webradio
