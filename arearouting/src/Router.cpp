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
#include <iomanip>

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
struct sortEdgeXcoordinate {
	bool operator () (Edge* a, Edge* b) { 
		assert(a -> getPins().first -> getX() <= a -> getPins().second -> getX());
		assert(b -> getPins().first -> getX() <= b -> getPins().second -> getX());
		return (a -> getPins().first -> getX() < b -> getPins().first -> getX());
	}
};
struct sortEdge {
	bool operator () (Edge* a, Edge* b) { return a -> getWeight() < b -> getWeight(); }
};
// for minHeap comparison
struct EdgeComparator {
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
	writeOASG();
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

	std::priority_queue<Edge*, std::vector<Edge*>, EdgeComparator> minHeap;
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
				Pin* p = minEdge -> getPins().first;
				std::vector<Edge*> OAST = p -> getOAST();
				// add neighboring edge to maxHeap
				for( unsigned j = 0, n = OAST.size(); j < n; ++j ) {
					OAST[j] -> push2MaxHeap(minEdge);
					minEdge -> push2MaxHeap(OAST[j]);
				}
				p -> pushBackOAST(minEdge);			

				p = minEdge -> getPins().second;
				OAST = p -> getOAST();
				for( unsigned j = 0, n = OAST.size(); j < n; ++j ) {
					OAST[j] -> push2MaxHeap(minEdge);
					minEdge -> push2MaxHeap(OAST[j]);
				}
				p -> pushBackOAST(minEdge);			
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
				Pin* p = minEdge -> getPins().first;
				std::vector<Edge*> OAST = p -> getOAST();
				for( unsigned j = 0, n = OAST.size(); j < n; ++j ) {
					OAST[j] -> push2MaxHeap(minEdge);
					minEdge -> push2MaxHeap(OAST[j]);
				}
				p -> pushBackOAST(minEdge);			

				p = minEdge -> getPins().second;
				OAST = p -> getOAST();
				for( unsigned j = 0, n = OAST.size(); j < n; ++j ) {
					OAST[j] -> push2MaxHeap(minEdge);
					minEdge -> push2MaxHeap(OAST[j]);
				}
				p -> pushBackOAST(minEdge);			
			}
			current = next;
		}
	}
	//reportOAST();
	std::cout << "OAST: " << (double)(clock() - start) / CLOCKS_PER_SEC << std::endl;
	writeOAST();
	std::cout << "cost: " << getCost() << std::endl;
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

void Router::OARST()
{
	clock_t start = clock();
/*
	reportPin();
	reportOAST();
	unsigned count = 0;
	for( unsigned i = 0, n = _numPins; i < n; ++i ) {
		std::cout << "pin: : " << _pinList[i] -> getName() << ", OAST: " << _pinList[i] -> getOAST().size() << std::endl;
		if( _pinList[i] -> getOAST().size() > 2 ) ++count;
	}
	std::cout << "count: " << count << std::endl;
*/
	std::sort(_OAST.begin(), _OAST.end(), EdgeComparator());
	//reportPin();
	//reportOAST();
	for( unsigned i = 0, n = _OAST.size(); i < n; ++i ) {
		Edge* e = _OAST[i];
		Edge* neighbor = NULL;
		bool oneSlant = false;
		if( e -> visitedInOARST() ) continue;
		auto maxHeap = e -> getMaxHeap();			// potential problem !!!!
		//std::cout << "size of maxHeap: " << maxHeap.size() << std::endl;
		// get the longest neighboring edge of e
		do {
			neighbor = maxHeap.top();
			//std::cout << "getting top: " << neighbor -> getWeight() << std::endl;
			maxHeap.pop();
			if( !maxHeap.size() && neighbor -> visitedInOARST() ) { oneSlant = true; break; }
		} while( neighbor -> visitedInOARST() );
/*
		if( oneSlant )
			std::cout << "[" << i << "] only one slant edge ..." << std::endl;
		else
			std::cout << "[" << i << "] longest unvisited neightboring edge weight: " << neighbor -> getWeight() << std::endl;
*/
		makeStraight(e, neighbor);
		//e -> setVisitedInOARST();					// these should do according to conditions
		//neighbor -> setVisitedInOARST();
	}
	reportPin();
	reportOAST();
	reportOARST();
	std::cout << "OARST: " << (double)(clock() - start) / CLOCKS_PER_SEC << std::endl;
	writeOARST();
}

void Router::makeStraight(Edge* e, Edge* neighbor)
{
	assert(!e -> visitedInOARST());
	if( neighbor -> visitedInOARST() ) {
		//std::cout << "straighten the only one slant edge e ..." << std::endl;
		std::pair<Pin*, Pin*> pins = e -> getPins();
		Pin* turning = new Pin("only_slant_t", CPoint(pins.first -> getX(), pins.second -> getY()), false);
		Edge* e1 = new Edge(turning, pins.first); Edge* e2 = new Edge(turning, pins.second);
		_OARST.push_back(e1); _OARST.push_back(e2);
		_turningPinsOARST.push_back(turning);
		e -> setVisitedInOARST();
		return;
	}
	std::pair<Pin*, Pin*> endPins;
	Pin* common = getCommon(e, neighbor, endPins);
	//std::cout << "common pin: " << common -> getName() << ", end1: " << endPins.first -> getName() << ", end2: " << endPins.second -> getName() << std::endl;
	// check the relation between endPins
	int dX1 = endPins.first -> getX() - common -> getX(), dX2 = endPins.second -> getX() - common -> getX();
	int dY1 = endPins.first -> getY() - common -> getY(), dY2 = endPins.second -> getY() - common -> getY();
	int dX = dX1 * dX2;
	int dY = dY1 * dY2;
	assert(dX >= -INT_MAX && dX <= INT_MAX); assert(dY >= -INT_MAX && dY <= INT_MAX);

	// Case 1
	if( dX < 0 && dY < 0 ) {
		//std::cout << "Case 1 ..." << std::endl;
		// find the smaller-weighted edge to straighten, may cause some problems , TODO
		//Edge* smaller = (e -> getWeight() < neighbor -> getWeight() ? e : neighbor);
		Edge* smaller = e;
		std::pair<Pin*, Pin*> pins = smaller -> getPins();
		Pin* turning = new Pin(common -> getName() + "_t", CPoint(pins.first -> getX(), pins.second -> getY()), false);
		Edge* e1 = new Edge(turning, pins.first); Edge* e2 = new Edge(turning, pins.second);
		_OARST.push_back(e1); _OARST.push_back(e2);
		_turningPinsOARST.push_back(turning);
		smaller -> setVisitedInOARST();
	}
	// Case 3
	else if( dX > 0 && dY > 0 ) {
		//std::cout << "Case 3 ..." << std::endl;
		int turningX = (std::abs(dX1) < std::abs(dX2) ? endPins.first -> getX() : endPins.second -> getX());
		int turningY = (std::abs(dY1) < std::abs(dY2) ? endPins.first -> getY() : endPins.second -> getY());
		Pin* turning1 = new Pin(common -> getName() + "_t1", CPoint(turningX, turningY), false);
		Pin* turning2 = new Pin(common -> getName() + "_t2", CPoint(common -> getX(), turningY), false);
		Edge* e1 = new Edge(turning1, endPins.first); Edge* e2 = new Edge(turning1, endPins.second);
		Edge* e3 = new Edge(turning2, common);			 Edge* e4 = new Edge(turning2, turning1);
		_OARST.push_back(e1); _OARST.push_back(e2); _OARST.push_back(e3); _OARST.push_back(e4);
		_turningPinsOARST.push_back(turning1); _turningPinsOARST.push_back(turning2);
		e -> setVisitedInOARST();					// these should do according to conditions
		neighbor -> setVisitedInOARST();
	}	
	// Case 4
	else if( dX == 0 || dY == 0 ) {
		//std::cout << "Case 4 ..." << std::endl;
		// find the slant edge to straighten
		std::pair<Pin*, Pin*> pins;
		Edge* rec = (dX1 == 0 || dY1 == 0 ? e : neighbor);
		pins.first = (dX1 == 0 || dY1 == 0 ? endPins.second : endPins.first);
		pins.second = common;
		Pin* turning = new Pin(common -> getName() + "_t", CPoint(pins.first -> getX(), pins.second -> getY()), false);
		Edge* e1 = new Edge(turning, pins.first); Edge* e2 = new Edge(turning, pins.second);
		_OARST.push_back(rec); _OARST.push_back(e1); _OARST.push_back(e2);
		_turningPinsOARST.push_back(turning);
		e -> setVisitedInOARST();
		neighbor -> setVisitedInOARST();
	}
	// Case 2
	else {
		//std::cout << "Case 2 ..." << std::endl;
		if( dX < 0 ) {
			assert(dY > 0);
			Pin* turning1 = new Pin(common -> getName() + "_t1", CPoint(common -> getX(), endPins.first -> getY()), false);
			Pin* turning2 = new Pin(common -> getName() + "_t2", CPoint(common -> getX(), endPins.second -> getY()), false);
			//std::cout << "t1: " << turning1 -> getCoordinate() << std::endl;
			//std::cout << "t2: " << turning2 -> getCoordinate() << std::endl;
			Edge* e1 = new Edge(turning1, common); Edge* e2 = new Edge(turning1, endPins.first);
			Edge* e3 = new Edge(turning2, common); Edge* e4 = new Edge(turning2, endPins.second);
			_OARST.push_back(e1); _OARST.push_back(e2); _OARST.push_back(e3); _OARST.push_back(e4);
			_turningPinsOARST.push_back(turning1); _turningPinsOARST.push_back(turning2);
		}
		else {
			assert(dY < 0);
			Pin* turning1 = new Pin(common -> getName() + "_t1", CPoint(endPins.first -> getX(), common -> getY()), false);
			Pin* turning2 = new Pin(common -> getName() + "_t2", CPoint(endPins.second -> getX(), common -> getY()), false);
			//std::cout << "t1: " << turning1 -> getCoordinate() << std::endl;
			//std::cout << "t2: " << turning2 -> getCoordinate() << std::endl;
			Edge* e1 = new Edge(turning1, common); Edge* e2 = new Edge(turning1, endPins.first);
			Edge* e3 = new Edge(turning2, common); Edge* e4 = new Edge(turning2, endPins.second);
			_OARST.push_back(e1); _OARST.push_back(e2); _OARST.push_back(e3); _OARST.push_back(e4);
			_turningPinsOARST.push_back(turning1); _turningPinsOARST.push_back(turning2);
		}
		e -> setVisitedInOARST();					// these should do according to conditions
		neighbor -> setVisitedInOARST();
	}

	//reportTurningPins();
	//reportOARST();
}

// Note: e/neighbor is always stored in endPins.first/endPins.second
Pin* Router::getCommon(Edge* e, Edge* neighbor, std::pair<Pin*, Pin*>& endPins)
{
	std::pair<Pin*, Pin*> pinsE = e -> getPins(), pinsN = neighbor -> getPins();
	if( pinsE.first == pinsN.first ) {
		endPins.first = pinsE.second; endPins.second = pinsN.second;
		return pinsE.first;
	}
	else if( pinsE.first == pinsN.second ) {
		endPins.first = pinsE.second; endPins.second = pinsN.first;
		return pinsE.first;
	}
	else if( pinsE.second == pinsN.first ) {
		endPins.first = pinsE.first; endPins.second = pinsN.second;
		return pinsE.second;
	}
	else {
		assert(pinsE.second == pinsN.second);
		endPins.first = pinsE.first; endPins.second = pinsN.first;
		return pinsE.second;
	}
}

void Router::OARSMT()
{
	clock_t start = clock();
	std::sort(_OARST.begin(), _OARST.end(), sortEdgeXcoordinate());
	reportOARST();
	for( unsigned i = 0, n = _OARST.size(); i < n; ++i ) {
		std::pair<Pin*, Pin*> pins = _OARST[i] -> getPins();
		for( unsigned j = i + 1, j < n; ++j ) {
			std::pair<Pin*, Pin*> nextPins = _OARST[j] -> getPins();
			if( pins.second -> getX() < nextPins.first -> getX() ) break;
		} 
	}
	
	std::cout << "OARSMT: " << (double)(clock() - start) / CLOCKS_PER_SEC << std::endl;
}

// currently get for OAST
long long Router::getCost()	
{
	long long cost = 0;
	for( unsigned i = 0, n = _OAST.size(); i < n; ++i ) {
		std::pair<Pin*, Pin*> pins = _OAST[i] -> getPins();
		cost += calWeight(pins.first, pins.second);
	}
	return cost;
}

void Router::reportPin()
{
	std::cout << "info of PINs: " << std::endl;
	for( unsigned i = 0, n = _numPins; i < n; ++i ) {
		std::cout << "name: " << _pinList[i] -> getName() << ", " << _pinList[i] -> getCoordinate() << std::endl;
	}
	std::cout << std::endl;
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
	std::cout << "size of OASG: " << _OASG.size() << std::endl << std::endl;
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
	std::cout << "size of OAST: " << _OAST.size() << std::endl << std::endl;
}

void Router::reportOARST()
{
	std::cout << "info of OARST: " << std::endl;
	for( unsigned i = 0, n = _OARST.size(); i < n; ++i ) {
		std::pair<Pin*, Pin*> pins = _OARST[i] -> getPins();
		std::cout << "[" << i << "]: " << pins.first -> getName() << ", " << pins.second -> getName() << ", (" << pins.first -> getX() << ", " << pins.first -> getY() << ") - (" << pins.second -> getX() << ", " << pins.second -> getY() << ")" << std::endl;
	}
	std::cout << std::endl;
	std::cout << "size of OARST: " << _OARST.size() << std::endl << std::endl;
}

void Router::reportTurningPinsOARST()
{
	std::cout << "info of turning PINs: " << std::endl;
	for( unsigned i = 0, n = _turningPinsOARST.size(); i < n; ++i ) {
		std::cout << "name: " << _turningPinsOARST[i] -> getName() << ", " << _turningPinsOARST[i] -> getCoordinate() << std::endl;
	}
	std::cout << std::endl;
	std::cout << "size of turning PINs: " << _turningPinsOARST.size() << std::endl << std::endl;
}

void Router::writeOASG()
{
	std::ofstream file;
	file.open("OASG.plt");
	file << "set title \"OASG.plt\"" << std::endl;
	file << "set size ratio -1" << std::endl;
	file << "set xrange[" << _boundaryBL.x() << ":" << _boundaryTR.x() << "]" << std::endl;
	file << "set yrange[" << _boundaryBL.y() << ":" << _boundaryTR.y() << "]" << std::endl;
	file << "plot \'-\' with linespoints pt 7 ps 1" << std::endl;
	for( unsigned i = 0, n = _OASG.size(); i < n; ++i ) {
		std::pair<Pin*, Pin*> pins = _OASG[i] -> getPins();
		file << pins.first -> getX() << " " << pins.first -> getY() << std::endl << pins.second -> getX() << " " << pins.second -> getY() << std::endl << std::endl;
	}
	file << "EOF" << std::endl;
	file << "pause -1 \'Press any key\'" << std::endl;
}

void Router::writeOAST()
{
	std::ofstream file;
	file.open("OAST.plt");
	file << "set title \"OAST.plt\"" << std::endl;
	file << "set size ratio -1" << std::endl;
	file << "set xrange[" << _boundaryBL.x() << ":" << _boundaryTR.x() << "]" << std::endl;
	file << "set yrange[" << _boundaryBL.y() << ":" << _boundaryTR.y() << "]" << std::endl;
	file << "plot \'-\' with linespoints pt 7 ps 1" << std::endl;
	for( unsigned i = 0, n = _OAST.size(); i < n; ++i ) {
		std::pair<Pin*, Pin*> pins = _OAST[i] -> getPins();
		file << pins.first -> getX() << " " << pins.first -> getY() << std::endl << pins.second -> getX() << " " << pins.second -> getY() << std::endl << std::endl;
	}
	file << "EOF" << std::endl;
	file << "pause -1 \'Press any key\'" << std::endl;
}

void Router::writeOARST()
{
	std::ofstream file;
	file.open("OARST.plt");
	file << "set title \"OARST.plt\"" << std::endl;
	file << "set size ratio -1" << std::endl;
	file << "set xrange[" << _boundaryBL.x() << ":" << _boundaryTR.x() << "]" << std::endl;
	file << "set yrange[" << _boundaryBL.y() << ":" << _boundaryTR.y() << "]" << std::endl;
	file << "plot \'-\' with linespoints pt 7 ps 1" << std::endl;
	for( unsigned i = 0, n = _OARST.size(); i < n; ++i ) {
		std::pair<Pin*, Pin*> pins = _OARST[i] -> getPins();
		file << pins.first -> getX() << " " << pins.first -> getY() << std::endl << pins.second -> getX() << " " << pins.second -> getY() << std::endl << std::endl;
	}
	file << "EOF" << std::endl;
	file << "pause -1 \'Press any key\'" << std::endl;
}
