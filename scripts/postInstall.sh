#!/bin/sh
# prerm script
#
echo "post installation script"
ldconfig
update-rc.d @PROJECT_NAME@ defaults
# overwrite lircd.conf
mv /etc/lirc/custom_lircd.conf /etc/lirc/lircd.conf
service lirc restart




#sudo cp -f webradio.m3u /var/lib/mpd/playlists/
#mpc clear
#mpc load webradio
