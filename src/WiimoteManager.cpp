#include "WiimoteManager.h"
#include "RadioManager.h"
#include "Utils.h"

#include <unistd.h>
#include <iostream>
#include <thread>
#include "jsoncpp/json/json.h"

#define MAX_WIIMOTES 2
#define DECO_TIMEOUT 60 // en secondes
void WiimoteManager::on_message( const struct mosquitto_message *message) {
	string topic( message->topic);
	LOG( "MQTT event received, topic = " << topic);
	if (topic == "/home/actuators") {
		string payload( static_cast<char *>( message->payload));
		if (payload == "list") {
			sleep( 1);
			Json::Value root;
			root[1]["icon"] = "lightbulb";
			root[2]["icon"] = "lightbulb";
			root[3]["icon"] = "lightbulb";
			root[4]["icon"] = "lightbulb";
			Json::StyledWriter jsonWriter;
			string values = jsonWriter.write( root);
			cout << "sending : " << values << endl;
			publish( NULL, "/home/actuators/radio/values", values.size(), values.c_str(), 2);
		}
	}
	else if (topic == "/home/actuators/radio"){
		int payload = *static_cast<int *>( message->payload);
		switch (payload) {
			case 1:
				wiiuse_set_leds( _wiimotes[0], WIIMOTE_LED_1);
				break;
			case 2:
				wiiuse_set_leds( _wiimotes[0], WIIMOTE_LED_2);
				break;
			case 3:
				wiiuse_set_leds( _wiimotes[0], WIIMOTE_LED_3);
				break;
			case 4:
				wiiuse_set_leds( _wiimotes[0], WIIMOTE_LED_4);
				break;
			default:
				break;
		}

	}
	return;
}

WiimoteManager::WiimoteManager() :
		_changingVolume( false) {
	/*
	 *	Initialize an array of wiimote objects.
	 *
	 *	The parameter is the number of wiimotes I want to create.
	 */
	_wiimotes = wiiuse_init( MAX_WIIMOTES);
	RadioManager::instance()->changeVolume( 0);
	int mqttResult = connect( BROKER_ADDRESS, BROKER_PORT);
	if (mqttResult != MOSQ_ERR_SUCCESS) {
		LOG( "could not connect to mqtt broker, error : " << mqttResult);
		return;
	}
	mqttResult = subscribe( NULL, "/home/actuators", 2);
	if (mqttResult != MOSQ_ERR_SUCCESS) {
		LOG( "could not subscribe to mqtt broker, error : " << mqttResult);
		return;
	}
	loop_start();
}

WiimoteManager::~WiimoteManager() {
	/*
	 *	Disconnect the wiimotes
	 */
	wiiuse_cleanup( _wiimotes, MAX_WIIMOTES);
}

int WiimoteManager::wiiConnect() {
	int found, connected;
	/*
	 *	Find wiimote devices
	 *
	 *	Now we need to find some wiimotes.
	 *	Give the function the wiimote array we created, and tell it there
	 *	are MAX_WIIMOTES wiimotes we are interested in.
	 *
	 *	Set the timeout to be 5 seconds.
	 *
	 *	This will return the number of actual wiimotes that are in discovery mode.
	 */
	found = wiiuse_find( _wiimotes, MAX_WIIMOTES, 5);
	if (!found) {
		printf( "No wiimotes found.\n");
		return 0;
	}

	/*
	 *	Connect to the wiimotes
	 *
	 *	Now that we found some wiimotes, connect to them.
	 *	Give the function the wiimote array and the number
	 *	of wiimote devices we found.
	 *
	 *	This will return the number of established connections to the found wiimotes.
	 */
	connected = wiiuse_connect( _wiimotes, MAX_WIIMOTES);
	if (connected)
		printf( "Connected to %i wiimotes (of %i found).\n", connected, found);
	else {
		printf( "Failed to connect to any wiimote.\n");
		return 0;
	}

	/*
	 *	Now set the LEDs and rumble for a second so it's easy
	 *	to tell which wiimotes are connected (just like the wii does).
	 */
	wiiuse_set_leds( _wiimotes[0], WIIMOTE_LED_1);
	rumbleAsync( 200000);
	return connected;
}

void WiimoteManager::disconnect() {
	wiiuse_disconnect( _wiimotes[0]);
	wiiuse_disconnected( _wiimotes[0]);
}

int WiimoteManager::resync() {
	int connected = wiiuse_connect( _wiimotes, MAX_WIIMOTES);
	if (connected)
		printf( "Connected to %i wiimotes.\n", connected);
	else {
		printf( "Failed to connect to any wiimote.\n");
	}
	wiiuse_resync( _wiimotes[0]);
	return 0;
}

bool WiimoteManager::startHandlingEvent() {
	int oneLoop = 200000; //µs
	int timeout = (DECO_TIMEOUT * 1000000) / oneLoop;
	int counter = 0;
	while (counter < timeout) {
		counter++;
		while (wiiuse_poll( _wiimotes, MAX_WIIMOTES)) {
			cout << "treating an event!!" << endl;
			counter = 0;
			switch (_wiimotes[0]->event) {
			case WIIUSE_EVENT:
				/* a generic event occured */
				handle_button_event( _wiimotes[0]);
				break;

			case WIIUSE_STATUS:
				/* a status event occured */
				//handle_ctrl_status(wiimotes[i]);
				LOG( "a status event occured");
				break;

			case WIIUSE_DISCONNECT:
				/* the wiimote disconnected
				 * exit function
				 */
				/*
				 *	Disconnect the wiimotes
				 */
				LOG( "Expected disconnet event received");
				rumbleSync( 200000);
				disconnect();
				return true;
			case WIIUSE_UNEXPECTED_DISCONNECT:
				/* the wiimote disconnected
				 * exit function
				 */
				/*
				 *	Disconnect the wiimotes
				 */
				LOG( "Unexpected disconnet event received");
				rumbleSync( 200000);
				disconnect();
				return false;
			case WIIUSE_READ_DATA:
				/*
				 *	Data we requested to read was returned.
				 *	Take a look at wiimotes[i]->read_req
				 *	for the data.
				 */
				break;

			case WIIUSE_NUNCHUK_INSERTED:
				/*
				 *	a nunchuk was inserted
				 *	This is a good place to set any nunchuk specific
				 *	threshold values.  By default they are the same
				 *	as the wiimote.
				 */
				//wiiuse_set_nunchuk_orient_threshold((struct nunchuk_t*)&wiimotes[i]->exp.nunchuk, 90.0f);
				//wiiuse_set_nunchuk_accel_threshold((struct nunchuk_t*)&wiimotes[i]->exp.nunchuk, 100);
				printf( "Nunchuk inserted.\n");
				break;

			case WIIUSE_CLASSIC_CTRL_INSERTED:
				printf( "Classic controller inserted.\n");
				break;

			case WIIUSE_GUITAR_HERO_3_CTRL_INSERTED:
				/* some expansion was inserted */
				//handle_ctrl_status(wiimotes[i]);
				printf( "Guitar Hero 3 controller inserted.\n");
				break;

			case WIIUSE_NUNCHUK_REMOVED:
			case WIIUSE_CLASSIC_CTRL_REMOVED:
			case WIIUSE_GUITAR_HERO_3_CTRL_REMOVED:
				/* some expansion was removed */
				//handle_ctrl_status(wiimotes[i]);
				printf( "An expansion was removed.\n");
				break;

			default:
				break;
			}
		}
		usleep( oneLoop);
	}
	rumbleSync( 200000);
	disconnect();
	return false;
}

void WiimoteManager::handle_button_event( struct wiimote_t* wm) {
	/* if a button is pressed, report it */
	if (IS_PRESSED(wm, WIIMOTE_BUTTON_A)) {
		LOG( "A pressed : start play");
		RadioManager::instance()->startRadio();
	}
	if (IS_PRESSED(wm, WIIMOTE_BUTTON_B)) {
		LOG( "B pressed : stop play");
		RadioManager::instance()->stopRadio();
	}
	if (IS_PRESSED(wm, WIIMOTE_BUTTON_UP)) {
		RadioManager::instance()->nextRadio();
	}
	if (IS_PRESSED(wm, WIIMOTE_BUTTON_DOWN)) {
		RadioManager::instance()->prevRadio();
	}
	if (IS_PRESSED(wm, WIIMOTE_BUTTON_LEFT)) {
		LOG( "LEFT pressed : Battery = " << wm->battery_level * 100 << "%");
	}
	if (IS_PRESSED(wm, WIIMOTE_BUTTON_RIGHT)) {
		LOG( "RIGHT pressed : Battery = " << wm->battery_level * 100 << "%");
	}
	if ((IS_JUST_PRESSED(wm, WIIMOTE_BUTTON_PLUS) or IS_JUST_PRESSED(wm, WIIMOTE_BUTTON_MINUS))and not _changingVolume){
		std::thread changingVolume( &WiimoteManager::changeVolume, this);
		changingVolume.detach();
	}

	if (IS_PRESSED(wm, WIIMOTE_BUTTON_ONE)) {
		RadioManager::instance()->setRadio( 0);
	}
	if (IS_PRESSED(wm, WIIMOTE_BUTTON_TWO)) {
		RadioManager::instance()->setRadio( 1);
	}
	if (IS_PRESSED(wm, WIIMOTE_BUTTON_HOME))
		wiiuse_status( wm);
}

void WiimoteManager::rumbleAsync( int microsecond) {
	std::thread rumbleIt( &WiimoteManager::rumbleSync, this, microsecond);
	rumbleIt.detach();
}

void WiimoteManager::rumbleSync( int microsecond) {
	wiiuse_rumble( _wiimotes[0], 1);
	usleep( microsecond);
	wiiuse_rumble( _wiimotes[0], 0);
}

void WiimoteManager::changeVolume() {
	_changingVolume = true;
	bool result;
	int offsetVolume = 0;
	while (_changingVolume) {
		if (IS_PRESSED(_wiimotes[0], WIIMOTE_BUTTON_PLUS)) {
			if (IS_PRESSED(_wiimotes[0], WIIMOTE_BUTTON_MINUS)) {
				//both PLUS and MINUS buttons are pressed -> doing nothing, exit
				break;
			}
			offsetVolume = 1;
		} else if (IS_PRESSED(_wiimotes[0], WIIMOTE_BUTTON_MINUS)) {
			offsetVolume = -1;
		} else {
			//no more buttons held
			break;
		}
		LOG( "change volume offset = " << offsetVolume);
		result = RadioManager::instance()->changeVolume( offsetVolume);
		if (!result) {
			rumbleAsync( 200000);
			break;
		}
		usleep( 200000);
	}
	_changingVolume = false;
}

