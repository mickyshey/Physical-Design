//	File name [ Router.cpp ]
#include <iostream>
#include <fstream>
#include <string>
#include <cassert>
#include <algorithm>
#include <queue>
#include <ctime>
#include <set>
#include <iterator>
#include <climits>

#include "Router.h"
#include "Util.h"

struct sortX {
	bool operator () (Pin* a, Pin* b) { return a -> getX() < b -> getX(); }
};
struct sortY {
	bool operator () (Pin* a, Pin* b) { return a -> getY() < b -> getY(); }
};
struct sortY_dec {
	bool operator () (Pin* a, Pin* b) { return a -> getY() > b -> getY(); }
};
struct sortXPlusY {
	bool operator () (Pin* a, Pin* b) { return (a -> getX() + a -> getY()) < (b -> getX() + b -> getY()); }
};
struct sortXMinusY {
	bool operator () (Pin* a, Pin* b) { return (a -> getX() - a -> getY()) < (b -> getX() - b -> getY()); }
};
struct sortEdge {
	bool operator () (Edge* a, Edge* b) { return a -> getWeight() < b -> getWeight(); }
};
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
/*
	std::sort(_pinList.begin(), _pinList.end(), sortX);
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
*/
	std::set<Pin*, sortX> actSet1;			// used for R1, R4
	std::set<Pin*, sortY> actSet2;			// used for R2
	std::set<Pin*, sortY_dec> actSet3;		// used for R3

	// search for R1, R2
	std::sort(_pinList.begin(), _pinList.end(), sortXPlusY());
	//reportPin();
	actSet1.clear();
	actSet2.clear();
	Pin* dummy1 = new Pin("", CPoint(_boundaryBL.x() - 1, _boundaryTR.y() + 1));	// no one's R1 would be dummy1
	Pin* dummy2 = new Pin("", CPoint(_boundaryTR.x() + 1, _boundaryBL.y() - 1));	// no one's R2 would be dummy2
	actSet1.insert(dummy1);
	actSet2.insert(dummy2);
	for( unsigned i = 0, n = _pinList.size(); i < n; ++i ) {
		//std::cout << std::endl;
		//std::cout << "info of actSet1: " << std::endl;
		//for( auto it = actSet1.begin(); it != actSet1.end(); ++it )
		//	std::cout << (*it) -> getCoordinate() << std::endl;
		auto it1 = actSet1.upper_bound(_pinList[i]);		
		//std::cout << "distance between upper_bound and begin: " << std::distance(actSet1.begin(), it1) << std::endl;
		//std::cout << "--it ..." << std::endl;
		--it1;					// get the floor x of pin
		
		//std::cout << "[" << i << "] pin: " << _pinList[i] -> getCoordinate() << std::endl;
		//std::cout << "[" << i << "] it1: " << (*it1) -> getCoordinate() << std::endl;
		int cost = INT_MAX;
		Pin* pin = NULL;
		// y intercept more than x intercept, in R1 !
		while(_pinList[i] -> getY() - (*it1) -> getY() >= _pinList[i] -> getX() - (*it1) -> getX() ) {			
			//std::cout << "distance between it1 and begin: " << std::distance(actSet1.begin(), it1) << std::endl;
			//std::cout << "in while loop, it1: " << (*it1) -> getCoordinate() << std::endl;
			//std::cout << "in while loop, pin: " << _pinList[i] -> getCoordinate() << std::endl;
			assert(it1 != actSet1.begin());			// no one's R1 would be dummy 1
			int currcost = calWeight((*it1), _pinList[i]);
			//std::cout << "tmp_cost: " << cost << ", edge cost: " << currcost << std::endl;
			if(currcost < cost) {
				//std::cout << "create edge for OASG ..." << std::endl;
				pin = (*it1);
				cost = currcost;
			}
			//else std::cout << "dont create edge ..." << std::endl;
			auto tmpIt = it1;
			--it1;		
			actSet1.erase(tmpIt);
		}
		if( cost < INT_MAX ) {
			Edge* e = new Edge(pin, _pinList[i], cost);
			_OASG.push_back(e);
			pin -> pushBackOASG(e);
			_pinList[i] -> pushBackOASG(e);
		}
		actSet1.insert(_pinList[i]);

		auto it2 = actSet2.upper_bound(_pinList[i]);
		--it2;
		cost = INT_MAX;
		// x intercept more than y intercept, in R2 !
		while(_pinList[i] -> getX() - (*it2) -> getX() >= _pinList[i] -> getY() - (*it2) -> getY() ) {			
			assert(it2 != actSet2.begin());			// no one's R2 would be dummy 2
			int currcost = calWeight((*it2), _pinList[i]);
			if(currcost < cost) {
				pin = (*it2);
				cost = currcost;
			}
			auto tmpIt = it2;
			--it2;		
			actSet2.erase(tmpIt);
		}
		if( cost < INT_MAX ) {
			Edge* e = new Edge(pin, _pinList[i], cost);
			_OASG.push_back(e);
			pin -> pushBackOASG(e);
			_pinList[i] -> pushBackOASG(e);
		}
		actSet2.insert(_pinList[i]);
	}
	delete dummy1; delete dummy2;

	// for R3, R4
	std::sort(_pinList.begin(), _pinList.end(), sortXMinusY());
	//reportPin();
	actSet1.clear();
	actSet3.clear();
	dummy1 = new Pin("", CPoint(_boundaryBL.x() - 1, _boundaryBL.y() - 1));			// no one's R4 would be dummy1
	Pin* dummy3 = new Pin("", CPoint(_boundaryTR.x() + 1, _boundaryTR.y() + 1));	// no one's R3 would be dummy3
	actSet1.insert(dummy1);
	actSet3.insert(dummy3);
	// search for R4, R3
	for( unsigned i = 0, n = _pinList.size(); i < n; ++i ) {
		auto it1 = actSet1.upper_bound(_pinList[i]);		
		--it1;					// get the floor x of pin
		
		int cost = INT_MAX;
		Pin* pin = NULL;
		// y intercept more than x intercept, in R4 !
		while((*it1) -> getY() - _pinList[i] -> getY() >= _pinList[i] -> getX() - (*it1) -> getX() ) {			
			assert(it1 != actSet1.begin());			// no one's R1 would be dummy 1
			int currcost = calWeight((*it1), _pinList[i]);
			if(currcost < cost) {
				pin = (*it1);
				cost = currcost;
			}
			auto tmpIt = it1;
			--it1;		
			actSet1.erase(tmpIt);
		}
		if( cost < INT_MAX ) {
			Edge* e = new Edge(pin, _pinList[i], cost);
			_OASG.push_back(e);
			pin -> pushBackOASG(e);
			_pinList[i] -> pushBackOASG(e);
		}
		actSet1.insert(_pinList[i]);

		auto it3 = actSet3.upper_bound(_pinList[i]);
		--it3;
		cost = INT_MAX;
		// x intercept more than y intercept, in R3 !
		while(_pinList[i] -> getX() - (*it3) -> getX() >= (*it3) -> getY() - _pinList[i] -> getY() ) {			
			assert(it3 != actSet3.begin());			// no one's R3 would be dummy 3
			int currcost = calWeight((*it3), _pinList[i]);
			if(currcost < cost) {
				pin = (*it3);
				cost = currcost;
			}
			auto tmpIt = it3;
			--it3;		
			actSet3.erase(tmpIt);
		}
		if( cost < INT_MAX ) {
			Edge* e = new Edge(pin, _pinList[i], cost);
			_OASG.push_back(e);
			pin -> pushBackOASG(e);
			_pinList[i] -> pushBackOASG(e);
		}
		actSet3.insert(_pinList[i]);
	}
	delete dummy1; delete dummy3;
	//reportOASG();
	std::cout << "OASG: " << (double)(clock() - start) / CLOCKS_PER_SEC << std::endl;
}

bool Router::isNeighbor(Pin* ori, Pin* p, int& upY, int& downY)
{
	int oriY = ori -> getY();
	int pY = p -> getY();
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
	setPinIndices4OAST();
	//reportPin();
	//reportOASG();

	std::priority_queue<Edge*, std::vector<Edge*>, comparator> minHeap;
	std::vector<bool> visited(_numPins, false);

	unsigned i = 0;
	// always start from idx = 0, better heuristic ?
	unsigned current = 0;
	while( i < _numPins ) {
		//std::cout << "i: " << i << std::endl;
		if( !visited[current] ) {
			visited[current] = true;
			std::vector<Edge*> adjacency = _pinList[current] -> getOASG();
			for( unsigned j = 0, n = adjacency.size(); j < n; ++j ) {
				if( !adjacency[j] -> added2OAST() ) {
					//std::cout << "current: " << current << ", OASG size: " << n << ", weight: " << adjacency[j] -> getWeight() << std::endl;
					minHeap.push(adjacency[j]);
					adjacency[j] -> setAdded2OAST();
				}
			}
			Edge* minEdge = minHeap.top(); minHeap.pop();
			//std::cout << "min edge weight: " << minEdge -> getWeight() << std::endl;
			std::pair<unsigned, unsigned> pinIndices = minEdge -> getPinIndices();
			unsigned next = (visited[pinIndices.first] ? pinIndices.second : pinIndices.first);
			//std::cout << "current: " << current << ", next: " << next << std::endl;
			if( !visited[next] ) {
				_OAST.push_back(minEdge);
				std::pair<Pin*, Pin*> pins = minEdge -> getPins();
				pins.first -> pushBackOAST(minEdge);
				pins.second -> pushBackOAST(minEdge);
			}
			current = next;
			++i;
		}
		else {
			//std::cout << "current already visited, minHeap size: " << minHeap.size() << std::endl;
			assert(minHeap.size());
			Edge* minEdge = minHeap.top(); minHeap.pop();
			//std::cout << "min edge weight: " << minEdge -> getWeight() << std::endl;
			std::pair<unsigned ,unsigned> pinIndices = minEdge -> getPinIndices();
			unsigned next = (visited[pinIndices.first] ? pinIndices.second : pinIndices.first);
			//std::cout << "current: " << current << ", next: " << next << std::endl;
			if( !visited[next] ) {
				_OAST.push_back(minEdge);
				std::pair<Pin*, Pin*> pins = minEdge -> getPins();
				pins.first -> pushBackOAST(minEdge);
				pins.second -> pushBackOAST(minEdge);
			}
			current = next;
		}
	}
	//reportOAST();
	std::cout << "OAST: " << (double)(clock() - start) / CLOCKS_PER_SEC << std::endl;
}

void Router::setPinIndices4OAST()
{
	for( unsigned i = 0, n = _pinList.size(); i < n; ++i ) {
		Pin* p = _pinList[i];
		std::vector<Edge*> OASG = p -> getOASG();
		for( unsigned j = 0, m = OASG.size(); j < m; ++j ) {
			Edge* e = OASG[j];
			std::pair<Pin*, Pin*> pins = e -> getPins();
			if(pins.first == p) e -> setPinIdxFirst(i);
			else e -> setPinIdxSecond(i);
		}
	}
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
