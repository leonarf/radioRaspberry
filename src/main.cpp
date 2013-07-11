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

using namespace std;

int main( int argc, char *argv[]) {
	ScreenManager::instance()->start();
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

	WiimoteManager wiimoteManager;
	//boucle pour se connecter
	while (wiimoteManager.connect() <= 0) {
		sleep( 1);
		LOG( "main loop : search wiimote to connect");
	}
	bool poweroff = wiimoteManager.startHandlingEvent();
	if (poweroff) {
		return 1;
	} else {
		return EXIT_SUCCESS;
	}
}
