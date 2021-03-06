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
#include <sstream>
#include <syslog.h>

using namespace std;

#ifndef SOUND_DIR //should be define by cmake
	#define SOUND_DIR "erreur"
#endif

#ifndef LIRCRC_FILE //should be define by cmake
	#define LIRCRC_FILE "erreur"
#endif

#ifndef LOG_FILE //should be define by cmake
	#define LOG_FILE "erreur"
#endif

#define BROKER_ADDRESS "127.0.0.1" /*"192.168.0.181"*/
#define BROKER_PORT 1883

string GetStdoutFromCommand( string cmd);

vector<string> splitString( const string &s, char delim);

string currentTime();

void readConfigurationFile( const string &fileName, map<string, string> &config);

void playSound(const string& soundFile);

void logToSyslog( int priority, string text);

static stringstream logFile;

#define LOG_ERROR(text) LOG(LOG_ERR, "[ERROR]" << text)
#define LOG_INFORMATION(text) LOG(LOG_INFO, "[INFO]" << text)
#define LOG(priority, text) 	logFile.str("");\
	logFile << __PRETTY_FUNCTION__ << " :" << text;\
	logToSyslog(priority, logFile.str())

void daemonize(const char* pidFilePath);

#endif /* UTILS_H_ */
