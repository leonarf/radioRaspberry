/*
 * ScreenManager.cpp
 *
 *  Created on: Jul 1, 2013
 *      Author: leonard
 */
#include <iostream>
#include <sstream>
#include <fstream>
#include <unistd.h>
#include <time.h>
#include "ScreenManager.h"

ScreenManager* ScreenManager::_pInstance = NULL;

ScreenManager::ScreenManager() {
	_firstLine = getTime();
	_secondLine = "";
	_thirdLine = "";
	_fourthLine = "";
	flush();
}

ScreenManager::~ScreenManager() {
	// TODO !CodeTemplates.destructorstub.tododesc!
}

ScreenManager* ScreenManager::instance() {
	if (!_pInstance) {
		_pInstance = new ScreenManager();
	}
	return _pInstance;
}

string ScreenManager::getTime() {
	time_t rawtime = time( NULL);
	struct tm * timeinfo = localtime( &rawtime);
	return asctime( timeinfo);
}

void ScreenManager::start() {
	flush();
}

void ScreenManager::flush() {
	int maxWidth = 15;
	int offset = 0;
	for (int i = 0; i < 100; i++) {
		ostringstream ss;
		ss << i * 99 << "flush content on screen " << getTime();
		string line = ss.str();
		cout << line << endl;
		while (offset + maxWidth < line.size()-1) {
			cout << line.substr( offset, maxWidth) << '\r';
			cout.flush();
			offset++;
			usleep( 100000);
		}
		while (offset >= 0) {
			cout << line.substr( offset, maxWidth) << '\r';
			cout.flush();
			offset--;
			usleep( 100000);
		}
		offset = 0;
		cout << endl;
	}
	ofstream myfile;
	myfile.open( "/home/leonard/workspace/radioCplus/target/screen.txt");
	myfile << _firstLine << endl;
	myfile << _secondLine << endl;
	myfile << _thirdLine << endl;
	myfile << _fourthLine << endl;
	myfile.close();
}

