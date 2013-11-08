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

#ifndef PID_FILE //should be define by cmake
	#define PID_FILE "erreur"
#endif

int main( int argc, char *argv[]) {
	cout << "daemonize de IRmanager" << endl;
	cout << "PID_FILE : " << PID_FILE << endl;
	cout << "LOG_FILE : " << LOG_FILE << endl;
	daemonize(PID_FILE);
	LOG("des choses dans le log file!!!!");
	RemoteIRManager remoteManager;
	remoteManager.listenIR();
}
