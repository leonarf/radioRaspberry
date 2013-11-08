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
static const string radioSensor;

};
#endif /* MQTT_TOPICS_H_ */
