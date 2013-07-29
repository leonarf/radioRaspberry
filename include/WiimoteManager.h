/*
 * WiimoteManager.h
 *
 *  Created on: 15 avr. 2013
 *      Author: leonard
 */

#ifndef WIIMOTEMANAGER_H_
#define WIIMOTEMANAGER_H_

#include <wiiuse.h>
#include <mosquittopp.h>

using namespace std;
using namespace mosqpp;

class WiimoteManager : private mosquittopp {
public:
	WiimoteManager();
	~WiimoteManager();
	bool startHandlingEvent();
	int wiiConnect();
	int resync();

private:
	void on_message(const struct mosquitto_message *message);
	void changeVolume();
	void rumbleAsync( int microsecond);
	void rumbleSync( int microsecond);
	void disconnect();
	void handle_button_event( struct wiimote_t* wm);
	wiimote** _wiimotes;
	bool _changingVolume;
};

#endif /* WIIMOTEMANAGER_H_ */
