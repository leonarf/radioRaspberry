/*
 * RemoteIRManager.h
 *
 *  Created on: Sep 21, 2013
 *      Author: leonard
 */

#ifndef REMOTEIRMANAGER_H_
#define REMOTEIRMANAGER_H_

#include <mosquittopp.h>
#include "Utils.h"

using namespace std;
using namespace mosqpp;

class RemoteIRManager: private mosquittopp {
public:
	RemoteIRManager();
	virtual ~RemoteIRManager();
	int listenIR();

private:
	void on_message( const struct mosquitto_message *message);
	void connectMQTT();
	string progname;
	string configFile;
	struct lirc_config *config;

};

#endif /* REMOTEIRMANAGER_H_ */
