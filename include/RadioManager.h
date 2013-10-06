/*
 * RadioManager.h
 *
 *  Created on: 10 avr. 2013
 *      Author: leonard
 */

#ifndef RADIOMANAGER_H_
#define RADIOMANAGER_H_

#include <vector>
#include <string>

#include <mpd/client.h>
#include <mosquittopp.h>

using namespace std;
using namespace mosqpp;

class RadioManager : private mosquittopp {
public:
	void start();
	void setRadio( int radioNumber);
	void startRadio();
	void stopRadio();
	void prevRadio();
	void nextRadio();
	/*
	 * return false if couldn't change volume because max or min already reach
	 */
	bool changeVolume( int offset);

	static RadioManager* instance();
	virtual ~RadioManager();

private:
	RadioManager();
	void changingRadio();
	static void callChangingRadio();
	void connectMPD();
	void connectMQTT();
	void on_message(const struct mosquitto_message *message);
	vector<string> _radioList;
	int _playedRadio;
	int _volume;
	bool _running;
	bool _changingRadio;
	bool _stopModuleAsked;
	static RadioManager* _pInstance;
	struct mpd_connection *_mpdConnect;
};

#endif /* RADIOMANAGER_H_ */
