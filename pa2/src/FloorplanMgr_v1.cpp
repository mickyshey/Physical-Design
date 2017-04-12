// File name [ FloorplanMgr.cpp ]

#include <iostream>
#include <fstream>
#include <string>
#include <queue>
#include <algorithm>
#include <climits>
#include <iterator>

#include "FloorplanMgr.h"
#include "Block.h"
#include "Util.h"

using namespace std;

void
FloorplanMgr::readInput(string s1, string s2)
{
	unordered_map<string, Block*> blockMap;
	readInputBlock(s1, blockMap);
	readInputNet(s2, blockMap);
}

void
FloorplanMgr::BTreeInit()
{
	queue<Block*> targetQ;
	_root = _blockList[0];
	targetQ.push(_root);
	
	for( size_t i = 1; i < _blockList.size(); ++i) {
		Block* b = _blockList[i];
		Block* target = targetQ.front();
		if( target -> isFull() ) {
			targetQ.push(target -> getLeft());
			targetQ.push(target -> getRight());
			targetQ.pop();
			target = targetQ.front();
			BTree_insert(_blockList[i], target);
		}
		else
			BTree_insert(_blockList[i], target);

/*		if( target -> getLeft() == NULL && (target -> getRightX()) + (b -> getWidth()) <= _outlineWidth && (target -> getY()) + (b -> getHeight()) <= _outlineHeight ) {
			b -> setX(target -> getRightX());
			BTree_insertLeft(b, target);
		}
		else if( target -> getRight() == NULL && (target -> getX() + b -> getWidth() <= _outlineWidth) && (target -> getUpY() + b -> getHeight() <= _outlineHeight ) ) {
			b -> setX(target -> getX());
			BTree_insertRight(b, target);
			if( target -> getLeft() != NULL ) targetQ.push(target -> getLeft());
			targetQ.push(target -> getRight());
			targetQ.pop();
			target = targetQ.front();
		}
		else {
			if( target -> getLeft() != NULL ) targetQ.push(target -> getLeft());
			if( target -> getRight() != NULL ) targetQ.push(target -> getRight());
			targetQ.pop();
			target = targetQ.front();
		}
*/	
	}
	reportBTree();
}

void
FloorplanMgr::BTreePacking()
{
	updateHcontour(_root);
	BTreePackingLeftRec(_root -> getLeft());
	BTreePackingRightRec(_root -> getRight());
	reportHcontour();
}

void
FloorplanMgr::updateHcontour(Block* b)
{
	unsigned xLeft = b -> getX();
	unsigned xRight = b -> getRightX();
	unsigned yValue = b -> getUpY();
	cout << b -> getName() << ", range: " << xLeft << " ~ " << xRight << ", with y: " << yValue << endl;
	for( auto it = _Hcontour.begin(); it != _Hcontour.end(); ++it ) {
		Xrange xrange = it -> first;
		cout << "checking: " << xrange.first << " ~ " << xrange.second << endl;
		unsigned oriValue = it -> second;
		if( xLeft >= xrange.second ) continue;
		if( xLeft >= xrange.first && xLeft < xrange.second) {
			cout << "case 1 " << endl;
			Xrange tmp = {xrange.first, xLeft};
			if( tmp.first != tmp.second)
				_Hcontour.insert(it, {tmp, oriValue});
			tmp = {xLeft, min(xRight, xrange.second)};
			cout << "tmp: " << tmp.first << ", " << tmp.second << endl;
			_Hcontour.insert(it, {tmp, yValue});
			if( xRight <= xrange.second ) {
				tmp = {xRight, xrange.second};
				_Hcontour.insert(it, {tmp, oriValue});
				_Hcontour.erase(it);
				return;
			}
			_Hcontour.erase(it);
			reportHcontour();
		}
		else if( xRight <= xrange.second ) {
			cout << " case 2 " << endl;
			Xrange tmp = {xrange.first, xRight};
			_Hcontour.insert(it, {tmp, yValue});
			tmp = {max(xRight, xrange.first), xrange.second};
			_Hcontour.insert(it, {tmp, oriValue});
			_Hcontour.erase(it);
			return;
		}
		else {
			cout << "case 3 " << endl;
			_Hcontour.insert(it, {xrange, yValue});
			_Hcontour.erase(it);
		}
	}
}

void
FloorplanMgr::BTreePackingLeftRec(Block* b)
{
	if( b == NULL ) return;
	Block* parent = b -> getParent();
	b -> setX(parent -> getX() + parent -> getWidth());
	b -> setY(parent -> getY());
	updateHcontour(b);	
	reportHcontour();

	BTreePackingLeftRec(b -> getLeft());
	BTreePackingRightRec(b -> getRight());
}

void
FloorplanMgr::BTreePackingRightRec(Block* b)
{
	if( b == NULL ) return;
	Block* parent = b -> getParent();
	b -> setX(parent -> getX());
	unsigned max = HcontourFindMaxY(b);
	cout << "maxY: " << max << endl;
	b -> setY(max);
	updateHcontour(b);
	reportHcontour();

	BTreePackingLeftRec(b -> getLeft());
	BTreePackingRightRec(b -> getRight());
}

unsigned
FloorplanMgr::HcontourFindMaxY(Block* b)
{
	unsigned max = 0;
	unsigned xLeft = b -> getX();
	unsigned xRight = b -> getRightX();
	for( auto it = _Hcontour.begin(); it != _Hcontour.end(); ++it ) {
		Xrange xrange = it -> first;
		unsigned y = it -> second;
		if( xLeft >= xrange.second ) continue;
		if( xRight <= xrange.first ) return max;
		if( y > max ) max = y;
	}
}

//===================================
//		private member function
//===================================

void
FloorplanMgr::readInputBlock(string s, unordered_map<string, Block*>& blockMap)
{
	ifstream file;
	file.open(s);
	string line;
	getline(file, line);
	vector<string> tokens = getTokens(line);
	_outlineWidth = stoi(tokens[1]);
	_Hcontour.push_back({{0, INT_MAX}, 0});		// initial horizontal contour
	_outlineHeight = stoi(tokens[2]);
	getline(file, line);
	tokens = getTokens(line);
	_blockList.reserve(stoi(tokens[1]));
	getline(file, line);
	tokens = getTokens(line);
	// still don't know # terminals
	while( getline(file, line) ) {
		tokens = getTokens(line);
		Block* b = new Block(tokens[0], stoi(tokens[1]), stoi(tokens[2]));
		blockMap.insert({tokens[0], b});
		_blockList.push_back(b);
	}
	file.close();
	//reportBlockList();	
}

void
FloorplanMgr::readInputNet(string s, unordered_map<string, Block*>& blockMap)
{
	ifstream file;
	file.open(s);
	string line;
	getline(file, line);
	vector<string> tokens = getTokens(line);
	_netList.reserve(stoi(tokens[1]));
	unsigned count = 0;
	while( getline(file, line) ) {
		tokens = getTokens(line);
		if( tokens[0] == "NetDegree:" ) {
			int netDegree = stoi(tokens[1]);
			vector<Block*> net;
			net.reserve(netDegree);
			for(int i = 0; i < netDegree; ++i) {
				getline(file, line);
				net.push_back(blockMap.at(line));
				Block* tmp = blockMap.at(line);
			}
			_netList.push_back(net);
		}
	}
	file.close();
	//reportNetList();
}

void
FloorplanMgr::BTree_insert(Block* target, Block* parent)
{
	target -> setParent(parent);
	if( parent -> getLeft() == NULL )
		parent -> setLeft(target);
	else
		parent -> setRight(target);
}

void
FloorplanMgr::BTree_insertLeft(Block* target, Block* parent)
{
	target -> setParent(parent);
	parent -> setLeft(target);
}

void
FloorplanMgr::BTree_insertRight(Block* target, Block* parent)
{
	target -> setParent(parent);
	parent -> setRight(target);
}

//===================================
//		reporting function
//===================================

void
FloorplanMgr::reportBlockList()
{
	for( size_t i = 0; i < _blockList.size(); ++i) {
		Block* b = _blockList[i];
		cout << b -> getName() << "\tWidth: " << b -> getWidth() << "\tHeight: " << b -> getHeight() << endl;
	}
	cout << endl;
}

void
FloorplanMgr::reportNetList()
{
	for( size_t i = 0; i < _netList.size(); ++i) {
		vector<Block*> net = _netList[i];
		cout << "NET:";
		for( size_t j = 0; j < net.size(); ++j) {
			cout << " " << net[j] -> getName();
		}
		cout << endl;
	}
}

void
FloorplanMgr::reportBTree()
{
	reportBTreeRec(_root, 0);	
}

void
FloorplanMgr::reportBTreeRec(Block* b, unsigned level)
{
	for( unsigned i = 0; i < level; ++i )
		cout << "   "; 
	cout << "[" << level << "] ";
	if( b == NULL ) { cout << "NULL" << endl; return; }
	else cout << b -> getName() << endl;
	reportBTreeRec(b -> getLeft(), level + 1);
	reportBTreeRec(b -> getRight(), level + 1);
}

void
FloorplanMgr::reportHcontour()
{
	for( auto it = _Hcontour.begin(); it != _Hcontour.end(); ++it ) {
		Xrange xrange = it -> first;
		cout << xrange.first << " ~ " << xrange.second << ": " << it -> second << endl;
	}
}
