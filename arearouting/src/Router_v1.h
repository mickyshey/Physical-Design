// File name [ Router.h ]

#ifndef ROUTER_H
#define ROUTER_H

#include <iostream>
#include <vector>
#include <cassert>
#include <cmath>
#include <cstdlib>
#include <queue>
#include <unordered_map>
#include <set>

class Edge;

class CPoint
{
public:
	CPoint() {}
	CPoint(int x, int y) { _x = x; _y = y; }
	~CPoint() {}
	CPoint& operator = (const CPoint& c) { _x = c._x; _y = c._y; return *this; }
	bool operator == (const CPoint& c) const { return (_x == c._x) && (_y == c._y); }

	friend std::ostream& operator << (std::ostream& os, const CPoint& c) {
		os << "x: " << c._x << ", y: " << c._y; return os;
	}
	
	const int& x() const { return _x; }
	const int& y() const { return _y; }

private:
	int _x;
	int _y;
};

class Pin
{
public:
	Pin() {}
	Pin(std::string n, CPoint c, bool b = true) { _name = n; _coordinate = c; _isPin = b; _visitedInBFS = false; }
	~Pin() {}
	
	bool operator == (Pin* p) { return _coordinate == p -> _coordinate; }

	std::string getName() { return _name; }
	const CPoint& getCoordinate() const { return _coordinate; }
	const int& getX() { return _coordinate.x(); }
	const int& getY() { return _coordinate.y(); }
	void pushBackOASG(Edge* e) { _OASG.push_back(e); } 
	const std::vector<Edge*>& getOASG() { return _OASG; }
	void pushBackOAST(Edge* e) { _OAST.push_back(e); } 
	const std::vector<Edge*>& getOAST() { return _OAST; }
	void pushBackOARSMT(Edge* e) { _OARSMT.push_back(e); }
	const std::vector<Edge*>& getOARSMT() { return _OARSMT; }
	void setFromPin(Pin* p) { _fromPin = p; }
	Pin* getFromPin() { return _fromPin; }
	void setVisitedInBFS() { _visitedInBFS = true; }
	bool visitedInBFS() { return _visitedInBFS; }
	bool isRealPin() { return _isPin; }
private:
	std::string	_name;
	CPoint	_coordinate;
	bool		_isPin;				// false: turning_pin, true: real_pin
	std::vector<Edge*>	_OASG;
	std::vector<Edge*>	_OAST;
	std::vector<Edge*>	_OARSMT;
	bool		_visitedInBFS;
	Pin*	_fromPin;
};

class Edge
{
public:
	Edge() {}
	// sort by x coordinate !! for OARSMT
	Edge(Pin* a, Pin* b, const int& w = 0) {
		if( a -> getX() < b -> getX() ) { _pins.first = a; _pins.second = b; }
		else { _pins.first = b; _pins.second = a; }
		//_pins.first = a; _pins.second = b;
		_added2OAST = false;
		_visitedInOARST = false;
		_visitedInOARSMT = false;
		_weight = w; 
		assert(_maxHeap.empty()); }
	~Edge() {}

	// for maxHeap comparison
	struct MaxHeapComparator {
		bool operator () (Edge* a, Edge* b) { return a -> getWeight() < b -> getWeight(); }
		// sort by Y (distinct pins) , not weight any more
/*
		bool operator () (Edge* a, Edge* b) {
			//std::cout << "a: " << a -> getPins().first -> getCoordinate() << " to " << a -> getPins().second -> getCoordinate() << ", b: " << b -> getPins().first -> getCoordinate() << " to " << b -> getPins().second -> getCoordinate() << std::endl;
			int aY = a -> getPins().first -> getY() > a -> getPins().second -> getY() ? a -> getPins().first -> getY() : a -> getPins().second -> getY();
			int bY = b -> getPins().first -> getY() > b -> getPins().second -> getY() ? b -> getPins().first -> getY() : b -> getPins().second -> getY();
			return aY < bY;
		}
*/
	};
	// typedef
	typedef std::priority_queue<Edge*, std::vector<Edge*>, MaxHeapComparator> EdgeMaxHeap;
	
	const std::pair<Pin*, Pin*>& getPins() { return _pins; }
	const std::pair<unsigned ,unsigned>& getPinIndices() { return _pinIndices; }
	const int& getWeight() { return _weight; }
	void setAdded2OAST() { _added2OAST = true; }
	bool added2OAST() { return _added2OAST; }
	void setVisitedInOARST() { _visitedInOARST = true; }
	bool visitedInOARST() { return _visitedInOARST; }
	void setVisitedInOARSMT() { _visitedInOARSMT = true; }
	bool visitedInOARSMT() { return _visitedInOARSMT; }
	void setPinIdxFirst(unsigned idx) { _pinIndices.first = idx; }
	void setPinIdxSecond(unsigned idx) { _pinIndices.second = idx; }
	void push2MaxHeap(Edge* e) { _maxHeap.push(e); }
	EdgeMaxHeap& getMaxHeap() { return _maxHeap; }

private:
	int								_weight;
	std::pair<Pin*, Pin*> 			_pins;
	std::pair<unsigned, unsigned> _pinIndices;			// in OAST
	bool		_added2OAST;										// in OAST
	bool		_visitedInOARST;									// in OARST
	EdgeMaxHeap	_maxHeap;										// in OARST
	bool		_visitedInOARSMT;
};

struct CPointHash {
	size_t operator () (const CPoint& c) const {
		return (std::hash<int>() (c.x())
					^ (std::hash<int>() (c.y())) << 1);
	}
};
struct pairHashPin {
	size_t operator () (const std::pair<Pin*, Pin*>& pins) const {
		return (std::hash<int>() (pins.first -> getX())
					^ (std::hash<int>() (pins.first -> getY())) << 1
					^ (std::hash<int>() (pins.second -> getX())) << 2
					^ (std::hash<int>() (pins.second -> getY())) >> 1);
	}
};
struct sortEdgeXcoordinate {
	bool operator () (Edge* a, Edge* b) { 
		assert(a -> getPins().first -> getX() <= a -> getPins().second -> getX());
		assert(b -> getPins().first -> getX() <= b -> getPins().second -> getX());
		return (a -> getPins().first -> getX() < b -> getPins().first -> getX());
	}
};

class Router
{
public:
	Router() {}
	~Router() {}
	
	void setNumPins(unsigned n)	{ _numPins = n; }
	void setBoundaryBL(CPoint c)	{ _boundaryBL = c; }
	void setBoundaryTR(CPoint c) 	{ _boundaryTR = c; }
	const unsigned& getNumPins() const	{ return _numPins; }
	const CPoint& getBoundaryBL()	const	{ return _boundaryBL; }
	const CPoint& getBoundaryTR() const { return _boundaryTR; }
	
	void readInput(const std::string& s);

	void OASG();
	bool isNeighbor(Pin* ori, Pin* p, int& upY, int& downY);

	void OAST();
	void setPinIndices4OAST();

	void OARST();
	void makeStraight(Edge* e, Edge* neighbor, std::unordered_map<CPoint, Pin*, CPointHash>& hashPin);
	Pin* getCommon(Edge* e, Edge* neighbor, std::pair<Pin*, Pin*>& endPins);
	void makeStraight(Edge* e);
	Pin* insert2HashPin(const std::string& name, const CPoint& c, std::unordered_map<CPoint, Pin*, CPointHash>& hashPin);

	void OARSMT();
	void removeOverlappingEdges(std::multiset<Edge*, sortEdgeXcoordinate>& edgeSet, std::unordered_map<std::pair<Pin*, Pin*>, Edge*, pairHashPin>& hashEdge);
	bool isHorizontal(Edge* e);
	bool isVertical(Edge* e);
	Edge* insert2HashEdge(Pin* a, Pin* b, std::unordered_map<std::pair<Pin*, Pin*>, Edge*, pairHashPin>& hashEdge);
	void comparePins(Pin* a, Pin* b, std::pair<Pin*, Pin*>& pins);
	bool overlapWithY(std::pair<Pin*, Pin*>& pins, std::pair<Pin*, Pin*>& nextPins);
	void detectCycleAndFix(std::multiset<Edge*, sortEdgeXcoordinate>& edgeSet);
	void fixCycle(const std::pair<Pin*, Pin*>& pins, std::multiset<Edge*, sortEdgeXcoordinate>& edgeSet);
	bool isCyclePin(Pin* p, int ,int ,int ,int);
	bool isWithinCycle(Pin* p, Edge* e, int, int, int, int);

	long long getCostOAST();
	long long getCostOARST();
	long long getCostOARSMT();
	void reportPin();
	void reportOASG();
	void reportOAST();
	void reportOARST();
	void reportOARSMT();
	void reportTurningPins();
	void reportCycles(const std::pair<Pin*, Pin*>& pins);
	void writeOASG();
	void writeOAST();
	void writeOARST();
	void writeOARSMT();
	void writeSolution();
	int calWeight(Edge* e) {
		return calWeight(e -> getPins().first, e -> getPins().second);
	}
	int calWeight(Pin* a, Pin* b) {
		int aX = a -> getX(), aY = a -> getY();
		int bX = b -> getX(), bY = b -> getY();
		return std::abs(bX - aX) + std::abs(bY - aY);
	}

private:
	CPoint _boundaryBL;		// bottom left of boundary
	CPoint _boundaryTR;		// top right of boundary
	unsigned _numPins;
	std::vector<Pin*>				_pinList;
	std::vector<Edge*>			_OASG;
	std::vector<Edge*>			_OAST;
	std::vector<Pin*>				_turningPins;
	std::vector<Edge*>			_OARST;
	std::vector<Edge*>			_OARSMT;
	std::unordered_map<CPoint, Pin*, CPointHash>	_hashPin;
};

#endif
