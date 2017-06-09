// File name [ Router.h ]

#ifndef ROUTER_H
#define ROUTER_H

#include <iostream>
#include <vector>
#include <cassert>
#include <cmath>
#include <cstdlib>

class Edge;

class CPoint
{
public:
	CPoint() {}
	CPoint(int x, int y) { _x = x; _y = y; }
	~CPoint() {}
	CPoint& operator = (const CPoint& c) { _x = c._x; _y = c._y; return *this; }

	friend std::ostream& operator << (std::ostream& os, const CPoint& c) {
		os << "x: " << c._x << ", y: " << c._y; return os;
	}
	
	const int& x() { return _x; }
	const int& y() { return _y; }

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
	// sorted by x coordinate of pin
	Edge(Pin* a, Pin* b, const int& w) {
		_pins.first = a; _pins.second = b;
		_added2OAST = false;
		_weight = w; }
	~Edge() {}
	
	const std::pair<Pin*, Pin*>& getPins() { return _pins; }
	const std::pair<unsigned ,unsigned>& getPinIndices() { return _pinIndices; }
	const int& getWeight() { return _weight; }
	void setAdded2OAST() { _added2OAST = true; }
	bool added2OAST() { return _added2OAST; }
	void setPinIdxFirst(unsigned idx) { _pinIndices.first = idx; }
	void setPinIdxSecond(unsigned idx) { _pinIndices.second = idx; }

private:
	int								_weight;
	std::pair<Pin*, Pin*> 			_pins;
	std::pair<unsigned, unsigned> _pinIndices;
	bool		_added2OAST;
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

	void reportPin();
	void reportOASG();
	void reportOAST();
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
};

#endif
