#!/bin/bash

stopComputer=-1

home=$(dirname "$0")
target="##TARGET##"

if [[ "$target" == "raspbian" ]]
then
	exitCommand="sudo poweroff"
	ipAddress="192.168.0.4"
elif [[ "$target" == "fedora" ]]
then
	exitCommand='echo "sudo poweroff"'
	ipAddress="192.168.0.5"
elif [[ "$target" == "qemu" ]]
then
	echo "todo target qemu"
else
	echo "ERROR : var 'target' must be define in ${home}/startWiimote.sh"
	exit 1
fi
echo "$home"
play "${home}/sound/hello_uk.wav"
while [ 1 ]
do
	sleep 10
	# check the network
	networkConfig="ifconfig | grep $ipAddress"
	if [ ! -n "$networkConfig" ]
	then
		play "${home}/sound/youllregretthat.wav"
		ifdown eth0
		sleep 5
		ifup eth0
		continue
	fi
	
	#check the bluetooth
	hciDevices=$(hcitool dev)
	if [[ ! "$hciDevices=" == *hci* ]]
	then
		play "${home}/sound/uh-oh.wav"
		sudo ${home}/resetLibusb 1 4
		sleep 5
		sudo service bluetooth restart
		continue
	fi
	play "${home}/sound/comeonthen.wav"
	${home}radio
	stopComputer=$?
	echo "$stopComputer"
	if [ "$stopComputer" == "1" ]
	then
		play "${home}/sound/byebye.wav"
		$exitCommand
	fi

done
