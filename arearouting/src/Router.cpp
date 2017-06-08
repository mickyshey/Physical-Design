//	File name [ Router.cpp ]
#include <iostream>
#include <fstream>
#include <string>
#include <cassert>
#include <algorithm>
#include <queue>
#include <ctime>

#include "Router.h"
#include "Util.h"

bool sortPin(Pin* a, Pin* b) { return a -> getCoordinate() < b -> getCoordinate(); }
bool sortEdge(Edge* a, Edge* b) { return a -> getWeight() < b -> getWeight(); }
struct comparator {
	bool operator () (Edge* a, Edge* b) { return a -> getWeight() > b -> getWeight(); }
};

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
	clock_t start = clock();
	std::sort(_pinList.begin(), _pinList.end(), sortPin);
	//reportPin();
	for( unsigned i = 0, n = _numPins; i < n - 1; ++i ) {
		Pin* ori = _pinList[i];
		// always start from values that exceed boundary
		int upY = _boundaryTR.y() + 1;
		int downY = _boundaryBL.y() - 1;
		// searching for pins after i
		for( unsigned j = i + 1; j < n; ++j ) {
			// create an edge between i and j
			Pin* p = _pinList[j];
			if( isNeighbor(ori, p, upY, downY) ) {
				Edge* e = new Edge(ori, p, i, j);
				_OASG.push_back(e);
				ori -> pushBackOASG(e);
				p -> pushBackOASG(e);
				if( upY == downY ) {
					assert(upY == (int)ori -> getY());
					break;
				}
			}
		}
	}
	std::cout << "OASG: " << (double)(clock() - start) / CLOCKS_PER_SEC << std::endl;
	//reportOASG();
}

bool Router::isNeighbor(Pin* ori, Pin* p, int& upY, int& downY)
{
	unsigned oriY = ori -> getY();
	unsigned pY = p -> getY();
	assert(upY > (int)oriY); assert(downY < (int)oriY);
	// create a horizontal edge, then return 
	if( oriY == pY ) {
		upY = downY = (int)oriY;
		return true;
	}
	// check upY
	if( oriY < pY ) {
		if( (int)pY < upY ) {
			upY = (int)pY;
			return true;
		}
		else return false;
	}
	// check downY
	else {
		if( (int)pY > downY ) {
			downY = (int)pY;
			return true;
		}
		else return false;
	}
}

void Router::OAST() {
	clock_t start = clock();
	//std::sort(_OASG.begin(), _OASG.end(), sortEdge);
	//reportOASG();
	std::priority_queue<Edge*, std::vector<Edge*>, comparator> minHeap;
	std::vector<bool> visited(_numPins, false);

	unsigned i = 0;
	// always start from idx = 0, better heuristic ?
	unsigned current = 0;
	while( i < _numPins ) {
		if( !visited[current] ) {
			visited[current] = true;
			std::vector<Edge*> adjacency = _pinList[i] -> getOASG();
			for( unsigned i = 0, n = adjacency.size(); i < n; ++i ) {
				if( !adjacency[i] -> added2OAST() ) {
					//std::cout << "current: " << current << ", weight: " << adjacency[i] -> getWeight() << std::endl;
					minHeap.push(adjacency[i]);
					adjacency[i] -> setAdded2OAST();
				}
			}
			Edge* minEdge = minHeap.top(); minHeap.pop();
			std::pair<unsigned, unsigned> pinIndices = minEdge -> getPinIndices();
			unsigned next = (visited[pinIndices.first] ? pinIndices.second : pinIndices.first);
			//std::cout << "current: " << current << ", next: " << next << std::endl;
			if( !visited[next] ) {
				_OAST.push_back(minEdge);
			}
			current = next;
			++i;
		}
		else {
			assert(minHeap.size());
			Edge* minEdge = minHeap.top(); minHeap.pop();
			std::pair<unsigned ,unsigned> pinIndices = minEdge -> getPinIndices();
			unsigned next = (visited[pinIndices.first] ? pinIndices.second : pinIndices.first);
			//std::cout << "current: " << current << ", next: " << next << std::endl;
			if( !visited[next] ) {
				_OAST.push_back(minEdge);
			}
			current = next;
		}
	}
	std::cout << "OAST: " << (double)(clock() - start) / CLOCKS_PER_SEC << std::endl;
	//reportOAST();
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
		std::pair<unsigned ,unsigned> pinIndices = _OASG[i] -> getPinIndices();
		std::cout << "[" << i << "]: " << pins.first -> getName() << "(" << pinIndices.first << "), " << pins.second -> getName() << "(" << pinIndices.second << "), weight: " << _OASG[i] -> getWeight() << std::endl;
	}
	std::cout << std::endl;
	std::cout << "size of OASG: " << _OASG.size() << std::endl;
}

void Router::reportOAST()
{
	std::cout << "info of OAST: " << std::endl;
	for( unsigned i = 0, n = _OAST.size(); i < n; ++i ) {
		std::pair<Pin*, Pin*> pins = _OAST[i] -> getPins();
		std::pair<unsigned ,unsigned> pinIndices = _OAST[i] -> getPinIndices();
		std::cout << "[" << i << "]: " << pins.first -> getName() << "(" << pinIndices.first << "), " << pins.second -> getName() << "(" << pinIndices.second << "), weight: " << _OAST[i] -> getWeight() << std::endl;
	}
	std::cout << std::endl;
	std::cout << "size of OAST: " << _OAST.size() << std::endl;
}
