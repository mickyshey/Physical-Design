// File name [ block.h ]

#ifndef BLOCK_H
#define BLOCK_H

#include <iostream>

using namespace std;

class Block
{
public:
	Block() {}
	Block(string s, unsigned w = 0, unsigned h = 0): _name(s), _width(w), _height(h) { _parent = NULL; _left = NULL; _right = NULL; _x = 0; _y = 0; }
	Block(Block* b) { _parent = b -> getParent(); _left = b -> getLeft(); _right = b -> getRight(); }
	~Block() {}

	string getName() { return _name; }
	unsigned getWidth() { return _width; }
	unsigned getHeight() { return _height; }
	Block* getParent() { return _parent; }
	Block* getLeft() { return _left; }
	Block* getRight() { return _right; }
	void setLeft(Block* b) { _left = b; }
	void setRight(Block* b) { _right = b; }
	void setParent(Block* b) { _parent = b; }
	bool isLeaf() { return (_left == NULL && _right == NULL); }
	bool isFull() { return (_left != NULL && _right != NULL); }
	unsigned getX() { return _x; }
	unsigned getRightX() { return _x + _width; }
	unsigned getUpY() { return _y + _height; }
	unsigned getY() { return _y; }
	void setX(unsigned x) { _x = x; }
	void setY(unsigned y) { _y = y; }
	void rotate() { unsigned tmp = _width; _width = _height; _height = tmp; }
	void setToLeaf() { _left = NULL; _right = NULL; }
	void setEqual(Block* b);
	void setToOrigin() { _x = 0; _y = 0; }


private:
	string			_name;
	unsigned		_x;					// x coordinate
	unsigned		_y;					// y coordinate
	unsigned 		_width;
	unsigned 		_height;
	Block*			_parent;
	Block*			_left;
	Block*			_right;
};

#endif
