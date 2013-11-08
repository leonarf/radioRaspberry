/*
 * mainLCD.cpp
 *
 *  Created on: Jul 27, 2013
 *      Author: leonard
 */

#include <unistd.h>
#include <iostream>
#include <LCDManager.h>
#include <Utils.h>

using namespace std;

int main( int argc, char *argv[]) {
	cout << "daemonize de LCDManager" << endl;
	daemonize(PID_FILE);
	LCDManager::instance()->start();
}
