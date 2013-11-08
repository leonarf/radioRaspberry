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
	void setRadio( int radioNumber, bool changeDirectly);
	void startRadio();
	void stopRadio();
	void prevRadio();
	void nextRadio();
	/**@brief change radio for 30sec before coming back to the previous radio
	 *
	 */
	void dodgeAd();
	/*
	 * return false if couldn't change volume because max or min already reach
	 */
	bool changeVolume( int offset);

	static RadioManager* instance();
	virtual ~RadioManager();

private:
	RadioManager();
	void changingRadio();
	void broadcastRadioInfo();
	void broadcastRadioInfoThread();
	static void callChangingRadio();
	static void callbackAfterAd();
	void connectMPD();
	void connectMQTT();
	void on_message(const struct mosquitto_message *message);
	vector<string> _radioList;
	int _playedRadio;
	int _volume;
	bool _running;
	bool _changingRadio;
	bool _dodgingAd;
	bool _stopModuleAsked;
	static RadioManager* _pInstance;
	struct mpd_connection *_mpdConnect;
};

#endif /* RADIOMANAGER_H_ */
