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
	bool operator == (const CPoint& c) { return (_x == c._x) && (_y == c._y); }

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
	Pin(std::string n, CPoint c, bool b = true) { _name = n; _coordinate = c; _isPin = b; }
	~Pin() {}
	
	bool operator == (Pin* p) { return _coordinate == p -> _coordinate; }

	std::string getName() { return _name; }
	const CPoint& getCoordinate() { return _coordinate; }
	const int& getX() { return _coordinate.x(); }
	const int& getY() { return _coordinate.y(); }
	void pushBackOASG(Edge* e) { _OASG.push_back(e); } 
	void pushBackOAST(Edge* e) { _OAST.push_back(e); } 
	const std::vector<Edge*>& getOASG() { return _OASG; }
	const std::vector<Edge*>& getOAST() { return _OAST; }
private:
	std::string	_name;
	CPoint	_coordinate;
	bool		_isPin;				// false: turning_pin, true: real_pin
	std::vector<Edge*>	_OASG;
	std::vector<Edge*>	_OAST;
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
	struct EdgeComparator_dec {
		bool operator () (Edge* a, Edge* b) { return a -> getWeight() < b -> getWeight(); }
	};
	// typedef
	typedef std::priority_queue<Edge*, std::vector<Edge*>, EdgeComparator_dec> EdgeMaxHeap;
	
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
	void makeStraight(Edge* e, Edge* neighbor);
	Pin* getCommon(Edge* e, Edge* neighbor, std::pair<Pin*, Pin*>& endPins);

	void OARSMT();
	void removeOverlappingEdges(std::multiset<Edge*, sortEdgeXcoordinate>& edgeSet, std::unordered_map<std::pair<Pin*, Pin*>, Edge*, pairHashPin>& hashEdge);
	bool isHorizontal(Edge* e);
	bool isVertical(Edge* e);
	Edge* insert2HashEdge(Pin* a, Pin* b, std::unordered_map<std::pair<Pin*, Pin*>, Edge*, pairHashPin>& hashEdge);
	void comparePins(Pin* a, Pin* b, std::pair<Pin*, Pin*>& pins);
	bool overlapWithY(std::pair<Pin*, Pin*>& pins, std::pair<Pin*, Pin*>& nextPins);

	long long getCostOAST();
	long long getCostOARST();
	long long getCostOARSMT();
	void reportPin();
	void reportOASG();
	void reportOAST();
	void reportOARST();
	void reportOARSMT();
	void reportTurningPinsOARST();
	void writeOASG();
	void writeOAST();
	void writeOARST();
	void writeOARSMT();
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
	std::vector<Pin*>				_turningPinsOARST;
	std::vector<Pin*>				_turningPinsOARSMT;
	std::vector<Edge*>			_OARST;
	std::vector<Edge*>			_OARSMT;
};

#endif
