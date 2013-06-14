/*
 * ConfigManager.cpp
 *
 *  Created on: Jun 13, 2013
 *      Author: leonard
 */

#include "ConfigManager.h"
#include "Utils.h"

ConfigManager* ConfigManager::_pInstance = NULL;

ConfigManager::ConfigManager() :
		_binaryDirectory( "") {

}

ConfigManager::~ConfigManager() {

}

ConfigManager* ConfigManager::instance() {
	if (!_pInstance) {
		_pInstance = new ConfigManager();
	}
	return _pInstance;
}

void ConfigManager::setConfigFile( string filePath) {
	if (filePath[0] != '/') {
		filePath = _binaryDirectory + filePath;
	}
	map<string, string> configMap;
	readConfigurationFile( filePath, configMap);
}

void ConfigManager::setBinDir( string path) {
	_binaryDirectory = path;
}

string ConfigManager::getBinDir() {
	return _binaryDirectory;
}
