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
	cout << "constucteur de IRmanager" << endl;
	RemoteIRManager remoteManager;
	remoteManager.listenIR();
	cout << "fini!" << endl;
}
