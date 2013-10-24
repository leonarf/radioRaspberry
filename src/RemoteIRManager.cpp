/*
 * RemoteIRManager.cpp
 *
 *  Created on: Sep 21, 2013
 *      Author: leonard
 */

#include "RemoteIRManager.h"
#include "lirc/lirc_client.h"

RemoteIRManager::RemoteIRManager() :
		mosquittopp( "IRmanager") {

	progname = "IRmanager";
	configFile = LIRCRC_FILE;

	if (lirc_init( (char*) progname.c_str(), 1) == -1)
		exit( EXIT_FAILURE);

	if (lirc_readconfig( (char*) configFile.c_str(), &config, NULL) != 0) {
		LOG( "Error while reading config file : " << configFile);
	}
	connectMQTT();
	loop_start();
}

RemoteIRManager::~RemoteIRManager() {
	lirc_freeconfig( config);
	lirc_deinit();
}

void RemoteIRManager::on_message( const struct mosquitto_message *message) {
	string topic( message->topic);
	string payload( static_cast<char *>( message->payload));
	LOG( "MQTT event received, topic = " << topic << " and payload = " << payload);
}

int RemoteIRManager::listenIR() {
	char *code;
	char *c;
	int ret;

	LOG( "Config read ");
	while (lirc_nextcode( &code) == 0) {
		LOG( "Some data received : " << code);
		if (code == NULL)
			continue;
		while ((ret = lirc_code2char( config, code, &c)) == 0 && c != NULL) {
			LOG( "Executing command " << c);
			string command( c);
			if (command == "HALT"){
				system("poweroff");
				return 0;
			}
			int result = publish( NULL, "/home/actuators/radio", command.size(), command.c_str(), 2);
			LOG( "result de publish = " << result);
		}
		free( code);
		if (ret == -1)
			break;
	}
	return 0;
}

void RemoteIRManager::connectMQTT() {
	int mqttResult = connect( BROKER_ADDRESS, BROKER_PORT);
	if (mqttResult != MOSQ_ERR_SUCCESS) {
		LOG( "could not connect to mqtt broker, error : " << mqttResult);
		return;
	}
	mqttResult = subscribe( NULL, "/home/sensors", 2);
	mqttResult = subscribe( NULL, "/home/sensors/InfraRed", 2);
	mqttResult = subscribe( NULL, "/home/sensors/InfraRed/#", 2);
	if (mqttResult != MOSQ_ERR_SUCCESS) {
		LOG( "could not subscribe to mqtt broker, error : " << mqttResult);
		return;
	}
	LOG( "Connecté au broker MQTT!");
}
