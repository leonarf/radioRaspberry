/*
 * Utils.cpp
 *
 *  Created on: 10 avr. 2013
 *      Author: leonard
 */

#include "Utils.h"
#include "ConfigManager.h"

#include <stdio.h>
#include <stdlib.h>
#include <sstream>
#include <time.h>
#include <fstream>
#include <boost/algorithm/string.hpp>

namespace ba = boost::algorithm;

string GetStdoutFromCommand( string cmd) {
	string data;
	FILE * stream;
	const int max_buffer = 256;
	char buffer[max_buffer];
	cmd.append( " 2>&1");

	stream = popen( cmd.c_str(), "r");
	if (stream) {
		while (!feof( stream))
			if (fgets( buffer, max_buffer, stream) != NULL)
				data.append( buffer);
		pclose( stream);
	}
	return data;
}

vector<string> splitString( const string &s, char delim) {
	vector<string> elems;
	stringstream ss( s);
	string item;
	while (getline( ss, item, delim)) {
		elems.push_back( item);
	}
	return elems;
}

string currentTime() {
	char cDate[10]; /* strlen("18:17:54") + 1 */
	time_t timer;
	time( &timer);
	struct tm* tm_info = localtime( &timer);
	strftime( cDate, 20, "%H:%M:%S", tm_info);
	return string( cDate);
}

void readConfigurationFile( const string &fileName, map<string, string> &config) {
	ifstream inFile( fileName);
	string line;
	while (getline( inFile, line)) {
		ba::trim( line);
		if (line.find( '#') == string::npos and not line.empty()) {
			vector<string> vect = splitString( line, '=');
			if (vect.size() == 2) {
				ba::trim( vect[0]);
				ba::trim( vect[1]);
				config[vect[0]] = vect[1];
			}
		}
	}
}

void playSound( const string& soundFile) {
	string tmp( "play " + string(SOUND_DIR) + soundFile);
	system( tmp.c_str());
}

//Converting a number to string
string numberToString( int num) {
	ostringstream strout;
	strout << num;
	return strout.str();
}
