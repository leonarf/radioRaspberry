//============================================================================
// Name        : radioCplus.cpp
// Author      : leonard
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C, Ansi-style
//============================================================================

#include "stdlib.h"
#include "unistd.h"
#include <iostream>
#include "WiimoteManager.h"
#include "Utils.h"
#include "ConfigManager.h"
#include "ScreenManager.h"
#include "LCDI2C.h"
#include "RemoteIRManager.h"

#include "jsoncpp/json/json.h"

using namespace std;

int main( int argc, char *argv[]) {
	//LCD_I2C screen("/dev/i2c-1", 0x20);

	Json::Value root;
	root["start"]["icon"] = "icon-play";
	root["stop"]["icon"] = "icon-pause";
	root["next"]["icon"] = "icon-forward";
	root["previous"]["icon"] = "icon-backward";
	root["plusVolume"]["icon"] = "icon-volume-up";
	root["minusVolume"]["icon"] = "icon-volume-down";
	Json::StyledWriter jsonWriter;
	cout << jsonWriter.write(root) << endl;

	/*
	ScreenManager::instance()->setText("texte assez long", 1, 10, 10);
	ScreenManager::instance()->setText("texte assez long", 2, 25, 10);
	ScreenManager::instance()->setText("texte assez long", 3, 20, 10);
	ScreenManager::instance()->setText("texte assez long", 4, 5, 10);
	ScreenManager::instance()->setText("texte assez long", 1, 10, 15);
	sleep(60);
	return 0;
	string binDir = argv[0];
	binDir = binDir.substr( 0, binDir.find_last_of( '/') + 1);
	ConfigManager::instance()->setBinDir( binDir);
	cout << "binDir : " << binDir << endl;
	cout << "argv[0] : " << argv[0] << endl;
	string configFile = "";
	if (argc >= 2) {
		configFile = argv[1];
		cout << "argv[1] : " << argv[1] << endl;
		ConfigManager::instance()->setConfigFile( argv[1]);
	}
	*/
	cout << "constucteur de IRmanager" << endl;
	RemoteIRManager remoteManager;
	remoteManager.listenIR();
	cout << "fini!" << endl;

	/*
	 * Code pour utiliser la wiimote
	 */
	/*
	WiimoteManager wiimoteManager;
	//boucle pour se connecter
	while (wiimoteManager.wiiConnect() <= 0) {
		sleep( 1);
		LOG( "main loop : search wiimote to connect");
	}
	bool poweroff = wiimoteManager.startHandlingEvent();
	if (poweroff) {
		return 1;
	} else {
		return EXIT_SUCCESS;
	}
	*/
}
