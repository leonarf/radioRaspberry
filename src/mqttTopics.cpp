#include <mqttTopics.h>
#include "jsoncpp/json/json.h"

//mqtt topics
const string mqttTopics::actuators = "/home/actuator/";
const string mqttTopics::sensors = "/home/sensor/";
const string mqttTopics::radioActuator = actuators + "radio";
const string mqttTopics::radioSensor = sensors + "radio";

//mqtt Messages
const string mqttMessages::radioPlaying = "playing";
const string mqttMessages::radioVolume = "volume";

bool radioPlayingMessage::initFromJson( string message) {
	Json::Reader jsonReader;
	Json::Value root;
	if (jsonReader.parse( message, root, false)) {
		Json::Value radioPlayed = root.get( mqttMessages::radioPlaying, Json::Value::null);
		if (not radioPlayed.isConvertibleTo( Json::nullValue)) {
			_running = radioPlayed.get( (unsigned int) 0, false).asBool();
			_radioNumberPlaying = radioPlayed.get( (unsigned int) 1, -1).asInt();
			_radioCount = radioPlayed.get( (unsigned int) 2, -1).asInt();
			_radioName = radioPlayed.get( (unsigned int) 3, "").asString();
			_songPlaying = radioPlayed.get( (unsigned int) 4, "").asString();
			return true;
		}
	}
	return false;
}
