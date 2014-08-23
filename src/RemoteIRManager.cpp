/*
 * RemoteIRManager.cpp
 *
 *  Created on: Sep 21, 2013
 *      Author: leonard
 */

#include "RemoteIRManager.h"
#include "mqttTopics.h"
#include "lirc/lirc_client.h"

#define HALT_TEMPO 20

RemoteIRManager::RemoteIRManager() :
		mosquittopp("IRmanager") {

	progname = "IRmanager";
	configFile = LIRCRC_FILE;

	if (lirc_init((char*) progname.c_str(), 1) == -1)
		exit (EXIT_FAILURE);

	if (lirc_readconfig((char*) configFile.c_str(), &config, NULL) != 0) {
		LOG_ERROR("Error while reading config file : " << configFile);
	}
	connectMQTT();
	loop_start();
}

RemoteIRManager::~RemoteIRManager() {
	lirc_freeconfig(config);
	lirc_deinit();
}

void RemoteIRManager::on_message(const struct mosquitto_message *message) {
	string topic(message->topic);
	string payload(static_cast<char *>(message->payload));
	LOG_ERROR(
			"MQTT event received, topic = " << topic << " and payload = " << payload);
}

int RemoteIRManager::listenIR() {
	char *code;
	char *c;
	int ret;

	LOG_INFORMATION("Config read ");
	while (lirc_nextcode(&code) == 0) {
		LOG_INFORMATION("Some data received : " << code);
		if (code == NULL)
			continue;
		while ((ret = lirc_code2char(config, code, &c)) == 0 && c != NULL) {
			LOG_INFORMATION("Executing command " << c);
			string command(c);
			if (command == "HALT") {
				istringstream inputStream(code);
				int repeat;
				try {
					inputStream >> hex >> repeat; // ignore key code
					inputStream >> hex >> repeat; // get repeat count
				} catch (...) {
					LOG_ERROR(
							"error extracting repeat from " + inputStream.str());
				}
				LOG_INFORMATION("repeat = " << repeat);
				if (repeat > HALT_TEMPO) {
					playSound("byebye.wav");
					system("poweroff");
					return 0;
				}
			}
			int result = publish( NULL, mqttTopics::radioActuator.c_str(),
					command.size(), command.c_str(), 2);
		}
		free(code);
		if (ret == -1)
			break;
	}
	return 0;
}

void RemoteIRManager::connectMQTT() {
	int mqttResult = connect( BROKER_ADDRESS, BROKER_PORT);
	if (mqttResult != MOSQ_ERR_SUCCESS) {
		LOG_ERROR("could not connect to mqtt broker, error : " << mqttResult);
		return;
	}
	if (mqttResult != MOSQ_ERR_SUCCESS) {
		LOG_ERROR("could not subscribe to mqtt broker, error : " << mqttResult);
		return;
	}
	LOG_INFORMATION("Connecté au broker MQTT!");
}
