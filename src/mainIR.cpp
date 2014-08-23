//============================================================================
// Name        : mainIR.cpp
// Author      : leonard
// Version     :
// Copyright   : Your copyright notice
// Description : maint() for launching infrared remote manager module
//============================================================================

#include <iostream>
#include "Utils.h"
#include "RemoteIRManager.h"

#include "jsoncpp/json/json.h"

using namespace std;

int main( int argc, char *argv[]) {
	cout << "daemonize de IRmanager" << endl;
	cout << "PID_FILE : " << PID_FILE << endl;
	daemonize(PID_FILE);
	RemoteIRManager remoteManager;
	remoteManager.listenIR();
}
