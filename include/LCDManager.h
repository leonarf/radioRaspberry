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

#include <mosquittopp.h>
#include <LiquidCrystal_I2C.h>
#include <LCDTexts.h>

using namespace std;
using namespace mosqpp;

class LCDManager: private mosquittopp {
public:
	void start();
	int setText( LCDText* text);
	void changeText( int textId, string text);
	void removeText( int textId);

	static LCDManager* instance();
	virtual ~LCDManager();

private:
	LCDManager();
	void connectMQTT();
	void on_message( const struct mosquitto_message *message);
	void update();
	int _nextFreeId;
	vector<int> _textIds;
	map<int, LCDText*> _messages;
	LiquidCrystal_I2C _lcd;
	static LCDManager* _pInstance;

	int _radioPlayedId;
	int _titlePlayedId;
	bool _LCDactive;
};

#endif /* RADIOMANAGER_H_ */
