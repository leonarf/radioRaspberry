/*
 * ScreenManager.h
 *
 *  Created on: Jul 1, 2013
 *      Author: leonard
 */

#ifndef SCREENMANAGER_H_
#define SCREENMANAGER_H_

using namespace std;

class ScreenManager {
public:
	static ScreenManager* instance();
	virtual ~ScreenManager();
	void start();

private:
	string _firstLine;
	string _secondLine;
	string _thirdLine;
	string _fourthLine;
	void flush();
	string getTime();
	ScreenManager();
	static ScreenManager* _pInstance;
};


#endif /* SCREENMANAGER_H_ */
