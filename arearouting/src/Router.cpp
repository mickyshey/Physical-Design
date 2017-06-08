//	File name [ Router.cpp ]
#include <iostream>
#include <fstream>
#include <string>
#include <cassert>
#include <algorithm>

#include "Router.h"
#include "Util.h"

bool sortPin(Pin* a, Pin* b) { return a -> getCoordinate() < b -> getCoordinate(); }
bool sortEdge(Edge* a, Edge* b) { return a -> getWeight() < b -> getWeight(); }

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
	
	_pinList.reserve(_numPins);
	// PIN pX (px, py)
	while( std::getline(file, s) ) {
		tokens = getTokens(s, " \t\r\n,()");
		assert(tokens.size() == 4); assert(tokens[0] == "PIN");
		Pin* p = new Pin(tokens[1], CPoint(std::stoi(tokens[2]), std::stoi(tokens[3])));
		_pinList.push_back(p);
	}
	assert(_pinList.size() == _numPins);

	//reportPin();
	file.close();
}

void Router::OASG()
{
	std::sort(_pinList.begin(), _pinList.end(), sortPin);
	//reportPin();
	for( unsigned i = 0, n = _numPins; i < n - 1; ++i ) {
		Pin* ori = _pinList[i];
		unsigned upY = 1e10;
		unsigned downY = 0;
		// searching for pins after i
		for( unsigned j = i + 1; j < n; ++j ) {
			// create an edge between i and j
			Pin* p = _pinList[j];
			if( isNeighbor(ori, p, upY, downY) ) {
				Edge* e = new Edge(ori, p);
				_OASG.push_back(e);
				if( upY == downY ) {
					assert(upY == ori -> getY());
					break;
				}
			}
		}
	}
	//reportOASG();
}

bool Router::isNeighbor(Pin* ori, Pin* p, unsigned& upY, unsigned& downY)
{
	unsigned oriY = ori -> getY();
	unsigned pY = p -> getY();
	assert(upY > oriY); assert(downY < oriY);
	// create a horizontal edge, then return 
	if( oriY == pY ) {
		upY = downY = oriY;
		return true;
	}
	// check upY
	if( oriY < pY ) {
		if( pY < upY ) {
			upY = pY;
			return true;
		}
		else return false;
	}
	// check downY
	else {
		if( pY > downY ) {
			downY = pY;
			return true;
		}
		else return false;
	}
}

void Router::OAST() {
	std::sort(_OASG.begin(), _OASG.end(), sortEdge);
	reportOASG();
}

void Router::reportPin()
{
	std::cout << "info of PINs: " << std::endl;
	for( unsigned i = 0, n = _numPins; i < n; ++i ) {
		std::cout << "name: " << _pinList[i] -> getName() << ", " << _pinList[i] -> getCoordinate() << std::endl;
	}
}

void Router::reportOASG()
{
	std::cout << "info of OASG: " << std::endl;
	for( unsigned i = 0, n = _OASG.size(); i < n; ++i ) {
		std::pair<Pin*, Pin*> pins = _OASG[i] -> getPins();
		std::cout << "[" << i << "]: " << pins.first -> getName() << ", " << pins.second -> getName() << ", weight: " << _OASG[i] -> getWeight() << std::endl;
	}
}
