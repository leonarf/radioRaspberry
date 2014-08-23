/*
 * RadioManager.h
 *
 *  Created on: 10 avr. 2013
 *      Author: leonard
 */

#ifndef LCDMANAGER_H_
#define LCDMANAGER_H_

#include <vector>
#include <string>
#include <map>
#include <mutex>

#include <mosquittopp.h>
#include <LiquidCrystal_I2C.h>
#include <LCDTexts.h>

using namespace std;
using namespace mosqpp;

typedef enum {
	TIME = 0,
	VOLUME,
	RADIO,
	SONG
} MessageId;

class LCDManager: private mosquittopp {
public:
	void start();
	void poweredOff();
	void newText( MessageId textId, LCDText* text);
	void changeText( MessageId textId, string text);
	void removeText( MessageId textId);
	bool textExist( MessageId textId);

	static LCDManager* instance();
	virtual ~LCDManager();

private:
	LCDManager();
	void connectMQTT();
	void on_message( const struct mosquitto_message *message);
	void update();
	map<MessageId, LCDText*> _messages;
	LiquidCrystal_I2C _lcd;
	static LCDManager* _pInstance;

	bool _LCDactive;
	bool _running;
	mutex update_mutex;
};

#endif /* RADIOMANAGER_H_ */
