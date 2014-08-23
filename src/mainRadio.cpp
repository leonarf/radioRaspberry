//============================================================================
// Name        : mainRadio.cpp
// Author      : leonard
// Version     :
// Copyright   : Your copyright notice
// Description : main() for launching Radio module
//============================================================================
#include "RadioManager.h"
#include "Utils.h"
#include "iostream"

using namespace std;

int main( int argc, char *argv[]) {
	cout << "daemonize de RadioManager" << endl;
	cout << "PID_FILE : " << PID_FILE << endl;
	daemonize(PID_FILE);
	RadioManager::instance()->start();
}
