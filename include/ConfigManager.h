/*
 * ConfigManager.h
 *
 *  Created on: Jun 13, 2013
 *      Author: leonard
 */

#ifndef CONFIGMANAGER_H_
#define CONFIGMANAGER_H_

#include <string>
using namespace std;

class ConfigManager {
public:
	static ConfigManager* instance();
	virtual ~ConfigManager();
	void setConfigFile( string filePath);
	/*
	 * set directory where the binary is
	 */
	void setBinDir( string path);
	/*
	 * get directory where the binary is
	 */
	string getBinDir();

private:
	string _binaryDirectory;
	ConfigManager();
	static ConfigManager* _pInstance;
};

#endif /* CONFIGMANAGER_H_ */
