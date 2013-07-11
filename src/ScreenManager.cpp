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
#include <thread>

#include "ScreenManager.h"
#include "Utils.h"

#define SCREEN_WIDTH 40

RotatingText::RotatingText( string text, int line, int position, int length) :
		text( text), line( line), position( position), length( length), offset( 0), forward( true) {
}

void RotatingText::rotate() {
	ScreenManager::instance()->_lines[line].replace( position, length, text.substr( offset, length));
	if (forward) {
		if (offset + length < text.size()) {
			offset++;
		} else {
			forward = false;
			offset--;
		}
	} else {
		if (offset > 0) {
			offset--;
		} else {
			offset++;
			forward = true;
		}
	}
}

ScreenManager* ScreenManager::_pInstance = NULL;

ScreenManager::ScreenManager(): _threadRunning(false) {
	for (int i = 0; i < _lines.size(); i++) {
		_lines[i].resize( SCREEN_WIDTH, ' ');
	}
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
	_threadRunning = true;
	ofstream myfile;
	while (not _texts.empty()) {
		myfile.open( "/home/leonard/workspace/radioCplus/target/screen.txt");

		for (int i = 0; i < _texts.size(); i++) {
			_texts[i].rotate();
		}

		for (int i = 0; i < _lines.size(); i++) {
			myfile << _lines[i] << endl;
		}
		usleep( 1100000);
		myfile.close();
	}
	_threadRunning = false;
}

bool ScreenManager::setText( string text, unsigned int line, unsigned int position, unsigned int length) {
	if (line < 1 or line > SCREEN_HEIGHT) {
		LOG( "ERROR : line " << line << " out of range [1-" << SCREEN_HEIGHT << "]");
		return false;
	}
	line = line - 1;
	_texts.push_back( RotatingText( text, line, position, length));
	if (not _threadRunning) {
		thread changingThread( &ScreenManager::flush, this);
		changingThread.detach();
	}
}

