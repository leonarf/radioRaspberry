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
#include <csignal>

using namespace std;

void signalHandler( int signum )
{
    LOG_INFORMATION("Interrupt signal (" << signum << ") received.\n");

    LCDManager::instance()->poweredOff();
    // cleanup and close up stuff here
    // terminate program

   exit(signum);

}

int main( int argc, char *argv[]) {
	cout << "daemonize de LCDManager" << endl;
	daemonize(PID_FILE);
	// register signal SIGINT and signal handler
	signal(SIGTERM, signalHandler);
	LCDManager::instance()->start();
}
