/*
 * LCDTexts.cpp
 *
 *  Created on: Jul 1, 2013
 *      Author: leonard
 */
#include "LCDTexts.h"
#include "Utils.h"

#define TextKey "text"
#define LineKey "line"
#define PositionKey "position"
#define LengthKey "length"

PingPongText::PingPongText( string text, int line, int position, int length) :
		LCDText(), _offset( 0), _forward( true) {
	_text = text;
	_line = line;
	_position = position;
	_length = length;
}

string PingPongText::update() {
	if (_forward) {
		if (_offset + _length < _text.size()) {
			_offset++;
		} else {
			_forward = false;
			_offset--;
		}
	} else {
		if (_offset > 0) {
			_offset--;
		} else {
			_offset++;
			_forward = true;
		}
	}
	return _text.substr( _offset, _length);
}

Json::Value PingPongText::mqttRepesentation() {
	Json::Value root;
	root[TextKey] = _text;
	root[LineKey] = _line;
	root[PositionKey] = _position;
	root[LengthKey] = _length;
	return root;
}

void PingPongText::changeText( string newText) {

	if (newText.size() < _length) {
		newText.resize( _length + 1, ' ');
	}
	if (newText != _text) {
		_text = newText;
		_offset = 0;
		_forward = true;
	}
}

FixedText::FixedText( string text, int line, int position) :
		LCDText() {
	_text = text;
	_line = line;
	_position = position;
}

string FixedText::update() {
	return _text;
}

Json::Value FixedText::mqttRepesentation() {
	Json::Value root;
	root[TextKey] = _text;
	root[LineKey] = _line;
	root[PositionKey] = _position;
	return root;
}

void FixedText::changeText( string newText) {
	if (_text != newText) {
		_text = newText;
	}
}

RotatingText::RotatingText( string text, int line, int position, int length) :
		LCDText(), _offset( 0) {
	_text = text;
	_line = line;
	_position = position;
	_length = length;
}

string RotatingText::update() {
	string tmp = "";
	if (_offset + _length <= _text.size()) {
		tmp = _text.substr( _offset, _length);
		_offset++;
		return tmp;
	} else if (_offset <= _text.size()) {
		tmp = _text.substr( _offset, _length);
		tmp += _text.substr( 0, _offset + _length - _text.size());
		_offset++;
		return tmp;
	}
	_offset = 0;
	return _text.substr( _offset, _length);
}

Json::Value RotatingText::mqttRepesentation() {
	Json::Value root;
	root[TextKey] = _text;
	root[LineKey] = _line;
	root[PositionKey] = _position;
	root[LengthKey] = _length;
	return root;
}

void RotatingText::changeText( string newText) {
	if (newText.size() < _length) {
		newText.resize( _length + 1, ' ');
	}
	if (_text != newText) {
		_text = newText;
		_offset = 0;
	}
}
