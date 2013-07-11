/*
 * ScreenManager.h
 *
 *  Created on: Jul 1, 2013
 *      Author: leonard
 */

#ifndef SCREENMANAGER_H_
#define SCREENMANAGER_H_

#include <array>
#include <vector>

using namespace std;

#define SCREEN_HEIGHT 4

class RotatingText {
public:
	RotatingText(string text, int line, int position, int length);
	void rotate();

private:
	string text;
	unsigned int line;
	unsigned int position;
	unsigned int length;
	unsigned int offset;
	bool forward;
};

class ScreenManager {
	friend RotatingText;
public:
	static ScreenManager* instance();
	virtual ~ScreenManager();
	void start();
	bool setText( string text, unsigned int line, unsigned int position, unsigned int length);

private:
	array<string, SCREEN_HEIGHT> _lines;
	vector<RotatingText> _texts;
	bool _threadRunning;
	void flush();
	string getTime();
	ScreenManager();
	static ScreenManager* _pInstance;
};

#endif /* SCREENMANAGER_H_ */
