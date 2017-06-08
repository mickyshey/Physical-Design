// File name [ Router.h ]

#ifndef ROUTER_H
#define ROUTER_H

#include <iostream>
#include <vector>
#include <cassert>
#include <cmath>

class Edge;

class CPoint
{
public:
	CPoint() {}
	CPoint(unsigned x, unsigned y) { _x = x; _y = y; }
	~CPoint() {}
	CPoint& operator = (const CPoint& c) { _x = c._x; _y = c._y; return *this; }
	bool operator < (const CPoint& c) const { return _x < c._x; }

	friend std::ostream& operator << (std::ostream& os, const CPoint& c) {
		os << "x: " << c._x << ", y: " << c._y; return os;
	}
	
	const unsigned& x() { return _x; }
	const unsigned& y() { return _y; }

private:
	unsigned _x;
	unsigned _y;
};

class Pin
{
public:
	Pin() {}
	Pin(std::string n, CPoint c, bool b = true) { _name = n; _coordinate = c; _isPin = b; }
	~Pin() {}
	
	std::string getName() { return _name; }
	const CPoint& getCoordinate() { return _coordinate; }
	const unsigned& getX() { return _coordinate.x(); }
	const unsigned& getY() { return _coordinate.y(); }
	void pushBackOASG(Edge* e) { _OASG.push_back(e); } 
	const std::vector<Edge*>& getOASG() { return _OASG; }
private:
	std::string	_name;
	CPoint	_coordinate;
	bool		_isPin;				// false: turning_pin, true: real_pin
	std::vector<Edge*>	_OASG;
};

class Edge
{
public:
	Edge() {}
	// sorted by x coordinate of pin
	Edge(Pin* a, Pin* b, const unsigned& i, const unsigned& j) { 
		//if( a -> getCoordinate() < b -> getCoordinate() ) _pins.first = a, _pins.second = b; 
		//else _pins.first = b; _pins.second = a; }
		assert(a -> getX() <= b -> getX());
		assert(i < j);
		_pins.first = a; _pins.second = b;
		_pinIndices.first = i; _pinIndices.second = j;
		_added2OAST = false;
		_weight = calWeight(a, b); }
	~Edge() {}
	
	unsigned calWeight(Pin* a, Pin* b) {
		unsigned aX = a -> getX(), aY = a -> getY();
		unsigned bX = b -> getX(), bY = a -> getY();
		assert(aX <= bX);
		unsigned dX = bX - aX;
		unsigned dY = (aY > bY ? aY - bY : bY - aY);
		return dX + dY;
	}
	const std::pair<Pin*, Pin*>& getPins() { return _pins; }
	const std::pair<unsigned ,unsigned>& getPinIndices() { return _pinIndices; }
	const unsigned& getWeight() { return _weight; }
	void setAdded2OAST() { _added2OAST = true; }
	bool added2OAST() { return _added2OAST; }

private:
	unsigned								_weight;
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

	// should be modified !!!
	void OASG();
	bool isNeighbor(Pin* ori, Pin* p, int& upY, int& downY);

	void OAST();

	void reportPin();
	void reportOASG();
	void reportOAST();

private:
	CPoint _boundaryBL;		// bottom left of boundary
	CPoint _boundaryTR;		// top right of boundary
	unsigned _numPins;
	std::vector<Pin*>				_pinList;
	std::vector<Edge*>			_OASG;
	std::vector<Edge*>			_OAST;
};

#endif
