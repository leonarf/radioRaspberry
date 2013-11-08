/*
 * RadioManager.cpp
 *
 *  Created on: 10 avr. 2013
 *      Author: leonard
 */
#include <stdlib.h>
#include <unistd.h>
#include <iostream>
#include <sstream>
#include <thread>

#include "Utils.h"
#include "LCDManager.h"
#include "mqttTopics.h"
#include "jsoncpp/json/json.h"

#define LCD_WIDTH 20
#define LCD_HEIGHT 4
#define LCD_I2C_ADDRESS 0x20

void LCDManager::on_message( const struct mosquitto_message *message) {
	string topic( message->topic);
	string payload( static_cast<char *>( message->payload));
	LOG( "MQTT event received, topic = " << topic << " and payload = " << payload);
//send radio module's capabilities as asked
	if (topic == mqttTopics::radioSensor) {
		LOG( "payload : " << payload);
		Json::Reader jsonReader;
		Json::Value root;
		if (jsonReader.parse( payload, root, false)) {
			Json::Value radioPlayed = root.get( "radioPlayed", Json::Value::null);
			LOG( "Found radioPlayed=" << radioPlayed.toStyledString());
			if (not radioPlayed.isConvertibleTo( Json::nullValue)) {
				bool running = radioPlayed.get( (unsigned int) 0, false).asBool();
				int radioNumber = radioPlayed.get( (unsigned int) 1, -1).asInt();
				string radioName = radioPlayed.get( (unsigned int) 2, "Radio name not found").asString();
				string songTitle = radioPlayed.get( (unsigned int) 3, " --- --- --- ---").asString() + " --- ";
				stringstream ss;
				if (running) {
					_lcd.backlight();
					ss << " (" << radioNumber << ")" << radioName << " --- ";
					if (_radioPlayedId == 0) {
						LOG( "Creating RotatingText with : " << ss.str());
						_radioPlayedId = setText( new RotatingText( ss.str(), 2, 1, LCD_WIDTH));
						_titlePlayedId = setText( new RotatingText( songTitle, 3, 1, LCD_WIDTH));
					} else {
						LOG( "Changing RotatingText with : " << ss.str());
						changeText( _radioPlayedId, ss.str());
						changeText( _titlePlayedId, songTitle);
					}
				} else {
					_lcd.noBacklight();
				}
			}
		}
	}
	return;
}

LCDManager* LCDManager::_pInstance = NULL;

LCDManager::LCDManager() :
		mosquittopp( "LCD"), _lcd( LCD_I2C_ADDRESS, LCD_WIDTH, LCD_HEIGHT), _nextFreeId( 1), _radioPlayedId( 0) { // set the LCD address to 0x20 for a 20 chars and 4 lines display
	_lcd.init();                      // initialize the lcd

// Print a message to the LCD.
	_lcd.backlight();
	_lcd.cursor_off();
	_lcd.blink_off();
	_lcd.setCursor( 0, 0);
	connectMQTT();
}

LCDManager::~LCDManager() {
	disconnect();
}

LCDManager* LCDManager::instance() {
	if (!_pInstance) {
		_pInstance = new LCDManager();
	}
	return _pInstance;
}

void LCDManager::connectMQTT() {
	int mqttResult = connect( BROKER_ADDRESS, BROKER_PORT);
	if (mqttResult != MOSQ_ERR_SUCCESS) {
		LOG( "could not connect to mqtt broker, error : " << mqttResult);
		return;
	}
	mqttResult = subscribe( NULL, mqttTopics::radioSensor.c_str(), 0);
	if (mqttResult != MOSQ_ERR_SUCCESS) {
		LOG( "could not subscribe to mqtt broker, error : " << mqttResult);
		return;
	}
}

void LCDManager::update() {
	auto iter = _messages.begin();
	for (; iter != _messages.end(); ++iter) {
		_lcd.setCursor( iter->second->_position - 1, iter->second->_line - 1);
		_lcd.print( iter->second->update().c_str());
	}
}

void LCDManager::start() {
	int mqttResult = loop_start();
	if (mqttResult != MOSQ_ERR_SUCCESS) {
		LOG( "could not connect to mqtt broker, error : " << mqttResult);
	}
	stringstream ss;
	time_t result = time( NULL);
	ss << asctime( localtime( &result));
	int textId = setText( new FixedText( ss.str().substr( 0, 20), 1, 1));
	while (1) {
		usleep( 500000);
		ss.str( "");
		result = time( NULL);
		ss << asctime( localtime( &result));
		changeText( textId, ss.str().substr( 0, 20));
		update();
	}
}

int LCDManager::setText( LCDText* text) {
	_messages[_nextFreeId] = text;
	return _nextFreeId++;
}

void LCDManager::changeText( int textId, string text) {
	try {
		_messages.at( textId)->changeText( text);
	} catch (const std::out_of_range& oor) {
		LOG( "LCDText id=" << textId << " doesn't exist!!!!!!!!!!");
		LOG( "Out of Range error: " << oor.what());
	}
}

void LCDManager::removeText( int textId) {

}

