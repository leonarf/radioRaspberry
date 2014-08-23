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
#define SCROLL_SPEED 300000 // en µs

void LCDManager::on_message( const struct mosquitto_message *message) {
	string topic( message->topic);
	string payload( static_cast<char *>( message->payload));
	LOG_INFORMATION( "MQTT event received, topic = " << topic);
	LOG_INFORMATION( "payload = \n" << payload);
	if (topic == mqttTopics::radioSensor) {
		Json::Reader jsonReader;
		stringstream ss;
		radioPlayingMessage radioMessage;
		if (radioMessage.initFromJson( payload)) {
			LOG_INFORMATION( "Received");
			if (radioMessage._running) {
				if (not _LCDactive) {
					_lcd.backlight();
					_LCDactive = true;
				}
				ss << " (" << (radioMessage._radioNumberPlaying+1) << "/" << radioMessage._radioCount << ")"
						<< radioMessage._radioName;
				if (textExist( RADIO)) {
					changeText( RADIO, ss.str());
				} else {
					newText( RADIO, new RotatingText( ss.str(), 2, 1, LCD_WIDTH));
				}
				if (textExist( SONG)) {
					if (radioMessage._songPlaying.empty()) {
						removeText( SONG);
					} else {
						changeText( SONG, radioMessage._songPlaying);
					}
				} else if (not radioMessage._songPlaying.empty()) {
					newText( SONG, new RotatingText( radioMessage._songPlaying, 3, 1, LCD_WIDTH));
				}
			} else {
				if (_LCDactive) {
					_lcd.noBacklight();
					_LCDactive = false;
				}
			}
			return;
		}
		Json::Value root;
		if (jsonReader.parse( payload, root, false)) {
			Json::Value radioPlayed = root.get( mqttMessages::radioVolume, Json::Value::null);
			if (not radioPlayed.isConvertibleTo( Json::nullValue)) {
				LOG_INFORMATION( "message volume received");
				int volume = radioPlayed.get( (unsigned int) 0, -1).asInt();
				ss << "Vol:" << volume;
				if (textExist( VOLUME)) {
					changeText( VOLUME, ss.str());
				} else {
					newText( VOLUME, new FixedText( ss.str(), 4, LCD_WIDTH - 6));
				}
			}
		}
	}
	return;
}

LCDManager* LCDManager::_pInstance = NULL;

LCDManager::LCDManager()
		: mosquittopp( "LCD"), _lcd( LCD_I2C_ADDRESS, LCD_WIDTH, LCD_HEIGHT) { // set the LCD address to 0x20 for a 20 chars and 4 lines display
	_lcd.init();                      // initialize the lcd

// Print a message to the LCD.
	_lcd.backlight();
	_lcd.cursor_off();
	_lcd.blink_off();
	_lcd.setCursor( 0, 0);
	connectMQTT();
}

LCDManager::~LCDManager() {
	_lcd.noBacklight();
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
		LOG_ERROR( "could not connect to mqtt broker, error : " << mqttResult);
		return;
	}
	mqttResult = subscribe( NULL, mqttTopics::radioSensor.c_str(), 0);
	if (mqttResult != MOSQ_ERR_SUCCESS) {
		LOG_ERROR( "could not subscribe to mqtt broker, error : " << mqttResult);
		return;
	}
}

void LCDManager::update() {
	for (auto& element : _messages) {
		_lcd.setCursor( element.second->_position - 1, element.second->_line - 1);
		_lcd.print( element.second->update().c_str());
	}
}

void LCDManager::start() {
	int mqttResult = loop_start();
	if (mqttResult != MOSQ_ERR_SUCCESS) {
		LOG_ERROR( "could not connect to mqtt broker, error : " << mqttResult);
	}
	stringstream ss;
	char cDate[15]; /* strlen("01/02 18:17:54") + 1 */
	time_t timer = time( NULL);
	tm* tm_info = localtime( &timer);
	strftime( cDate, 20, "%d/%m %H:%M:%S", tm_info);
	ss << cDate;
	newText( TIME, new FixedText( ss.str().substr( 0, 20), 1, 1));
	_running = true;
	while (_running) {
		usleep( SCROLL_SPEED);
		if (_LCDactive) {
			ss.str( "");
			timer = time( NULL);
			tm* tm_info = localtime( &timer);
			strftime( cDate, 20, "%d/%m %H:%M:%S", tm_info);
			ss << cDate;
			changeText( TIME, ss.str().substr( 0, 20));
			if( update_mutex.try_lock()){
				update();
				update_mutex.unlock();
			}
		}
	}
}

void LCDManager::poweredOff() {
	_running = false;
	_lcd.backlight();
	update_mutex.lock();
	_lcd.setCursor(0, 0);
	_lcd.print("######   IL   ######");
	_lcd.setCursor(0, 1);
	_lcd.print("#####   FAUT   #####");
	_lcd.setCursor(0, 2);
	_lcd.print("####     ME     ####");
	_lcd.setCursor(0, 3);
	_lcd.print("###  DEBRANCHER  ###");
}

void LCDManager::newText( MessageId textId, LCDText* text) {
	_messages[textId] = text;
}

bool LCDManager::textExist( MessageId textId) {
	return _messages.count( textId);
}

void LCDManager::changeText( MessageId textId, string text) {
	try {
		update_mutex.lock();
		_messages.at( textId)->changeText( text);
	} catch (const std::out_of_range& oor) {
		LOG_ERROR( "LCDText id=" << textId << " doesn't exist!!!!!!!!!!");
		LOG_ERROR( "Out of Range error: " << oor.what());
	}
	update_mutex.unlock();
}

void LCDManager::removeText( MessageId textId) {
	try {
		changeText(textId, "");
		update_mutex.lock();
		update();
		_messages.erase( textId);
	} catch (const std::out_of_range& oor) {
		LOG_ERROR( "LCDText id=" << textId << " doesn't exist!!!!!!!!!!");
		LOG_ERROR( "Out of Range error: " << oor.what());
	}
	update_mutex.unlock();
}

