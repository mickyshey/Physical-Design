// File name [ Router.h ]

#ifndef ROUTER_H
#define ROUTER_H

#include <iostream>
#include <vector>

class CPoint
{
public:
	CPoint() {}
	CPoint(unsigned x, unsigned y) { _x = x; _y = y; }
	~CPoint() {}
	CPoint& operator = (CPoint c) { _x = c._x; _y = c._y; return *this; }

	friend std::ostream& operator << (std::ostream& os, const CPoint& c) {
		os << "x: " << c._x << ", y: " << c._y; return os;
	}

private:
	unsigned _x;
	unsigned _y;
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

	void reportPin();

private:
	CPoint _boundaryBL;		// bottom left of boundary
	CPoint _boundaryTR;		// top right of boundary
	unsigned _numPins;
	std::vector<std::string>	_pinName;
	std::vector<CPoint>			_pinCPoint;
};

#endif
