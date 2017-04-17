// File name [ main.cpp ]

#include <iostream>
#include <string>
#include <cstdlib>
#include <ctime>

#include "FloorplanMgr.h"

using namespace std;

int main(int argc, char** argv) {
	srand(time(NULL));
	FloorplanMgr* fp = new FloorplanMgr(atof(argv[1]));
	
	fp -> readInput(argv[2], argv[3]);
	fp -> BTreeInit();

	delete fp;
	//fp -> BTreePacking();
	//fp -> reportBTree();
	//fp -> BlockDeleteAndInsert();
	//fp -> BlockSwap();
	//fp -> BTreePacking();
	//fp -> reportBTree();
}
