/*
 * Utils.h
 *
 *  Created on: 10 avr. 2013
 *      Author: leonard
 */

#ifndef UTILS_H_
#define UTILS_H_

#include <string>
#include <vector>
#include <map>

using namespace std;

string GetStdoutFromCommand( string cmd);

vector<string> splitString( const string &s, char delim);

string currentTime();

void readConfigurationFile( const string &fileName, map<string, string> &config);

void playSound(const string& soundFile);

string numberToString( int num);

#define LOG(text) cout << currentTime() << " :\t" << __PRETTY_FUNCTION__ << " :\t" << text << endl

#define BROKER_ADDRESS /*"127.0.0.1"*/ "192.168.0.181"
#define BROKER_PORT 1883

#endif /* UTILS_H_ */
