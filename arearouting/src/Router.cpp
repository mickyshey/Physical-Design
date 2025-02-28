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
#include <unordered_map>
#include <unordered_set>
#include <functional>
#include <utility>

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
struct sortPair {
	bool operator () (std::pair<Pin*, Pin*>& a, std::pair<Pin*, Pin*>& b) {
		int aX = a.first -> getX() < a.second -> getX() ? a.first -> getX() : a.second -> getX();
		int bX = b.first -> getX() < b.second -> getX() ? b.first -> getX() : b.second -> getX();
		return aX < bX;
	}
};
// for minHeap comparison
struct MinHeapComparator {
	bool operator () (Edge* a, Edge* b) { return a -> getWeight() > b -> getWeight(); }
};
struct EdgeComparator {
	bool operator () (Edge* a, Edge* b) { return a -> getWeight() > b -> getWeight(); }
	// sort by X_inc then Y_dec, not weight any more
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
	std::cout << "OASG: " << (double)(clock() - start) / CLOCKS_PER_SEC << std::endl;
	//writeOASG();
}
/*
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
*/
void Router::OAST() {
	clock_t start = clock();
	setPinIndices4OAST();
	//reportPin();
	//reportOASG();

	std::priority_queue<Edge*, std::vector<Edge*>, MinHeapComparator> minHeap;
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
				//std::cout << "min first: " << p -> getName() << std::endl;
				//std::cout << "OAST size: " << OAST.size() << std::endl;
				for( unsigned j = 0, n = OAST.size(); j < n; ++j ) {
					//std::cout << "OAST weight: " << OAST[j] -> getWeight() << std::endl;
					OAST[j] -> push2MaxHeap(minEdge);
					minEdge -> push2MaxHeap(OAST[j]);
					//std::cout << "after push2MaxHeap" << std::endl;
					//if( OAST[j] -> getMaxHeap().size() )
					//	std::cout << "OAST: " << OAST[j] -> getMaxHeap().top() -> getWeight() << std::endl;
					//if( minEdge -> getMaxHeap().size() )
					//	std::cout << "minEdge: " << minEdge -> getMaxHeap().top() -> getWeight() << std::endl;
				}
				p -> pushBackOAST(minEdge);			

				p = minEdge -> getPins().second;
				OAST = p -> getOAST();
				//std::cout << "min second: " << p -> getName() << std::endl;
				//std::cout << "OAST size: " << OAST.size() << std::endl;
				for( unsigned j = 0, n = OAST.size(); j < n; ++j ) {
					//std::cout << "OAST weight: " << OAST[j] -> getWeight() << std::endl;
					OAST[j] -> push2MaxHeap(minEdge);
					minEdge -> push2MaxHeap(OAST[j]);
					//std::cout << "after push2MaxHeap" << std::endl;
					//if( OAST[j] -> getMaxHeap().size() )
					//	std::cout << "OAST: " << OAST[j] -> getMaxHeap().top() -> getWeight() << std::endl;
					//if( minEdge -> getMaxHeap().size() )
					//	std::cout << "minEdge: " << minEdge -> getMaxHeap().top() -> getWeight() << std::endl;
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
	//writeOAST();
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
	assert(!_edgeSet.size());

	std::sort(_OAST.begin(), _OAST.end(), EdgeComparator());

	for( unsigned i = 0, n = _OAST.size(); i < n; ++i ) {
		Edge* e = _OAST[i];
		Edge* neighbor = NULL;
		if( e -> visitedInOARST() ) continue;
		auto maxHeap = e -> getMaxHeap();			// potential problem !!!!
		// get the longest neighboring edge of e
		do {
			neighbor = maxHeap.top();
			maxHeap.pop();
			if( !maxHeap.size() && neighbor -> visitedInOARST() ) break;
		} while( neighbor -> visitedInOARST() );

		makeStraight(e, neighbor, _hashPin);

	}
	//reportPin();
	//reportOAST();
	//reportOARST();
	std::cout << "OARST: " << (double)(clock() - start) / CLOCKS_PER_SEC << std::endl;
	//writeOARST();
}

void Router::makeStraight(Edge* e, Edge* neighbor, std::unordered_map<CPoint, Pin*, CPointHash>& hashPin)
{
	assert(!e -> visitedInOARST());
	if( neighbor -> visitedInOARST() ) {
		//std::cout << "straighten the only one slant edge e ..." << std::endl;
		if( isHorizontal(e) || isVertical(e) ) {
			//_OARST.push_back(e);
			_edgeSet.insert(e);
			e -> setVisitedInOARST();
			return;
		}
		std::pair<Pin*, Pin*> pins = e -> getPins();
		// modification
/*
		int turningX, turningY;
		turningY = std::min(pins.first -> getY(), pins.second -> getY());
		turningX = (pins.first -> getY() == turningY ? pins.second -> getX() : pins.first -> getX());
		Pin* turning = new Pin("only_slant_t", CPoint(turningX, turningY), false);
*/
		// end of modification
		//Pin* turning = insert2HashPin("only_slant_t", CPoint(pins.first -> getX(), pins.second -> getY()), hashPin);
		Pin* turning = insert2HashPin("", CPoint(pins.first -> getX(), pins.second -> getY()), hashPin);
		Edge* e1 = new Edge(turning, pins.first); Edge* e2 = new Edge(turning, pins.second);
		//_OARST.push_back(e1); _OARST.push_back(e2);
		_edgeSet.insert(e1); _edgeSet.insert(e2);
		e -> setVisitedInOARST();
		return;
	}
	std::pair<Pin*, Pin*> endPins;
	Pin* common = getCommon(e, neighbor, endPins);
	//	std::cout << "common pin: " << common -> getName() << ", end1: " << endPins.first -> getName() << ", end2: " << endPins.second -> getName() << std::endl;
	// check the relation between endPins
	int dX1 = endPins.first -> getX() - common -> getX(), dX2 = endPins.second -> getX() - common -> getX();
	int dY1 = endPins.first -> getY() - common -> getY(), dY2 = endPins.second -> getY() - common -> getY();
	//long double dX = (long double)dX1 * (long double)dX2;
	//long double dY = (long double)dY1 * (long double)dY2;
	double dX = (double)dX1/(double)(common -> getX()) * (double)dX2/(double)(common -> getX());
	double dY = (double)dY1/(double)(common -> getY()) * (double)dY2/(double)(common -> getY());
	//	std::cout << "dX: " << dX << ", dY: " << dY << std::endl;
	//}

	// Case 1
	if( dX < 0 && dY < 0 ) {
		//std::cout << "Case 1 ..." << std::endl;
		// find the smaller-weighted edge to straighten, may cause some problems , TODO
		//Edge* smaller = (e -> getWeight() < neighbor -> getWeight() ? e : neighbor);
		std::pair<Pin*, Pin*> pins = e -> getPins();
		//Pin* turning1 = insert2HashPin(common -> getName() + "_t", CPoint(pins.first -> getX(), pins.second -> getY()), hashPin);
		Pin* turning1 = insert2HashPin("", CPoint(pins.first -> getX(), pins.second -> getY()), hashPin);
		Edge* e1 = new Edge(turning1, pins.first); Edge* e2 = new Edge(turning1, pins.second);
		_edgeSet.insert(e1); _edgeSet.insert(e2);
		e -> setVisitedInOARST();
	}
	// Case 3
	else if( dX > 0 && dY > 0 ) {
		//std::cout << "Case 3 ..." << std::endl;
		int turningX = (std::abs(dX1) < std::abs(dX2) ? endPins.first -> getX() : endPins.second -> getX());
		int turningY = (std::abs(dY1) < std::abs(dY2) ? endPins.first -> getY() : endPins.second -> getY());
		// special case
		if( dX1 == dX2 || dY1 == dY2 ) {
			Pin* turning1;
			if( dX1 == dX2 )
				turning1 = (std::abs(dY1) < std::abs(dY2) ? endPins.first : endPins.second);
			else turning1 = (std::abs(dX1) < std::abs(dX2) ? endPins.first : endPins.second);
			//Pin* turning2 = insert2HashPin(common -> getName() + "_t2", CPoint(common -> getX(), turningY), hashPin);
			Pin* turning2 = insert2HashPin("", CPoint(common -> getX(), turningY), hashPin);
			Edge* e1 = new Edge(turning1, endPins.second);
			Edge* e2 = new Edge(turning2, common); Edge* e3 = new Edge(turning2, turning1);
			//_OARST.push_back(e1); _OARST.push_back(e2); _OARST.push_back(e3);
			_edgeSet.insert(e1); _edgeSet.insert(e2); _edgeSet.insert(e3);
		}
		else {
			//Pin* turning1 = insert2HashPin(common -> getName() + "_t1", CPoint(turningX, turningY), hashPin);
			Pin* turning1 = insert2HashPin("", CPoint(turningX, turningY), hashPin);
			//Pin* turning2 = insert2HashPin(common -> getName() + "_t2", CPoint(common -> getX(), turningY), hashPin);
			Pin* turning2 = insert2HashPin("", CPoint(common -> getX(), turningY), hashPin);
			Edge* e1 = new Edge(turning1, endPins.first); Edge* e2 = new Edge(turning1, endPins.second);
			Edge* e3 = new Edge(turning2, common);			 Edge* e4 = new Edge(turning2, turning1);
			_edgeSet.insert(e1); _edgeSet.insert(e2); _edgeSet.insert(e3); _edgeSet.insert(e4);
		}
		e -> setVisitedInOARST();					// these should do according to conditions
		neighbor -> setVisitedInOARST();
	}	
	// Case 5: both edges are in rectilinear form
	else if( dX == 0 && dY == 0 ) {
		_edgeSet.insert(e); _edgeSet.insert(neighbor);
		e -> setVisitedInOARST(); neighbor -> setVisitedInOARST();
	}
	// Case 4
	else if( dX == 0 || dY == 0 ) {
		//std::cout << "Case 4 ..." << std::endl;
		//special case
		if( (dX1 == 0 && dX2 == 0) || (dY1 == 0 && dY2 == 0) ) {
			_edgeSet.insert(e); _edgeSet.insert(neighbor);
		}
		// find the slant edge to straighten
		else {
			std::pair<Pin*, Pin*> pins;
			Edge* rec = (dX1 == 0 || dY1 == 0 ? e : neighbor);
			pins.first = (dX1 == 0 || dY1 == 0 ? endPins.second : endPins.first);
			pins.second = common;
			//Pin* turning = insert2HashPin(common -> getName() + "_t", CPoint(pins.first -> getX(), pins.second -> getY()), hashPin);
			Pin* turning = insert2HashPin("", CPoint(pins.first -> getX(), pins.second -> getY()), hashPin);
			Edge* e1 = new Edge(turning, pins.first); Edge* e2 = new Edge(turning, pins.second);
			_edgeSet.insert(rec); _edgeSet.insert(e1); _edgeSet.insert(e2);
		}
		e -> setVisitedInOARST();
		neighbor -> setVisitedInOARST();
	}
	// Case 2
	else {
		//std::cout << "Case 2 ..." << std::endl;
		if( dX < 0 ) {
			assert(dY > 0);
			//special case
			if( dY1 == dY2 ) {
				//Pin* turning1 = insert2HashPin(common -> getName() + "_t1", CPoint(common -> getX(), endPins.first -> getY()), hashPin);
				Pin* turning1 = insert2HashPin("", CPoint(common -> getX(), endPins.first -> getY()), hashPin);
				Edge* e1 = new Edge(turning1, common);
				Edge* e2 = new Edge(turning1, endPins.first); Edge* e3 = new Edge(turning1, endPins.second);
				_edgeSet.insert(e1); _edgeSet.insert(e2); _edgeSet.insert(e3);
			}
			else {
				//Pin* turning1 = insert2HashPin(common -> getName() + "_t1", CPoint(common -> getX(), endPins.first -> getY()), hashPin);
				Pin* turning1 = insert2HashPin("", CPoint(common -> getX(), endPins.first -> getY()), hashPin);
				//Pin* turning2 = insert2HashPin(common -> getName() + "_t2", CPoint(common -> getX(), endPins.second -> getY()), hashPin);
				Pin* turning2 = insert2HashPin("", CPoint(common -> getX(), endPins.second -> getY()), hashPin);
				//std::cout << "t1: " << turning1 -> getCoordinate() << std::endl;
				//std::cout << "t2: " << turning2 -> getCoordinate() << std::endl;
				Edge* e1;
				if( std::abs(dY1) < std::abs(dY2) ) e1 = new Edge(turning1, common);
				else e1 = new Edge(turning2, common);
				Edge* e2 = new Edge(turning1, turning2); Edge* e3 = new Edge(turning1, endPins.first); Edge* e4 = new Edge(turning2, endPins.second);
				_edgeSet.insert(e1); _edgeSet.insert(e2); _edgeSet.insert(e3); _edgeSet.insert(e4);
			}
		}
		else {
			assert(dY < 0);
			if( dX1 == dX2 ) {
				//Pin* turning1 = insert2HashPin(common -> getName() + "_t1", CPoint(endPins.first -> getX(), common -> getY()), hashPin);
				Pin* turning1 = insert2HashPin("", CPoint(endPins.first -> getX(), common -> getY()), hashPin);
				Edge* e1 = new Edge(turning1, common);
				Edge* e2 = new Edge(turning1, endPins.first); Edge* e3 = new Edge(turning1, endPins.second);
				_edgeSet.insert(e1); _edgeSet.insert(e2); _edgeSet.insert(e3);
			}
			else {
				//Pin* turning1 = insert2HashPin(common -> getName() + "_t1", CPoint(endPins.first -> getX(), common -> getY()), hashPin);
				Pin* turning1 = insert2HashPin("", CPoint(endPins.first -> getX(), common -> getY()), hashPin);
				//Pin* turning2 = insert2HashPin(common -> getName() + "_t2", CPoint(endPins.second -> getX(), common -> getY()), hashPin);
				Pin* turning2 = insert2HashPin("", CPoint(endPins.second -> getX(), common -> getY()), hashPin);
				//std::cout << "t1: " << turning1 -> getCoordinate() << std::endl;
				//std::cout << "t2: " << turning2 -> getCoordinate() << std::endl;
				Edge* e1;
				if( std::abs(dX1) < std::abs(dX2) ) e1 = new Edge(turning1, common);
				else e1 = new Edge(turning2, common);
				Edge* e2 = new Edge(turning1, turning2); Edge* e3 = new Edge(turning1, endPins.first); Edge* e4 = new Edge(turning2, endPins.second);
				_edgeSet.insert(e1); _edgeSet.insert(e2); _edgeSet.insert(e3); _edgeSet.insert(e4);
			}
		}
		e -> setVisitedInOARST();					// these should do according to conditions
		neighbor -> setVisitedInOARST();
	}

	//reportTurningPinsOARST();
	//reportOARST();
}

// Note: e/neighbor is always stored in endPins.first/endPins.second
Pin* Router::getCommon(Edge* e, Edge* neighbor, std::pair<Pin*, Pin*>& endPins)
{
	std::pair<Pin*, Pin*> pinsE = e -> getPins(), pinsN = neighbor -> getPins();
	if( pinsE.first -> operator ==(pinsN.first) ) {
		endPins.first = pinsE.second; endPins.second = pinsN.second;
		return pinsE.first;
	}
	else if( pinsE.first -> operator ==(pinsN.second) ) {
		endPins.first = pinsE.second; endPins.second = pinsN.first;
		return pinsE.first;
	}
	else if( pinsE.second -> operator ==(pinsN.first) ) {
		endPins.first = pinsE.first; endPins.second = pinsN.second;
		return pinsE.second;
	}
	else {
		assert(pinsE.second -> operator ==(pinsN.second));
		endPins.first = pinsE.first; endPins.second = pinsN.first;
		return pinsE.second;
	}
}

void Router::OARSMT()
{
	clock_t start = clock();
	std::unordered_map<std::pair<Pin*, Pin*>, Edge*, pairHashPin> hashEdge;

	removeOverlappingEdges(_edgeSet, hashEdge);
	detectCycleAndFix(_edgeSet);
	LShapedRefinement(_edgeSet);

	std::cout << "OARSMT: " << (double)(clock() - start) / CLOCKS_PER_SEC << std::endl;

// TODO
// should be modified !!
	//reportOARSMT();
	//writeOARSMT();
	writeSolution();
}

void Router::removeOverlappingEdges(std::multiset<Edge*, sortEdgeXcoordinate>& edgeSet, std::unordered_map<std::pair<Pin*, Pin*>, Edge*, pairHashPin>& hashEdge)
{
	auto it = edgeSet.begin();
	while( it != edgeSet.end() ) {
		bool revised = false;
		if( (*it) -> visitedInOARSMT() ) { ++it; continue; }
		//std::cout << "start from: " << std::distance(edgeSet.begin(), it) << std::endl;
/*
		// report edgeSet
		std::cout << "info of edgeSet: " << std::endl;
		for( auto itt = edgeSet.begin(); itt != edgeSet.end(); ++itt ) {
			std::pair<Pin*, Pin*> pins = (*itt) -> getPins();
			std::cout << "[" << std::distance(edgeSet.begin(), itt) << "]: " << pins.first -> getName() << ", " << pins.second -> getName() << ", (" << pins.first -> getX() << ", " << pins.first -> getY() << ") - (" << pins.second -> getX() << ", " << pins.second -> getY() << ")" << std::endl;
		}
		std::cout << std::endl;
		std::cout << "size of edgeSet: " << edgeSet.size() << std::endl << std::endl;
		// end of report
*/
		auto itNext = it;
		++itNext;
		while( itNext != edgeSet.end() ) {
			Edge* e = (*it);	Edge* nextE = (*itNext);
			std::pair<Pin*, Pin*> pins = e -> getPins();
			std::pair<Pin*, Pin*> nextPins = nextE -> getPins();
			// delete the identical edge
			if( pins.first -> operator ==(nextPins.first) && pins.second -> operator ==(nextPins.second) ) {
				//std::cout << "identical edges deleted: it: " << std::distance(edgeSet.begin(), it) << ", itNext: " << std::distance(edgeSet.begin(), itNext) << std::endl;
				auto tmpItNext = itNext;
				--itNext;
				edgeSet.erase(tmpItNext);
				++itNext;
				if( itNext == edgeSet.end() ) { (*it) -> setVisitedInOARSMT(); break; }
				else continue;
			}
			/********************/
			// speed up heuristic
			if( pins.second -> getX() < nextPins.first -> getX() ) {
				(*it) -> setVisitedInOARSMT();
				break;
			}
			// end of speed up heuristic
			/********************/
			//std::cout << "it: " << std::distance(edgeSet.begin(), it) << ", itNext: " << std::distance(edgeSet.begin(), itNext) << std::endl;
			if( isHorizontal(e) && isHorizontal(nextE) ) {
				if( pins.first -> getY() == nextPins.first -> getY() ) {
					//std::cout << "both edges are horizontal and have same y..." << std::endl;
					std::vector<Pin*> vPins(4);
					std::vector<Edge*> tmpEdges;
					vPins[0] = pins.first; vPins[1] = pins.second; vPins[2] = nextPins.first; vPins[3] = nextPins.second;
					sort(vPins.begin(), vPins.end(), sortX());
					if( vPins[1] -> getX() == vPins[2] -> getX() ) {
						// do nothing
					}
					else {
						for( unsigned k = 0; k < 3; ++k ) {
							if( vPins[k] -> getX() == vPins[k + 1] -> getX() ) continue;
							Edge* tmpEdge = insert2HashEdge(vPins[k], vPins[k + 1], hashEdge);
							tmpEdges.push_back(tmpEdge);
						}
						// delete the two overlapping edges and insert the revised edges
						auto tmpIt = it; auto tmpItNext = itNext;
						--it; --itNext;
						edgeSet.erase(tmpItNext); edgeSet.erase(tmpIt);
						for( unsigned k = 0; k < tmpEdges.size(); ++k ) edgeSet.insert(it, tmpEdges[k]);
						// update it and break to the outer while loop
						it = edgeSet.lower_bound(e);
						//std::cout << "lower bound: " << std::distance(edgeSet.begin(), it) << std::endl;
						revised = true;
						break;
					}
				}
			}
			else if( isVertical(e) && isVertical(nextE) ) {
				assert(pins.first -> getX() == nextPins.first -> getX());
				if( overlapWithY(pins, nextPins) ) {
					//std::cout << "pin: " << pins.first -> getName() << ", " << pins.second -> getName() << ", nextPin: " << nextPins.first -> getName() << ", " << nextPins.second -> getName() << std::endl;
					//std::cout << "firstX: " << pins.first -> getX() << ", nextX: " << nextPins.first -> getX() << std::endl;
					//std::cout << "both edges are vertical and have same x..." << std::endl;
					std::vector<Pin*> vPins(4);
					std::vector<Edge*> tmpEdges;
					vPins[0] = pins.first; vPins[1] = pins.second; vPins[2] = nextPins.first; vPins[3] = nextPins.second;
					sort(vPins.begin(), vPins.end(), sortY());
					if( vPins[1] -> getY() == vPins[2] -> getY() ) {}
					else {
						for( unsigned k = 0; k < 3; ++k ) {
							if( vPins[k] -> getY() == vPins[k + 1] -> getY() ) continue;
							Edge* tmpEdge = insert2HashEdge(vPins[k], vPins[k + 1], hashEdge);
							tmpEdges.push_back(tmpEdge);
						}
						// delete the two overlapping edges and insert the revised edges
						auto tmpIt = it; auto tmpItNext = itNext;
						--it; --itNext;
						edgeSet.erase(tmpItNext); edgeSet.erase(tmpIt);
						for( unsigned k = 0; k < tmpEdges.size(); ++k ) edgeSet.insert(it, tmpEdges[k]);
						// update it and break to the outer while loop
						it = edgeSet.lower_bound(e);
						//std::cout << "lower bound: " << std::distance(edgeSet.begin(), it) << std::endl;
						revised = true;
						break;
					}
				}
			}
			else {
				Edge* horizontal;
				Edge* vertical;
				if( isHorizontal(e) ) {
					horizontal = e; vertical = nextE;
				}
				else {
					horizontal = nextE; vertical = e;
				}
				assert(isHorizontal(horizontal)); assert(isVertical(vertical));
				std::pair<Pin*, Pin*> Hpins = horizontal -> getPins();
				std::pair<Pin*, Pin*> Vpins = vertical -> getPins();
				if( Vpins.first -> getX() >= Hpins.first -> getX() && Vpins.first -> getX() <= Hpins.second -> getX() ) {
				int downY = (Vpins.first -> getY() < Vpins.second -> getY() ? Vpins.first -> getY() : Vpins.second -> getY());
				int upY = (Vpins.first -> getY() > Vpins.second -> getY() ? Vpins.first -> getY() : Vpins.second -> getY());
					// overlapping condition
					if( Hpins.first -> getY() >= downY && Hpins.first -> getY() <= upY ) {
						//std::cout << "pin: " << pins.first -> getName() << ", " << pins.second -> getName() << ", nextPin: " << nextPins.first -> getName() << ", " << nextPins.second -> getName() << std::endl;
						//std::cout << "cross-like edges ..." << std::endl;
						std::vector<Pin*> vPins(4);
						vPins[0] = Hpins.first; vPins[1] = Hpins.second; vPins[2] = Vpins.first; vPins[3] = Vpins.second;
						sort(vPins.begin(), vPins.end(), sortX());
						unsigned count = 0;
						for( unsigned k = 0; k < 3; ++k ) {
							if( vPins[k] -> getX() == vPins[k + 1] -> getX() ) ++count;
						}
						assert(count == 1 || count == 2);
						std::vector<Edge*> tmpEdges;
						if( count == 1 ) {
							if( Hpins.first -> getY() == Vpins.first -> getY() ) {
								Edge* e1 = insert2HashEdge(Hpins.first, Vpins.first, hashEdge);
								Edge* e2 = insert2HashEdge(Hpins.second, Vpins.first, hashEdge);
								Edge* e3 = insert2HashEdge(Vpins.first, Vpins.second, hashEdge);
								tmpEdges.push_back(e1); tmpEdges.push_back(e2); tmpEdges.push_back(e3);
							}
							else if( Hpins.first -> getY() == Vpins.second -> getY() ) {
								Edge* e1 = insert2HashEdge(Hpins.first, Vpins.second, hashEdge);
								Edge* e2 = insert2HashEdge(Hpins.second, Vpins.second, hashEdge);
								Edge* e3 = insert2HashEdge(Vpins.first, Vpins.second, hashEdge);
								tmpEdges.push_back(e1); tmpEdges.push_back(e2); tmpEdges.push_back(e3);
							}
							else {
								//Pin* p = insert2HashPin("cross pin", CPoint(Vpins.first -> getX(), Hpins.first -> getY()), _hashPin);
								Pin* p = insert2HashPin("", CPoint(Vpins.first -> getX(), Hpins.first -> getY()), _hashPin);
								Edge* e1 = insert2HashEdge(Hpins.first, p, hashEdge);
								Edge* e2 = insert2HashEdge(Hpins.second, p, hashEdge);
								Edge* e3 = insert2HashEdge(Vpins.first, p, hashEdge);
								Edge* e4 = insert2HashEdge(Vpins.second, p, hashEdge);
								tmpEdges.push_back(e1); tmpEdges.push_back(e2); tmpEdges.push_back(e3); tmpEdges.push_back(e4);
							}
						}
						else {
							if( Hpins.first -> getY() > Vpins.first -> getY() && Hpins.first -> getY() < Vpins.second -> getY() ) {
								assert(Vpins.first -> getX() == Hpins.first -> getX() || Vpins.first -> getX() == Hpins.second -> getX());
								if( Vpins.first -> getX() == Hpins.first -> getX() ) {
									Edge* e1 = insert2HashEdge(Hpins.first, Vpins.first, hashEdge);
									Edge* e2 = insert2HashEdge(Hpins.first, Vpins.second, hashEdge);
									Edge* e3 = insert2HashEdge(Hpins.first, Hpins.second, hashEdge);
									tmpEdges.push_back(e1); tmpEdges.push_back(e2); tmpEdges.push_back(e3);
								}
								else {
									Edge* e1 = insert2HashEdge(Hpins.second, Vpins.first, hashEdge);
									Edge* e2 = insert2HashEdge(Hpins.second, Vpins.second, hashEdge);
									Edge* e3 = insert2HashEdge(Hpins.first, Hpins.second, hashEdge);
									tmpEdges.push_back(e1); tmpEdges.push_back(e2); tmpEdges.push_back(e3);
								}
							}
							else {
								//Edge* e1 = insert2HashEdge(Hpins.first, Hpins.second, hashEdge);
								//Edge* e2 = insert2HashEdge(Vpins.first, Vpins.second, hashEdge);
								//tmpEdges.push_back(e1); tmpEdges.push_back(e2);
								assert(!tmpEdges.size());
							}
						}
						// delete the two overlapping edges and insert the revised edges
						if( tmpEdges.size() ) {
							auto tmpIt = it; auto tmpItNext = itNext;
							--it; --itNext;
							edgeSet.erase(tmpItNext); edgeSet.erase(tmpIt);
							for( unsigned k = 0; k < tmpEdges.size(); ++k ) edgeSet.insert(it, tmpEdges[k]);
							// update it and break to the outer while loop
							it = edgeSet.lower_bound(e);
							//std::cout << "lower bound: " << std::distance(edgeSet.begin(), it) << std::endl;
							revised = true;
							break;
						}
					}
				}
			}
			++itNext;
			if( itNext == edgeSet.end() ) { (*it) -> setVisitedInOARSMT(); break; }
		}
		if( !revised ) ++it;
	}
}

void Router::detectCycleAndFix(std::multiset<Edge*, sortEdgeXcoordinate>& edgeSet)
{
	// set OARSMT for each pins
	for( auto it = edgeSet.begin(); it != edgeSet.end(); ++it ) {
		Edge* e = (*it); std::pair<Pin*, Pin*> pins = e -> getPins();
		pins.first -> pushBackOARSMT(e); pins.second -> pushBackOARSMT(e);
	}
	
	std::vector<std::pair<Pin*, Pin*>> cycles;
	std::queue<Pin*> queueBFS;
	// detect cycle by BFS
	//start BFS from _pinList[0], better heuristic ?
	queueBFS.push(_pinList[0]);
	std::unordered_set<CPoint, CPointHash> currStep, preStep;
	unsigned loop_count = 0;
	unsigned count = 0;
	unsigned currCount = 0;
	unsigned preCount = 1;
	while( !queueBFS.empty() ) {
		Pin* p = queueBFS.front();
		queueBFS.pop();
		if( p -> visitedInBFS() ) continue;
		//std::cout << "BFS in: " << p -> getName() << ", " << p -> getCoordinate() << std::endl;
		p -> setVisitedInBFS();
		assert(p -> getOARSMT().size());
		std::vector<Edge*> OARSMT = p -> getOARSMT();
		// push the neighboring pins to queue and detect cycle
	
		for( unsigned i = 0, n = OARSMT.size(); i < n; ++i ) {
			std::pair<Pin*, Pin*> pins = OARSMT[i] -> getPins();
			assert(pins.first -> operator ==(p) || pins.second -> operator ==(p));
			Pin* neighbor = (pins.first -> operator ==(p) ? pins.second : pins.first);
			//std::cout << "neighbor pin: " << neighbor -> getName() <<", " << neighbor -> getCoordinate() << std::endl;
			if( preStep.find(neighbor -> getCoordinate()) != preStep.end() ) {
				//std::cout << "find a loop with preStep..." << std::endl;
				++loop_count;
				cycles.push_back({p, neighbor});
				//assert(neighbor -> visitedInBFS());
			}
			else if ( currStep.find(neighbor -> getCoordinate()) != currStep.end() ) {
				//std::cout << "find a loop with currStep..." << std::endl;
				++loop_count;
				cycles.push_back({p, neighbor});
			}
			else if( !neighbor -> visitedInBFS() ) {
				//std::cout << "have not visited, add to queue and set from pin ..." << std::endl;
				++currCount;
				neighbor -> setFromPin(p);
				currStep.insert(neighbor -> getCoordinate());
				queueBFS.push(neighbor);
			}
		}
			
		++count;
		//std::cout << "count: " << count << std::endl;
		// update preSteps and preCount
		if( count == preCount ) {
			//std::cout << "go to next step ... currCount: " << currCount << std::endl;
			unsigned size = currStep.size();
			preStep.clear();
			preStep.swap(currStep);
			assert(preStep.size() == size);
			currStep.clear();
			preCount = currCount;
			currCount = 0;
			count = 0;
		}
	}
	
	if( cycles.size() ) {
		std::sort(cycles.begin(), cycles.end(), sortPair());
		//report cycles
	/*
		for( unsigned i = 0, n = cycles.size(); i < n; ++i ) {
			reportCycles(cycles[i]);
		}
		std::cout << "# of loops: " << cycles.size() << std::endl;
	*/
		// fix cycles by filtering out same cycles
		std::vector<std::pair<Pin*, Pin*>> tmpV;
		tmpV.push_back(cycles[0]);
		for( unsigned i = 1, n = cycles.size(); i < n; ++i ) {
			if( cycles[i].first -> operator ==(cycles[i - 1].second) ) continue;
			tmpV.push_back(cycles[i]);
		}
		cycles.swap(tmpV);
		for( unsigned i = 0, n = cycles.size(); i < n; ++i ) {
			//reportCycles(cycles[i]);
			fixCycle(cycles[i], edgeSet);
		}
		//std::cout << "# of loops: " << cycles.size() << std::endl;
	}
}

void Router::fixCycle(const std::pair<Pin*, Pin*>& pins, std::multiset<Edge*, sortEdgeXcoordinate>& edgeSet)
{
	// get Cycle info
	std::unordered_map<CPoint, Pin*, CPointHash> CHash;
	Pin* p1 = pins.first;
	Pin* p2 = pins.second;
	bool at1 = false, at2 = false;
	while(1) {
		//std::cout << "p1: " << p1 -> getName() << ": " << p1 -> getCoordinate() << std::endl;
		//std::cout << "p2: " << p2 -> getName() << ": " << p2 -> getCoordinate() << std::endl;
		if( CHash.find(p1 -> getCoordinate()) == CHash.end() )
			CHash.insert({p1 -> getCoordinate(), p1});
		else { at1 = true; break; }
		if( CHash.find(p2 -> getCoordinate()) == CHash.end() )
			CHash.insert({p2 -> getCoordinate(), p2});
		else { at2 = true; break; }
		p1 = p1 -> getFromPin();
		p2 = p2 -> getFromPin();
	}	
	// delete the useless pin for case 2
	if( p1 -> operator ==(p2) ) {}
	else if( at1 ) {}
	else {
		assert(at2);
		CHash.erase(CHash.find(p1 -> getCoordinate()));
	}
	
	std::vector<Pin*> vPins;
	// get corner info
	for( auto it = CHash.begin(); it != CHash.end(); ++it ) vPins.push_back((*it).second);
	int BLx = INT_MAX, BLy = INT_MAX, TRx = 0, TRy = 0;
	for( unsigned i = 0, n = vPins.size(); i < n; ++i ) {
		Pin* p = vPins[i];
		if( p -> getX() < BLx ) BLx = p -> getX();
		if( p -> getX() > TRx ) TRx = p -> getX();
		if( p -> getY() < BLy ) BLy = p -> getY();
		if( p -> getY() > TRy ) TRy = p -> getY();
	}
	//std::cout << "BL: (" << BLx << "," << BLy << "), TR: (" << TRx << "," << TRy << ")" << std::endl;
	
	// find the pin with in-degree 2 and points to corner, then delete the corresponding edges
	for( unsigned i = 0, n = vPins.size(); i < n; ++i ) {
		Pin* p = vPins[i];
		if( p -> getOARSMT().size() == 2 && !p -> isRealPin() ) {
			if( isCyclePin(p, BLx, BLy, TRx, TRy) ) {
				// delete the corresponding edges in edgeSet
				//std::cout << "cycle pin: " << p -> getName() << ", (" << p -> getX() << "," << p -> getY() << ")" << std::endl;
				Edge* e1 = (p -> getOARSMT())[0]; Edge* e2 = (p -> getOARSMT())[1];
				// we should delete e1 and e2 in OARSMT of other p
				deleteEdgeInOARSMT(p, e1, e2);
				auto it = edgeSet.lower_bound(e1);
				unsigned count = 0;
				while( 1 ) {
					//std::cout << "checking: [" << std::distance(edgeSet.begin(), it) << "]" << std::endl;
					if( (*it) == e1 || (*it) == e2 ) {
						auto tmpIt = it;
						--it;
						edgeSet.erase(tmpIt);
						++count;
					}
					if( count == 2 ) { _hashPin.erase(_hashPin.find(p -> getCoordinate())); delete p; return; }
					++it;
				}
			}
		}
	}
	//std::cout << "no good case to simplify, perform simple edge removement ..." << std::endl;
	for( unsigned i = 0, n = vPins.size(); i < n; ++i ) {
		Pin* p = vPins[i];
		if( p -> isRealPin() ) {
			std::vector<Edge*> OARSMT = p -> getOARSMT();
			// find the most-weighted edge which is within cycle
			unsigned bestWeight = 0, currWeight = 0;
			Edge* mostWeighted = NULL;
			for( unsigned j = 0; j < OARSMT.size(); ++j ) {
				if( isWithinCycle(p, OARSMT[j], BLx, BLy, TRx, TRy) ) {
					currWeight = calWeight(OARSMT[j]);
					if( currWeight > bestWeight ) {
						bestWeight = currWeight;
						mostWeighted = OARSMT[j];
					}
				}
			}
			assert(mostWeighted);
			// no need to maintain OARSMT of p, cause the following process, e.g. U-shaped refinement only considers turning pins !!!
			// turns out that you need maintain OARSMT of p
			deleteEdgeInOARSMT(mostWeighted);
			auto it = edgeSet.lower_bound(mostWeighted);
			while( 1 ) {
				//std::cout << "checking: [" << std::distance(edgeSet.begin(), it) << "]" << std::endl;
				if( (*it) == mostWeighted ) {
					auto tmpIt = it;
					--it;
					edgeSet.erase(tmpIt);
					return;
				}
				++it;
			}
		}
	}
}

void Router::deleteEdgeInOARSMT(Pin* p, Edge* e1, Edge* e2)
{
	// delete e1 first
	Pin* other = getAnotherPin(p, e1);
/*
	std::vector<Edge*> OARSMT = other -> getOARSMT();
	for( auto it = OARSMT.begin(); it != OARSMT.end(); ++it ) {
		if( (*it) == e1 ) { OARSMT.erase(it); break; }
	}
*/
	other -> deleteInOARSMT(e1);
	//std::cout << "other pin for e1: " << other -> getName() << ", " << other << std::endl;
	//std::vector<Edge*> OARSMT = other -> getOARSMT();
	//for( unsigned i = 0; i < OARSMT.size(); ++i )
	//	std::cout << OARSMT[i] -> getPins().first -> getName() << " - " << OARSMT[i] -> getPins().second -> getName() << std::endl;
	
	// delete e2
	other = getAnotherPin(p ,e2);
/*
	OARSMT = other -> getOARSMT();
	for( auto it = OARSMT.begin(); it != OARSMT.end(); ++it ) {
		if( (*it) == e2 ) { OARSMT.erase(it); break; }
	}
*/
	other -> deleteInOARSMT(e2);
	//std::cout << "other pin for e2: " << other -> getName() << ", " << other << std::endl;
	//OARSMT = other -> getOARSMT();
	//for( unsigned i = 0; i < OARSMT.size(); ++i )
	//	std::cout << OARSMT[i] -> getPins().first -> getName() << " - " << OARSMT[i] -> getPins().second -> getName() << std::endl;
}

void Router::deleteEdgeInOARSMT(Edge* e)
{
	std::pair<Pin*, Pin*> pins = e -> getPins();
	//std::cout << "deleteing edge: " << e -> getPins().first -> getCoordinate() << " - " << e -> getPins().second -> getCoordinate() << ", " << e << std::endl;
	// delete pins.first
	//std::vector<Edge*> OARSMT = pins.first -> getOARSMT();
	// report pins.first
/*
	std::cout << "pins.first: " << pins.first -> getName() << ", " << pins.first << std::endl;
	std::cout << "OARSMT of pins.first: " << OARSMT.size() << std::endl;
	for( unsigned i = 0; i < OARSMT.size(); ++i )
		std::cout << OARSMT[i] -> getPins().first -> getCoordinate() << " - " << OARSMT[i] -> getPins().second -> getCoordinate() << ", " << OARSMT[i] << std::endl;
*/
/*
	for( auto it = OARSMT.begin(); it != OARSMT.end(); ++it ) {
		if( (*it) == e ) { 
			std::cout << "found at: " << std::distance(OARSMT.begin(), it) << std::endl;
			OARSMT.erase(it); break; }
	}
*/
	pins.first -> deleteInOARSMT(e);
	// delete pins.second
	//OARSMT = pins.second -> getOARSMT();
/*
	std::cout << "pins.second: " << pins.second -> getName() << ", " << pins.second << std::endl;
	std::cout << "OARSMT of pins.second: " << OARSMT.size() << std::endl;
	for( unsigned i = 0; i < OARSMT.size(); ++i )
		std::cout << OARSMT[i] -> getPins().first -> getCoordinate() << " - " << OARSMT[i] -> getPins().second -> getCoordinate() << ", " << OARSMT[i] << std::endl;
*/
/*
	for( auto it = OARSMT.begin(); it != OARSMT.end(); ++it ) {
		if( (*it) == e ) {
			std::cout << "found at: " << std::distance(OARSMT.begin(), it) << std::endl;
			OARSMT.erase(it); break; }
	}
*/
	pins.second -> deleteInOARSMT(e);
}

bool Router::isWithinCycle(Pin* p, Edge* e, int BLx, int BLy, int TRx, int TRy)
{
	std::pair<Pin*, Pin*> pins = e -> getPins();
	Pin* neighbor = pins.first -> operator ==(p) ? pins.second : pins.first;
	return (neighbor -> getX() >= BLx && neighbor -> getX() <= TRx && neighbor -> getY() >= BLy && neighbor -> getY() <= TRy);
}

bool Router::isCyclePin(Pin* p, int BLx, int BLy, int TRx, int TRy)
{
	std::vector<Edge*> OARSMT = p -> getOARSMT();
	assert(OARSMT.size() == 2);
	for( unsigned i = 0; i < OARSMT.size(); ++i ) {
		std::pair<Pin*, Pin*> pins = OARSMT[i] -> getPins();
		Pin* neighbor = pins.first -> operator ==(p) ? pins.second : pins.first;
		if( neighbor -> getX() == BLx || neighbor -> getX() == TRx ) {
			if( neighbor -> getY() == BLy || neighbor -> getY() == TRy ) {}
			else return false;
		}
		else return false;
	}
	return true;
}

void Router::LShapedRefinement(std::multiset<Edge*, sortEdgeXcoordinate>& edgeSet)
{
	//std::cout << "size of turningPins: " << _hashPin.size() << std::endl;
	std::vector<Pin*> LPins;
	for( auto it = _hashPin.begin(); it != _hashPin.end(); ++it ) LPins.push_back((*it).second);

	for( unsigned i = 0; ; ++i ) {
		if( i == LPins.size() ) break;//return;
		assert(!(LPins[i] -> isRealPin()));
		Pin* p = LPins[i];
	//for( auto it = _hashPin.begin(); it != _hashPin.end(); ++it ) {
	//	assert(!((*it).second -> isRealPin()));
	//	Pin* p = (*it).second;
		/***************/
		// modified code
		//if( p -> visitedInLCheck() ) continue;
		// end of modification
		/***************/
		if( p -> getOARSMT().size() != 2 ) continue;
		if( isHorizontal(p -> getOARSMT()[0]) == isHorizontal(p -> getOARSMT()[1]) ) continue;
		bool findAtFirst = false;
		std::pair<Pin*, Pin*> pins; std::pair<bool, bool> isH; std::pair<bool, bool> dir;
		//dir: 1 for Right(H)/Up(V), 0 for Left(H)/Down(V)
		getOtherPinsInfo(p, pins, isH, dir);
		/***************/
		// modified code
		//if( pins.first -> visitedInLCheck() || pins.second -> visitedInLCheck() ) continue;
		// end of modification
		/***************/
		assert(isH.first != isH.second);
		// detect for pins.first	
		std::vector<Edge*> OARSMT = pins.first -> getOARSMT();
		assert(OARSMT.size());
		if( OARSMT.size() > 1 ) {
			for( unsigned k = 0; k < OARSMT.size(); ++k ) {
				if( isH.first ? isVertical(OARSMT[k]) : isHorizontal(OARSMT[k]) ) {
					Pin* tmpP = getAnotherPin(pins.first, OARSMT[k]);
					/***************/
					// modified
					//if( tmpP -> visitedInLCheck() ) continue;
					// end of modification
					/***************/
					// satisfying condition: dir is same as dir.second
					if( getDirection(pins.first, tmpP, isH.first) == dir.second ) {
						findAtFirst = true;
						/***************/
						// modified code
						//p -> setVisitedInLCheck(); pins.first -> setVisitedInLCheck();
						//pins.second -> setVisitedInLCheck(); tmpP -> setVisitedInLCheck();
						// end of modification
						/***************/
						//std::cout << "L-Shaped pin: " << p -> getName() << "(" << p -> getCoordinate() << ")... find at first: " << pins.first -> getName() << "(" << pins.first -> getCoordinate() << "), another pin: " << pins.second -> getName() << "(" << pins.second -> getCoordinate() << "), tmpP: " << tmpP -> getName() << "(" << tmpP -> getCoordinate() << ")" << std::endl;
						// two cases for the refinement
						if( isH.first ) {
							assert(pins.first -> getX() == tmpP -> getX());
							// Case 1
							if( std::abs(tmpP -> getY() - p -> getY()) > std::abs(pins.second -> getY() - p -> getY()) ) {
								//Pin* LPin = insert2HashPin(p -> getName() + "_L", CPoint(pins.first -> getX(), pins.second -> getY()), _hashPin);
								Pin* LPin = insert2HashPin("", CPoint(pins.first -> getX(), pins.second -> getY()), _hashPin);
								LPins.push_back(LPin);
								// delete edge first
								deleteEdgeInOARSMT(p, p -> getOARSMT()[0], p -> getOARSMT()[1]);
								deleteEdgeInOARSMT(OARSMT[k]);
								deleteInEdgeSet(p -> getOARSMT()[0]); deleteInEdgeSet(p -> getOARSMT()[1]); deleteInEdgeSet(OARSMT[k]);
								/*****************/
								// should we maintain the OARSMT in each pin and the _hashPin ?
								// maintain OARSMT in each pin is necessary !!!
								/*****************/
								// the newly created edge should not exist in current edgeSet
								Edge* LEdge1 = new Edge(pins.first, LPin); Edge* LEdge2 = new Edge(pins.second, LPin); Edge* LEdge3 = new Edge(tmpP, LPin);
								addToOARSMT(LEdge1); addToOARSMT(LEdge2); addToOARSMT(LEdge3);
								edgeSet.insert(LEdge1); edgeSet.insert(LEdge2); edgeSet.insert(LEdge3);
								// TODO
							}
							// Case 2
							else {
								//if( pins.second -> getY() == tmpP -> getY() ) std::cout << "minor bug to be fixed: same height between pins.second and tmpP" << std::endl;
								//Pin* LPin = insert2HashPin(p -> getName() + "_L", CPoint(pins.second -> getX(), tmpP -> getY()), _hashPin);
								Pin* LPin = insert2HashPin("", CPoint(pins.second -> getX(), tmpP -> getY()), _hashPin);
								LPins.push_back(LPin);
								deleteEdgeInOARSMT(p, p -> getOARSMT()[0], p -> getOARSMT()[1]);
								deleteInEdgeSet(p -> getOARSMT()[0]); deleteInEdgeSet(p -> getOARSMT()[1]);
								Edge* LEdge1 = new Edge(LPin, pins.second); Edge* LEdge2 = new Edge(LPin, tmpP);
								addToOARSMT(LEdge1); addToOARSMT(LEdge2);
								edgeSet.insert(LEdge1); edgeSet.insert(LEdge2);
							}
						}
						else {
							// Case 1
							if( std::abs(tmpP -> getX() - p -> getX()) > std::abs(pins.second -> getX() - p -> getX()) ) {
								//Pin* LPin = insert2HashPin(p -> getName() + "_L", CPoint(pins.second -> getX(), pins.first -> getY()), _hashPin);
								Pin* LPin = insert2HashPin("", CPoint(pins.second -> getX(), pins.first -> getY()), _hashPin);
								LPins.push_back(LPin);
								deleteEdgeInOARSMT(p, p -> getOARSMT()[0], p -> getOARSMT()[1]);
								deleteEdgeInOARSMT(OARSMT[k]);
								deleteInEdgeSet(p -> getOARSMT()[0]); deleteInEdgeSet(p -> getOARSMT()[1]); deleteInEdgeSet(OARSMT[k]);
								Edge* LEdge1 = new Edge(pins.first, LPin); Edge* LEdge2 = new Edge(pins.second, LPin); Edge* LEdge3 = new Edge(tmpP, LPin);
								addToOARSMT(LEdge1); addToOARSMT(LEdge2); addToOARSMT(LEdge3);
								edgeSet.insert(LEdge1); edgeSet.insert(LEdge2); edgeSet.insert(LEdge3);
							}
							// Case 2
							else {
								//if( pins.second -> getX() == tmpP -> getX() ) std::cout << "minor bug to be fixed: same width between pins.second and tmpP" << std::endl;
								//Pin* LPin = insert2HashPin(p -> getName() + "_L", CPoint(tmpP -> getX(), pins.second -> getY()), _hashPin);
								Pin* LPin = insert2HashPin("", CPoint(tmpP -> getX(), pins.second -> getY()), _hashPin);
								LPins.push_back(LPin);
								deleteEdgeInOARSMT(p, p -> getOARSMT()[0], p -> getOARSMT()[1]);
								deleteInEdgeSet(p -> getOARSMT()[0]); deleteInEdgeSet(p -> getOARSMT()[1]);
								Edge* LEdge1 = new Edge(LPin, pins.second); Edge* LEdge2 = new Edge(LPin, tmpP);
								addToOARSMT(LEdge1); addToOARSMT(LEdge2); 
								edgeSet.insert(LEdge1); edgeSet.insert(LEdge2);
							}
						}
					}
				}
			}
		}
		if( findAtFirst ) continue;
		// check pins.second
		OARSMT = pins.second -> getOARSMT();
		//assert(OARSMT.size());
		if( OARSMT.size() > 1 ) {
			for( unsigned k = 0; k < OARSMT.size(); ++k ) {
				if( isH.second ? isVertical(OARSMT[k]) : isHorizontal(OARSMT[k]) ) {
					Pin* tmpP = getAnotherPin(pins.second, OARSMT[k]);
					/***************/
					// modified
					//if( tmpP -> visitedInLCheck() ) continue;
					// end of modification
					/***************/
					// satisfying condition: dir is same as dir.second
					if( getDirection(pins.second, tmpP, isH.second) == dir.first ) {
						assert(!findAtFirst);
						/***************/
						// modified code
						//p -> setVisitedInLCheck(); pins.first -> setVisitedInLCheck();
						//pins.second -> setVisitedInLCheck(); tmpP -> setVisitedInLCheck();
						// end of modification
						/***************/
						//std::cout << "L-Shaped pin: " << p -> getName() << "(" << p -> getCoordinate() << ")... find at second: " << pins.second -> getName() << "(" << pins.second -> getCoordinate() << "), another pin: " << pins.first -> getName() << "(" << pins.first -> getCoordinate() << "), tmpP: " << tmpP -> getName() << "(" << tmpP -> getCoordinate() << ")" << std::endl;
						// two cases for the refinement
						if( isH.second ) {
							assert(pins.second -> getX() == tmpP -> getX());
							// Case 1
							if( std::abs(tmpP -> getY() - p -> getY()) > std::abs(pins.first -> getY() - p -> getY()) ) {
								//Pin* LPin = insert2HashPin(p -> getName() + "_L", CPoint(pins.second -> getX(), pins.first -> getY()), _hashPin);
								Pin* LPin = insert2HashPin("", CPoint(pins.second -> getX(), pins.first -> getY()), _hashPin);
								LPins.push_back(LPin);
								// delete edge first
								deleteEdgeInOARSMT(p, p -> getOARSMT()[0], p -> getOARSMT()[1]);
								deleteEdgeInOARSMT(OARSMT[k]);
								deleteInEdgeSet(p -> getOARSMT()[0]); deleteInEdgeSet(p -> getOARSMT()[1]); deleteInEdgeSet(OARSMT[k]);
								// the newly created edge should not exist in current edgeSet
								Edge* LEdge1 = new Edge(pins.second, LPin); Edge* LEdge2 = new Edge(pins.first, LPin); Edge* LEdge3 = new Edge(tmpP, LPin);
								addToOARSMT(LEdge1); addToOARSMT(LEdge2); addToOARSMT(LEdge3);
								edgeSet.insert(LEdge1); edgeSet.insert(LEdge2); edgeSet.insert(LEdge3);
								// TODO
								// should we maintain the OARSMT in each pin and the _hashPin ?
								// maintain OARSMT in each pin is necessay !!!
								// deletion done in deleteInEdgeSet, insertion done in addOARSMT
							}
							// Case 2
							else {
								//if( pins.first -> getY() == tmpP -> getY() ) std::cout << "minor bug to be fixed: same height between pins.first and tmpP" << std::endl;
								//Pin* LPin = insert2HashPin(p -> getName() + "_L", CPoint(pins.first -> getX(), tmpP -> getY()), _hashPin);
								Pin* LPin = insert2HashPin("", CPoint(pins.first -> getX(), tmpP -> getY()), _hashPin);
								LPins.push_back(LPin);
								deleteEdgeInOARSMT(p, p -> getOARSMT()[0], p -> getOARSMT()[1]);
								deleteInEdgeSet(p -> getOARSMT()[0]); deleteInEdgeSet(p -> getOARSMT()[1]);
								Edge* LEdge1 = new Edge(LPin, pins.first); Edge* LEdge2 = new Edge(LPin, tmpP);
								addToOARSMT(LEdge1); addToOARSMT(LEdge2);
								edgeSet.insert(LEdge1); edgeSet.insert(LEdge2);
							}
						}
						else {
							// Case 1
							if( std::abs(tmpP -> getX() - p -> getX()) > std::abs(pins.first -> getX() - p -> getX()) ) {
								//Pin* LPin = insert2HashPin(p -> getName() + "_L", CPoint(pins.first -> getX(), pins.second -> getY()), _hashPin);
								Pin* LPin = insert2HashPin("", CPoint(pins.first -> getX(), pins.second -> getY()), _hashPin);
								LPins.push_back(LPin);
								deleteEdgeInOARSMT(p, p -> getOARSMT()[0], p -> getOARSMT()[1]);
								deleteEdgeInOARSMT(OARSMT[k]);
								deleteInEdgeSet(p -> getOARSMT()[0]); deleteInEdgeSet(p -> getOARSMT()[1]); deleteInEdgeSet(OARSMT[k]);
								Edge* LEdge1 = new Edge(pins.second, LPin); Edge* LEdge2 = new Edge(pins.first, LPin); Edge* LEdge3 = new Edge(tmpP, LPin);
								addToOARSMT(LEdge1); addToOARSMT(LEdge2); addToOARSMT(LEdge3);
								edgeSet.insert(LEdge1); edgeSet.insert(LEdge2); edgeSet.insert(LEdge3);
							}
							// Case 2
							else {
								//if( pins.first -> getX() == tmpP -> getX() ) std::cout << "minor bug to be fixed: same width between pins.first and tmpP" << std::endl;
								//Pin* LPin = insert2HashPin(p -> getName() + "_L", CPoint(tmpP -> getX(), pins.first -> getY()), _hashPin);
								Pin* LPin = insert2HashPin("", CPoint(tmpP -> getX(), pins.first -> getY()), _hashPin);
								LPins.push_back(LPin);
								deleteEdgeInOARSMT(p, p -> getOARSMT()[0], p -> getOARSMT()[1]);
								deleteInEdgeSet(p -> getOARSMT()[0]); deleteInEdgeSet(p -> getOARSMT()[1]);
								Edge* LEdge1 = new Edge(LPin, pins.first); Edge* LEdge2 = new Edge(LPin, tmpP);
								addToOARSMT(LEdge1); addToOARSMT(LEdge2);
								edgeSet.insert(LEdge1); edgeSet.insert(LEdge2);
							}
						}
					}
				}
			}
		}
	}
	// check if there are identical edges or edges with same pin
/*
	unsigned count = 0;
	for( auto it = edgeSet.begin(); it != edgeSet.end(); ++it ) {
		Edge* e = (*it);
		if( e -> getPins().first -> operator ==(e -> getPins().second) ) {
			std::cout << "edge with same pin ..." << std::endl;
			++count;
		}
		auto itNext = it;
		++itNext;
		if( itNext == edgeSet.end() ) break;
		Edge* eNext = (*itNext);
		if( e -> getPins().first -> operator == (eNext -> getPins().first) ) {
			if( e -> getPins().second -> operator == (eNext -> getPins().second) ) {
				std::cout << "identical edge ..." << std::endl;
				++count;
			}
		}
	}
	std::cout << "total potential bugs: " << count << std::endl;
*/
}

void Router::addToOARSMT(Edge* e)
{
	std::pair<Pin*, Pin*> pins = e -> getPins();
	// add pins.first
	pins.first -> pushBackOARSMT(e);
	pins.second -> pushBackOARSMT(e);
}

void Router::deleteInEdgeSet(Edge* e)
{
	// maintain OARSMT in both pins of e
	// do not do here
	//deleteEdgeInOARSMT(e);

	// report edgeSet
	//std::cout << "deleting: " << e -> getPins().first -> getName() << " - " << e -> getPins().second -> getName() << std::endl;
	auto it = _edgeSet.lower_bound(e);
	while( 1 ) {
		//std::cout << "checking " << std::distance(_edgeSet.begin(), it) << " in _edgeSet" << std::endl;
		if( (*it) == e ) { _edgeSet.erase(it); return; }
		++it;
	}
}

bool Router::getDirection(Pin* p, Pin* anotherPin, bool isH)
{
	if( isH ) return anotherPin -> getY() > p -> getY();
	else return anotherPin -> getX() > p -> getX();
}

Pin* Router::getAnotherPin(Pin* p, Edge* e)
{
	std::pair<Pin*, Pin*> pins = e -> getPins();
	if( pins.first -> operator ==(p) ) return pins.second;
	else return pins.first;
}

void Router::getOtherPinsInfo(Pin* p, std::pair<Pin*, Pin*>& pins, std::pair<bool, bool>& isH, std::pair<bool, bool>& dir)
{
	assert(p -> getOARSMT().size() == 2);
	Edge* e1 = p -> getOARSMT()[0]; Edge* e2 = p -> getOARSMT()[1];
	//std::pair<Pin*, Pin*> tmpPins = e1 -> getPins();
	//pins.first = tmpPins.first -> operator ==(p) ? tmpPins.second : tmpPins.first;
	//std::cout << "p: " << p -> getCoordinate() << std::endl;
	pins.first = getAnotherPin(p, e1);
	isH.first = pins.first -> getX() == p -> getX() ? false : true;
	if( isH.first ) dir.first = pins.first -> getX() > p -> getX() ? true : false;
	else dir.first = pins.first -> getY() > p -> getY() ? true : false;
	//tmpPins = e2 -> getPins();
	//pins.second = tmpPins.first -> operator ==(p) ? tmpPins.second : tmpPins.first;
	pins.second = getAnotherPin(p, e2);
	isH.second = pins.second -> getX() == p -> getX() ? false: true;
	if( isH.second ) dir.second = pins.second -> getX() > p -> getX() ? true : false;
	else dir.second = pins.second -> getY() > p -> getY() ? true : false;
	//std::cout << "first: " << pins.first -> getCoordinate() << ", isH: " << isH.first << ", dir: " << dir.first << std::endl;
	//std::cout << "second: " << pins.second -> getCoordinate() << ", isH: " << isH.second << ", dir: " << dir.second << std::endl;
}

Edge* Router::insert2HashEdge(Pin* a, Pin* b, std::unordered_map<std::pair<Pin*, Pin*>, Edge*, pairHashPin>& hashEdge)
{
	std::pair<Pin*, Pin*> pins;
	comparePins(a, b, pins);
	if( hashEdge.find({pins.first, pins.second}) == hashEdge.end() ) {
		//std::cout << "adding edge: " << pins.first -> getName() << ", " << pins.second -> getName() << std::endl;
		Edge* e = new Edge(pins.first, pins.second);
		hashEdge.insert({{pins.first, pins.second}, e});
		return e;
	}
	else {
		//std::cout << "edge already exist ..." << std::endl;
		return hashEdge.at({pins.first, pins.second});
	}
}

Pin* Router::insert2HashPin(const std::string& name, const CPoint& c, std::unordered_map<CPoint, Pin*, CPointHash>& hashPin)
{
	if( hashPin.find(c) == hashPin.end() ) {
		Pin* p = new Pin(name, c, false);
		hashPin.insert({c, p});
		return p;
	}
	else return hashPin.at(c);
}

void Router::comparePins(Pin* a, Pin* b, std::pair<Pin*, Pin*>& pins)
{
	assert(a -> getX() == b -> getX() || a -> getY() == b -> getY());
	if( a -> getX() < b -> getX() ) pins = {a, b};
	else if( a -> getY() < b -> getY() ) pins = {a, b};
	else pins = {b, a};
}

bool Router::overlapWithY(std::pair<Pin*, Pin*>& pins, std::pair<Pin*, Pin*>& nextPins) {
	// use pins to decide
	int firstY = pins.first -> getY(), secondY = pins.second -> getY();
	int upY = std::max(nextPins.first -> getY(), nextPins.second -> getY());
	int downY = std::min(nextPins.first -> getY(), nextPins.second -> getY());
	if( firstY >= downY && firstY <= upY ) return true;
	else if( secondY >= downY && secondY <= upY ) return true;
	else {
		if( firstY > secondY ) { firstY = pins.second -> getY(); secondY = pins.first -> getY(); }
		assert(firstY < secondY);
		assert(firstY != downY); assert(firstY != upY);
		assert(secondY != downY); assert(secondY != upY);
		return(firstY < downY && secondY > upY);
	}
}

bool Router::isHorizontal(Edge* e)
{
	std::pair<Pin*, Pin*> pins = e -> getPins();
	return (pins.first -> getY() == pins.second -> getY());
}

bool Router::isVertical(Edge* e)
{
	std::pair<Pin*, Pin*> pins = e -> getPins();
	return (pins.first -> getX() == pins.second -> getX());
}

long long Router::getCostOAST()	
{
	long long cost = 0;
	for( unsigned i = 0, n = _OAST.size(); i < n; ++i ) {
		std::pair<Pin*, Pin*> pins = _OAST[i] -> getPins();
		cost += calWeight(pins.first, pins.second);
	}
	return cost;
}

long long Router::getCostOARST()	
{
/*
	long long cost = 0;
	for( unsigned i = 0, n = _OARST.size(); i < n; ++i ) {
		std::pair<Pin*, Pin*> pins = _OARST[i] -> getPins();
		cost += calWeight(pins.first, pins.second);
	}
	return cost;
*/
}

long long Router::getCostOARSMT()	
{
	long long cost = 0;
/*
	for( unsigned i = 0, n = _OARSMT.size(); i < n; ++i ) {
		std::pair<Pin*, Pin*> pins = _OARSMT[i] -> getPins();
		cost += calWeight(pins.first, pins.second);
	}
*/
	for( auto it = _edgeSet.begin(); it != _edgeSet.end(); ++it ) {
		std::pair<Pin*, Pin*> pins = (*it) -> getPins();
		cost += calWeight(pins.first, pins.second);
	}
	return cost;
}

void Router::reportCycles(const std::pair<Pin*, Pin*>& pins)
{
	std::unordered_set<CPoint, CPointHash> CHash;
	Pin* p1 = pins.first;
	Pin* p2 = pins.second;
	bool at1 = false, at2 = false;
	while(1) {
		std::cout << "p1: " << p1 -> getName() << ": " << p1 -> getCoordinate() << ", in degree: " << p1 -> getOARSMT().size() << std::endl;
		std::cout << "p2: " << p2 -> getName() << ": " << p2 -> getCoordinate() << ", in degree: " << p2 -> getOARSMT().size() << std::endl;
		if( CHash.find(p1 -> getCoordinate()) == CHash.end() )
			CHash.insert(p1 -> getCoordinate());
		else { at1 = true; break; }
		if( CHash.find(p2 -> getCoordinate()) == CHash.end() )
			CHash.insert(p2 -> getCoordinate());
		else { at2 = true; break; }
		p1 = p1 -> getFromPin();
		p2 = p2 -> getFromPin();
	}	
	if( p1 -> operator ==(p2) ) {}
	else if( at1 ) {}
	else {
		assert(at2);
		CHash.erase(CHash.find(p1 -> getCoordinate()));
	}
	for( auto it = CHash.begin(); it != CHash.end(); ++it )
		std::cout << "[" << std::distance(CHash.begin(), it) << "]: " << (*it) << std::endl;
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
/*
	std::cout << "info of OARST: " << std::endl;
	for( unsigned i = 0, n = _OARST.size(); i < n; ++i ) {
		std::pair<Pin*, Pin*> pins = _OARST[i] -> getPins();
		std::cout << "[" << i << "]: " << pins.first -> getName() << ", " << pins.second -> getName() << ", (" << pins.first -> getX() << ", " << pins.first -> getY() << ") - (" << pins.second -> getX() << ", " << pins.second -> getY() << ")" << std::endl;
	}
	std::cout << std::endl;
	std::cout << "size of OARST: " << _OARST.size() << std::endl << std::endl;
*/
}

void Router::reportOARSMT()
{
	std::cout << "info of OARSMT: " << std::endl;
/*
	for( unsigned i = 0, n = _OARSMT.size(); i < n; ++i ) {
		std::pair<Pin*, Pin*> pins = _OARSMT[i] -> getPins();
		std::cout << "[" << i << "]: " << pins.first -> getName() << ", " << pins.second -> getName() << ", (" << pins.first -> getX() << ", " << pins.first -> getY() << ") - (" << pins.second -> getX() << ", " << pins.second -> getY() << ")" << std::endl;
	}
	std::cout << std::endl;
	std::cout << "size of OARSMT: " << _OARSMT.size() << std::endl << std::endl;
*/
	for( auto it = _edgeSet.begin(); it != _edgeSet.end(); ++it ) {
		std::pair<Pin*, Pin*> pins = (*it) -> getPins();
		std::cout << "[" << std::distance(_edgeSet.begin(), it) << "]: " << pins.first -> getName() << ", " << pins.second -> getName() << ", (" << pins.first -> getX() << ", " << pins.first -> getY() << ") - (" << pins.second -> getX() << ", " << pins.second -> getY() << "), size of OARSMT: " << pins.first -> getOARSMT().size() << " and " << pins.second -> getOARSMT().size() << std::endl;
	}
	std::cout << std::endl;
	std::cout << "size of OARSMT: " << _edgeSet.size() << std::endl << std::endl;
}

void Router::reportTurningPins()
{
/*
	std::cout << "info of turning PINs: " << std::endl;
	for( unsigned i = 0, n = _turningPins.size(); i < n; ++i ) {
		std::cout << "name: " << _turningPins[i] -> getName() << ", " << _turningPins[i] -> getCoordinate() << std::endl;
	}
	std::cout << std::endl;
	std::cout << "size of turning PINs: " << _turningPins.size() << std::endl << std::endl;
*/
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
	file.close();
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
	file.close();
}

void Router::writeOARST()
{
/*
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
	file.close();
*/
}

void Router::writeOARSMT()
{
	std::ofstream file;
	file.open("OARSMT.plt");
	file << "set title \"OARSMT.plt\"" << std::endl;
	file << "set size ratio -1" << std::endl;
	file << "set xrange[" << _boundaryBL.x() << ":" << _boundaryTR.x() << "]" << std::endl;
	file << "set yrange[" << _boundaryBL.y() << ":" << _boundaryTR.y() << "]" << std::endl;
	file << "plot \'-\' with linespoints pt 7 ps 1" << std::endl;
/*
	for( unsigned i = 0, n = _OARSMT.size(); i < n; ++i ) {
		std::pair<Pin*, Pin*> pins = _OARSMT[i] -> getPins();
		file << pins.first -> getX() << " " << pins.first -> getY() << std::endl << pins.second -> getX() << " " << pins.second -> getY() << std::endl << std::endl;
	}
*/
	for( auto it = _edgeSet.begin(); it != _edgeSet.end(); ++it ) {
		std::pair<Pin*, Pin*> pins = (*it) -> getPins();
		file << pins.first -> getX() << " " << pins.first -> getY() << std::endl << pins.second -> getX() << " " << pins.second -> getY() << std::endl << std::endl;
	}
	file << "EOF" << std::endl;
	file << "pause -1 \'Press any key\'" << std::endl;
	file.close();
}

void Router::writeSolution()
{
	std::ofstream file;
	file.open("Router.out");
	file << "NumRoutedPins = " << _numPins << std::endl;
	file << "WireLength = " << getCostOARSMT() << std::endl;
/*
	for( unsigned i = 0, n = _OARSMT.size(); i < n; ++i ) {
		Edge* e = _OARSMT[i];
		assert(isHorizontal(e) || isVertical(e));
		std::pair<Pin*, Pin*> pins = e -> getPins();
		if( isHorizontal(e) ) {
			file << "H-line (" << pins.first -> getX() << "," << pins.first -> getY() << ") (" << pins.second -> getX() << "," << pins.second -> getY() << ")" << std::endl;
		}
		else {
			file << "V-line (" << pins.first -> getX() << "," << pins.first -> getY() << ") (" << pins.second -> getX() << "," << pins.second -> getY() << ")" << std::endl;
		}
	}
*/
	for( auto it = _edgeSet.begin(); it != _edgeSet.end(); ++it ) {
		Edge* e = (*it);
		assert(isHorizontal(e) || isVertical(e));
		std::pair<Pin*, Pin*> pins = e -> getPins();
		if( isHorizontal(e) ) {
			file << "H-line (" << pins.first -> getX() << "," << pins.first -> getY() << ") (" << pins.second -> getX() << "," << pins.second -> getY() << ")" << std::endl;
		}
		else {
			file << "V-line (" << pins.first -> getX() << "," << pins.first -> getY() << ") (" << pins.second -> getX() << "," << pins.second -> getY() << ")" << std::endl;
		}
	}
	file.close();
}
