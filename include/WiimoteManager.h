/*
 * WiimoteManager.h
 *
 *  Created on: 15 avr. 2013
 *      Author: leonard
 */

#ifndef WIIMOTEMANAGER_H_
#define WIIMOTEMANAGER_H_

#include <wiiuse.h>

using namespace std;

class WiimoteManager {
public:
	WiimoteManager();
	~WiimoteManager();
	bool startHandlingEvent();
	int connect();
	int resync();

private:
	void changeVolume();
	void rumbleAsync( int microsecond);
	void rumbleSync( int microsecond);
	void disconnect();
	void handle_button_event( struct wiimote_t* wm);
	wiimote** _wiimotes;
	bool _changingVolume;
};

#endif /* WIIMOTEMANAGER_H_ */
