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

using namespace std;

int main( void) {
	map<string, string> test;
	readConfigurationFile( "/home/leonard/workspace/radioCplus/target/test.txt", test);
	map<string, string>::iterator iter = test.begin();
	for (; iter != test.end(); iter++) {
		cout << iter->first << " = " << iter->second << endl;
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
