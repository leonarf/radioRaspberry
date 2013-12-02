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
#include <map>
#include <thread>
#include <mpd/playlist.h>
#include <mpd/status.h>
#include <mpd/player.h>
#include <mpd/mixer.h>
#include <mpd/tag.h>

#include "Utils.h"
#include "RadioManager.h"
#include "mqttTopics.h"
#include "jsoncpp/json/json.h"

void RadioManager::on_message( const struct mosquitto_message *message) {
	string topic( message->topic);
	string payload( static_cast<char *>( message->payload));
	LOG( "MQTT event received, topic = " << topic << " and payload = " << payload);
	//send radio module's capabilities as asked
	if (topic == "/home/actuators" and payload == "list") {
		Json::Value root;
		root["start"]["icon"] = "play";
		root["stop"]["icon"] = "pause";
		root["next"]["icon"] = "forward";
		root["previous"]["icon"] = "backward";
		root["plusVolume"]["icon"] = "volume-up";
		root["minusVolume"]["icon"] = "volume-down";
		root["refreshList"]["icon"] = "refresh";
		Json::StyledWriter jsonWriter;
		string values = jsonWriter.write( root);
		cout << "sending : " << values << endl;
		publish( NULL, "/home/actuators/radio/values", values.size(), values.c_str(), 2);
	}
	//do received command
	else if (topic == mqttTopics::radioActuator) {
		LOG( "payload : " << payload);
		Json::Reader jsonReader;
		Json::Value root;
		//string value;
		if (jsonReader.parse( payload, root, false)) {
			int offset = root.get( "VOLUME", 0).asInt();
			if (offset != 0) {
				RadioManager::instance()->changeVolume( offset);
				return;
			}
			playSound( "COUGH1.WAV");
			int radioNumber = root.get( "SETRADIO", -1).asInt();
			if (radioNumber > 0) {
				RadioManager::instance()->setRadio( radioNumber - 1, true);
			}
			offset = root.get( "CHANGERADIO", 0).asInt();
			if (offset > 0) {
				RadioManager::instance()->nextRadio();
			} else if (offset < 0) {
				RadioManager::instance()->prevRadio();
			}
			string newState = root.get( "CHANGESTATE", "").asString();
			if (newState == "play") {
				RadioManager::instance()->startRadio();
			} else if (newState == "stop" || newState == "pause") {
				RadioManager::instance()->stopRadio();
			} else if (newState == "dodge_ad") {
				RadioManager::instance()->stopRadio();
			}
		}
		if (payload == "refreshList") {

		}
	}
	return;
}

RadioManager* RadioManager::_pInstance = NULL;

#define IF_MPD_BREAK(function)\
if (!function) {\
	LOG( #function << " returned false : " << mpd_connection_get_error_message( _mpdConnect));\
	playSound("uh-oh.wav");\
	connectMPD();\
}

RadioManager::RadioManager() :
		mosquittopp( "Radio"), _playedRadio( 0), _running( false), _changingRadio( false), _mpdConnect( NULL) {
	connectMPD();
	connectMQTT();
	//loop_start();
}

RadioManager::~RadioManager() {
	disconnect();
}

RadioManager* RadioManager::instance() {
	if (!_pInstance) {
		_pInstance = new RadioManager();
	}
	return _pInstance;
}

void RadioManager::connectMQTT() {
	int mqttResult = connect( BROKER_ADDRESS, BROKER_PORT);
	if (mqttResult != MOSQ_ERR_SUCCESS) {
		LOG( "could not connect to mqtt broker, error : " << mqttResult);
		return;
	}
	mqttResult = subscribe( NULL, mqttTopics::radioActuator.c_str(), 0);
	if (mqttResult != MOSQ_ERR_SUCCESS) {
		LOG( "could not subscribe to mqtt broker, error : " << mqttResult);
		return;
	}
}

void RadioManager::connectMPD() {
	if (_mpdConnect != NULL) {
		mpd_connection_free( _mpdConnect);
	}
	while (1) {
		_mpdConnect = mpd_connection_new( NULL, 0, 30000);
		if (mpd_connection_get_error( _mpdConnect) == MPD_ERROR_SUCCESS) {
			break;
		}
		LOG( "Failed to connect to MPD because : " << mpd_connection_get_error_message( _mpdConnect));
		mpd_connection_free( _mpdConnect);
		playSound( "uh-oh.wav");
		sleep( 5);
	}
	//init playlist list
	IF_MPD_BREAK( mpd_send_list_queue_meta( _mpdConnect));
	mpd_song* oneRadio;
	while ((oneRadio = mpd_recv_song( _mpdConnect)) != NULL) {
		const char *value;
		value = mpd_song_get_tag( oneRadio, MPD_TAG_NAME, 0);
		if (value == NULL) {
			value = mpd_song_get_uri( oneRadio);
		}
		LOG( value << " with ID=" << mpd_song_get_id(oneRadio));
		_radioList.push_back( value);
		mpd_song_free( oneRadio);
	}

	map<string, int> tmp;
	for (int i = 0; i < _radioList.size(); i++) {
		if (tmp.count( _radioList.at( i)) > 0) {
			LOG( "radio :" << _radioList[i] << " en double");
		} else {
			LOG( i << ") " << _radioList[i]);
			tmp[_radioList[i]] = i;
		}
	}

	//init mpc playing what
	mpd_status * status = mpd_run_status( _mpdConnect);
	mpd_state currentState = mpd_status_get_state( status);
	LOG( "Current MPD state is " << currentState);
	if (currentState == MPD_STATE_PLAY) {
		_running = true;
	} else {
		_running = false;
	}
	_playedRadio = mpd_status_get_song_pos( status);
	LOG( "Played radio is (" << _playedRadio << ")" << _radioList[_playedRadio]);
	_volume = mpd_status_get_volume( status);
	mpd_status_free( status);
}

void RadioManager::prevRadio() {
	setRadio( _playedRadio - 1, false);
}

void RadioManager::nextRadio() {
	setRadio( _playedRadio + 1, false);
}

void RadioManager::dodgeAd() {
	if (not _dodgingAd) {
		std::thread dodgingAdThread( &RadioManager::changingRadio, this);
		dodgingAdThread.detach();
	}
}

void RadioManager::callbackAfterAd() {
}

void RadioManager::stopRadio() {
	if (_running) {
		IF_MPD_BREAK( mpd_run_stop( _mpdConnect)) else {
			_running = false;
		}
		broadcastRadioInfo();
	}
}

void RadioManager::startRadio() {
	if (not _running) {
		IF_MPD_BREAK( mpd_run_play( _mpdConnect)) else {
			_running = true;
		}
		broadcastRadioInfo();
	}
}

void RadioManager::setRadio( int radioNumber, bool changeDirectly) {
	if (radioNumber < 0)
		_playedRadio = _radioList.size() - 1;
	else if (radioNumber >= _radioList.size())
		_playedRadio = 0;
	else {
		_playedRadio = radioNumber;
	}
	if (not _changingRadio) {
		if (changeDirectly) {
			if (_running) {
				IF_MPD_BREAK( mpd_run_stop( _mpdConnect));
			}
			IF_MPD_BREAK( mpd_run_play_pos( _mpdConnect, _playedRadio));
			_running = true;
			broadcastRadioInfo();
		} else {
			std::thread changingThread( &RadioManager::changingRadio, this);
			changingThread.detach();
		}
	}

}

bool RadioManager::changeVolume( int offset) {
	offset += _volume;
	if (offset > 100) {
		if (_volume == 100) {
			return false;
		}
		offset = 100;
	} else if (offset < 0) {
		if (_volume == 0) {
			return false;
		}
		offset = 0;
	}
	IF_MPD_BREAK( mpd_run_set_volume( _mpdConnect, offset)) else {
		_volume = offset;
	}
	return true;
}

void RadioManager::start() {
	std::thread broadcastRadioInfoThread( &RadioManager::broadcastRadioInfoThread, this);
	_stopModuleAsked = false;
	while (not _stopModuleAsked) {
		int result = loop();
		if (result != MOSQ_ERR_SUCCESS) {
			LOG( "ERROR : loop result was : " << result);
			sleep( 1);
		}
	}
	broadcastRadioInfoThread.join();
}

void RadioManager::changingRadio() {
	if (_changingRadio) {
		return;
	}
	_changingRadio = true;
	int counter = 0;
	int oneLoop = 50000; //µs
	int totalWait = 500000; //µs
	/*totalWait µs /oneLoop µs*/
	int tempo = totalWait / oneLoop;
	int localradio = _playedRadio;
	while (counter < tempo) {
		counter++;
		if (localradio != _playedRadio) {
			counter = 0;
			localradio = _playedRadio;
		}
		usleep( oneLoop);
	}
	LOG( "radio : [" << _playedRadio << "]" << _radioList[_playedRadio]);
	if (_running) {
		IF_MPD_BREAK( mpd_run_stop( _mpdConnect));
	}
	IF_MPD_BREAK( mpd_run_play_pos( _mpdConnect, _playedRadio));
	_running = true;
	broadcastRadioInfo();
	_changingRadio = false;
}

void RadioManager::broadcastRadioInfoThread() {
	while (not _stopModuleAsked) {
		broadcastRadioInfo();
		sleep( 10);
	}
}

void RadioManager::broadcastRadioInfo() {
	LOG( " broadcast called");
	Json::Value root;
	root["radioPlayed"].append( _running);
	root["radioPlayed"].append( _playedRadio);
	Json::StyledWriter jsonWriter;
	mpd_song* oneRadio = mpd_run_get_queue_song_id( _mpdConnect, _playedRadio);
	if (oneRadio != NULL) {
		const char *value;
		//add radio name
		if ((value = mpd_song_get_tag( oneRadio, MPD_TAG_NAME, 0)) != NULL) {
			LOG( "RADIONAME FOUND : " << value);
			root["radioPlayed"].append( string( value));
		}
		//add artist name
		if ((value = mpd_song_get_tag( oneRadio, MPD_TAG_TITLE, 0)) != NULL) {
			root["radioPlayed"].append( string( value));
		}
		for (int tag = MPD_TAG_UNKNOWN; tag < MPD_TAG_COUNT; ++tag) {
			int i = 0;
			while ((value = mpd_song_get_tag( oneRadio, (mpd_tag_type) tag, i)) != NULL) {
				LOG( "tag:" << tag << "; valeur:" << i << " : " << value);
				i++;
			}
		}
	}
	mpd_song_free( oneRadio);
	string playedRadio = jsonWriter.write( root);
	LOG( "sending : " << playedRadio);
	publish( NULL, mqttTopics::radioSensor.c_str(), playedRadio.size(), playedRadio.c_str(), 2);
}

