//============================================================================
// Name        : mainRadio.cpp
// Author      : leonard
// Version     :
// Copyright   : Your copyright notice
// Description : main() for launching Radio module
//============================================================================
#include "RadioManager.h"

using namespace std;

int main( int argc, char *argv[]) {
	RadioManager::instance()->start();
}
