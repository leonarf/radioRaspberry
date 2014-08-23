/*
 * mqttTopics.h
 *
 *  Created on: 7 nov. 2013
 *      Author: leonard
 */

#ifndef MQTT_TOPICS_H_
#define MQTT_TOPICS_H_

#include <string>

using namespace std;

struct mqttTopics {

	static const string actuators;
	static const string sensors;
	static const string radioActuator;
	/**
	 * topic where radio module regularly send it's state with messages:
	 *  - radioPlaying
	 *  - radioVolume
	 */
	static const string radioSensor;

};

struct mqttMessages {
	/**
	 * "radioPlaying" : [ bool running, int radioNumberCurrentlyPlayed, int radioCount, string radioName, string songName ]
	 */
	static const string radioPlaying;
	/**
	 *  "radioVolume" : int volume
	 */
	static const string radioVolume;
};

struct radioPlayingMessage {
	/**
	 * fill radioPlayingMessage structure from a json-styled string
	 * @return true if correctly filled
	 */
	bool initFromJson( string message);
	bool _running;
	int _radioNumberPlaying;
	int _radioCount;
	string _radioName;
	string _songPlaying;
};

#endif /* MQTT_TOPICS_H_ */
