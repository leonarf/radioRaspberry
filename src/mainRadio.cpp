//============================================================================
// Name        : mainRadio.cpp
// Author      : leonard
// Version     :
// Copyright   : Your copyright notice
// Description : main() for launching Radio module
//============================================================================
#include "RadioManager.h"
#include "Utils.h"

using namespace std;

#ifndef PID_FILE //should be define by cmake
	#define PID_FILE "erreur"
#endif

#ifndef LOG_FILE //should be define by cmake
	#define LOG_FILE "erreur"
#endif

int main( int argc, char *argv[]) {
	cout << "daemonize de RadioManager" << endl;
	cout << "PID_FILE : " << PID_FILE << endl;
	cout << "LOG_FILE : " << LOG_FILE << endl;
	daemonize(PID_FILE);
	logFile.open("/var/log/Radio.log", ofstream::app | ofstream::ate);
	logFile << "des choses dans le log file!!!!" << endl;
	logFile.flush();
	LOG("des choses dans le log file!!!!");
	ofstream toto;
	toto.open("/var/log/test.log");
	toto << "sdofihsdfoiguvysdofvuqsnocuyerfcoirc" << endl;
	toto.flush();
	RadioManager::instance()->start();
}
