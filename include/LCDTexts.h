/*
 * LCDTexts.h
 *
 *  Created on: Nov 3, 2013
 *      Author: leonard
 */

#ifndef LCDTEXTS_H_
#define LCDTEXTS_H_

#include <string>
#include "jsoncpp/json/json.h"

using namespace std;

#define SCREEN_HEIGHT 4

class LCDText {
public:
	virtual string update()=0;
	virtual ~LCDText() {
	}
	virtual Json::Value mqttRepesentation()=0;
	virtual void changeText( string newText)=0;
	unsigned int _line;
	unsigned int _position;

protected:
	string _text;
};

class FixedText: public LCDText {
public:
	FixedText( string text, int line, int position);
	string update();
	Json::Value mqttRepesentation();
	void changeText( string newText);
};

class RotatingText: public LCDText {
public:
	RotatingText( string text, int line, int position, int length);
	string update();
	Json::Value mqttRepesentation();
	void changeText( string newText);

private:
	unsigned int _length;
	unsigned int _offset;
};

class PingPongText: public LCDText {
public:
	PingPongText( string text, int line, int position, int length);
	string update();
	Json::Value mqttRepesentation();
	void changeText( string newText);

private:
	unsigned int _length;
	unsigned int _offset;
	bool _forward;
};

#endif /* LCDTEXTS_H_ */
