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

#include "Utils.h"
#include "RadioManager.h"

RadioManager* RadioManager::_pInstance = NULL;

#define IF_MPD_BREAK(function)\
if (!function) {\
	LOG( #function << " returned false : " << mpd_connection_get_error_message( _mpdConnect));\
	playSound("uh-oh.wav");\
	connectMPD();\
}

RadioManager::RadioManager() :
		_playedRadio( 0), _running( false), _changingRadio( false), _mpdConnect( NULL) {
	connectMPD();
	IF_MPD_BREAK(false);
	//init playlist list
	IF_MPD_BREAK( mpd_send_list_queue_meta( _mpdConnect));
	mpd_song* oneRadio;
	while ((oneRadio = mpd_recv_song( _mpdConnect)) != NULL) {
		const char *value;
		value = mpd_song_get_tag( oneRadio, MPD_TAG_NAME, 0);
		if (value == NULL) {
			value = mpd_song_get_uri( oneRadio);
		}
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
	if (currentState == MPD_STATE_PLAY) {
		_running = true;
	}
	_playedRadio = mpd_status_get_song_pos( status);
	_volume = mpd_status_get_volume( status);
	mpd_status_free( status);
}

RadioManager::~RadioManager() {
	// TODO !CodeTemplates.destructorstub.tododesc!
}

RadioManager* RadioManager::instance() {
	if (!_pInstance) {
		_pInstance = new RadioManager();
	}
	return _pInstance;
}

void RadioManager::connectMPD() {
	if (_mpdConnect != NULL) {
		mpd_connection_free( _mpdConnect);
	}
	_mpdConnect = mpd_connection_new( NULL, 0, 30000);
	if (mpd_connection_get_error( _mpdConnect) != MPD_ERROR_SUCCESS) {
		LOG( mpd_connection_get_error_message( _mpdConnect));
		mpd_connection_free( _mpdConnect);
	}
}

void RadioManager::prevRadio() {
	setRadio( _playedRadio - 1);
}

void RadioManager::nextRadio() {
	setRadio( _playedRadio + 1);
}

void RadioManager::stopRadio() {
	if (_running) {
		IF_MPD_BREAK( mpd_run_stop( _mpdConnect))
		else {
			_running = false;
		}
	}
}

void RadioManager::startRadio() {
	if (not _running) {
		IF_MPD_BREAK( mpd_run_play( _mpdConnect))
		else {
			_running = true;
		}
	}
}

void RadioManager::setRadio( int radioNumber) {
	if (radioNumber < 0)
		_playedRadio = _radioList.size() - 1;
	else if (radioNumber >= _radioList.size())
		_playedRadio = 0;
	else {
		_playedRadio = radioNumber;
	}
	if (not _changingRadio) {
		std::thread changingThread( &RadioManager::changingRadio, this);
		changingThread.detach();
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
	IF_MPD_BREAK( mpd_run_play_pos( _mpdConnect, _playedRadio));
	_changingRadio = false;
}
