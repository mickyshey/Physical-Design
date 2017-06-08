//	File name [ Router.cpp ]
#include <iostream>
#include <fstream>
#include <string>
#include <cassert>

#include "Router.h"
#include "Util.h"

void Router::readInput(const std::string& inputFile)
{
	std::ifstream file;
	std::string s;
	std::vector<std::string> tokens;

	file.open(inputFile);

	// Boundary = (BLx,BLy), (TRx,TRy)
	std::getline(file, s);
	tokens = getTokens(s, " \t\r\n,()=");
	assert(tokens.size() == 5); assert(tokens[0] == "Boundary");
	_boundaryBL = CPoint(std::stoi(tokens[1]), std::stoi(tokens[2]));
	_boundaryTR = CPoint(std::stoi(tokens[3]), std::stoi(tokens[4]));
	
	// NumPins = _numPins
	std::getline(file, s);
	tokens = getTokens(s, " \t\r\n=");
	assert(tokens.size() == 2); assert(tokens[0] == "NumPins");
	_numPins = std::stoi(tokens[1]);
	
	// PIN pX (px, py)
	while( std::getline(file, s) ) {
		tokens = getTokens(s, " \t\r\n,()");
		assert(tokens.size() == 4); assert(tokens[0] == "PIN");
		_pinName.push_back(tokens[1]);
		_pinCPoint.push_back(CPoint(std::stoi(tokens[2]), std::stoi(tokens[3])));
	}
	assert(_pinName.size() == _numPins); assert(_pinCPoint.size() == _numPins);

	file.close();
}

void Router::reportPin()
{
	std::cout << "info of PINs: " << std::endl;
	for( unsigned i = 0; i < _numPins; ++i ) {
		std::cout << "name: " << _pinName[i] << ", " << _pinCPoint[i] << std::endl;
	}
}
