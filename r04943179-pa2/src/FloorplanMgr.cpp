// File name [ FloorplanMgr.cpp ]

#include <iostream>
#include <fstream>
#include <string>
#include <queue>
#include <climits>
#include <cstdlib>
#include <iomanip>
#include <cassert>
#include <cmath>
#include <map>

#include "FloorplanMgr.h"
#include "Block.h"
#include "Util.h"

#define RANDOM_CONSTRUCT 100
//#define ITERATION_COUNT 1000 
#define ITERATION_COUNT 20 
#define THRESHOLD 1000
#define TRY 2

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
	for( size_t i = 0; i < _blockList.size(); ++i)
		_blockList[i] -> reset();

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
	}
	BTreePacking();
	//reportBTree();
}

void
FloorplanMgr::computeAvg()
{
	BTreeInit();
	bool legal = true;
	unsigned area = BTreeGetArea(legal);
	unsigned length = BTreeGetWireLength();
	unsigned outOfRangeCount = legal ? 0 : 1;

	for( int i = 1; i < RANDOM_CONSTRUCT; ++i) {			// randomly construct 99 BTree + init
		unsigned num = rand() % 3;	
		if( num == 0 ) 		BlockRotate();
		else if( num == 1 ) BlockDeleteAndInsert(0);		// don't duplicate BTree here
		else				BlockSwap();
		BTreePacking();
		//reportBTree();

		legal = true;
		area += BTreeGetArea(legal);
		length += BTreeGetWireLength();
		if( !legal ) ++outOfRangeCount;
	}

	_avgArea = (double)area / (double)RANDOM_CONSTRUCT;
	_avgWireLength = (double)length / (double)RANDOM_CONSTRUCT;
	cerr << "avgArea: " << fixed << _avgArea << endl;
	cerr << "avgWireLength: " << fixed << _avgWireLength << endl;
	cerr << "# illegal: " << outOfRangeCount << endl;
}

void
FloorplanMgr::simAnnealing()
{
	//unsigned itCount = (_blockList.size() > 10 ? ITERATION_COUNT : ITERATION_COUNT / 2);
	unsigned itCount = _blockList.size() * ITERATION_COUNT;
	_start = clock();	
	double cost;
	double bestCost = INT_MAX;
	Block* bestBTree = NULL;
	unsigned SA_count = 1;
	unsigned fit_count = 1;
	while( 1 ) {
		BTreeInit();
		cost = getCost();
		bool findFit = false;
		double T = 1e08; 
		while( T > 1e-03 ) {
			unsigned iterationCount = 0;
			//while( iterationCount <= ITERATION_COUNT ) {
			while( iterationCount <= itCount ) {
				unsigned num = rand() % 3;
				double currCost;
				++iterationCount;
				if( num == 0 ) {
					unsigned idx = BlockRotate();
					BTreePacking();
					if( findFit ) {
						if( isFit() ) {
							currCost = getCost();
							if( currCost > cost && (double)rand() / RAND_MAX > exp((cost - currCost) / T) )	undoRotate(idx);						
							else cost = currCost;
						}
						else {
							undoRotate(idx);
						}
					}
					else {
						currCost = getCost();
						if( isFit() ) {
							findFit = true;
							cost = currCost;
						}
						else {
							if( currCost > cost && (double)rand() / RAND_MAX > exp((cost - currCost) / T) ) undoRotate(idx); 
							else cost = currCost;
						}
					}
				}
				else if( num == 1 ) {
					Block* newRoot = BlockDeleteAndInsert(1);	
					BTreePacking();
					if( findFit ) {
						if( isFit() ) {
							currCost = getCost();
							if( currCost > cost && (double)rand() / RAND_MAX > exp((cost - currCost) / T) )	undoDeleteAndInsert(newRoot);						
							else cost = currCost;
						}
						else {
							undoDeleteAndInsert(newRoot);
						}
					}
					else {
						currCost = getCost();
						if( isFit() ) {
							findFit = true;
							cost = currCost;
						}
						else {
							if( currCost > cost && (double)rand() / RAND_MAX > exp((cost - currCost) / T) ) undoDeleteAndInsert(newRoot); 
							else cost = currCost;
						}
					}
				}
				else {
					pair<unsigned, unsigned> swapIdx = BlockSwap();
					BTreePacking();
					if( findFit ) {
						if( isFit() ) {
							currCost = getCost();
							if( currCost > cost && (double)rand() / RAND_MAX > exp((cost - currCost) / T) )	undoSwap(swapIdx);						
							else cost = currCost;
						}
						else {
							undoSwap(swapIdx);
						}
					}
					else {
						currCost = getCost();
						if( isFit() ) {
							findFit = true;
							cost = currCost;
						}
						else {
							if( currCost > cost && (double)rand() / RAND_MAX > exp((cost - currCost) / T) ) undoSwap(swapIdx); 
							else cost = currCost;
						}
					}
				}
			cerr << "time: " << getTime() << ", T: " << fixed << T << ", current cost: " << cost << "               \r" << flush;
			}
			T *= 0.9;
		}	
		//cout << endl << endl << "# SA: " << SA_count << endl;
		++SA_count;
		if( SA_count > 4 && bestBTree ) break; 
		if( !isFit() ) continue;	
		//if( cost > THRESHOLD ) continue;
		if( cost < bestCost ) {
			bestCost = cost;
			//cout << "find better solution ..." << endl;
			unsigned maxX = BTreeGetMaxX();
			unsigned maxY = BTreeGetMaxY();
			//cout << "area: " << fixed << maxX * maxY << endl;
			//cout << "length: " << fixed << BTreeGetWireLength() << endl;
			if( bestBTree ) BTreeFree(bestBTree);
			bestBTree = BTreeDuplicate();
		}
		if( fit_count > 1 ) break;
		++fit_count;
	}
	BTreeExchange(bestBTree);
	BTreePacking();
	bool legal = false;
	unsigned area = BTreeGetArea(legal);
	unsigned length = BTreeGetWireLength();
	cerr << endl << endl << endl;
	cerr << "time: " << fixed << getTime() << endl;
	cerr << "legal: " << legal << endl;
	cerr << "area: " << fixed << area << endl;
	cerr << "wire length: " << fixed << length << endl;
	cerr << "cost: " << fixed << _alpha * (double)area + (1 - _alpha) * (double)length << endl;
	//reportBTree();
	//reportHcontour();
	//reportVcontour();
}

// (1) maintain _blockList and _netList
// (2) free the origin BTree
// (3) set _root = newRoot
void
FloorplanMgr::BTreeExchange(Block* newRoot)
{
	vector<Block*> newBlockList;
	vector<vector<Block*>> newNetList;
	newBlockList.reserve(_blockList.size());
	newNetList.reserve(_netList.size());	
	unordered_map<string, Block*> newBlockMap;
	setBlockMap(newRoot, newBlockMap);
	for( size_t i = 0; i < _blockList.size(); ++i )
		newBlockList.push_back(newBlockMap.at(_blockList[i] -> getName()));
	for( size_t i = 0; i < _netList.size(); ++i) {
		vector<Block*> newNet;
		newNet.reserve(_netList[i].size());
		for( size_t j = 0; j < _netList[i].size(); ++j ) {
			Block* b = _netList[i][j];
			if( newBlockMap.find(b -> getName()) != newBlockMap.end() )
				newNet.push_back(newBlockMap.at(b -> getName()));
			else newNet.push_back(b);				// terminal
		}
		newNetList.push_back(newNet);
	}
	_blockList.swap(newBlockList);
	_netList.swap(newNetList);
	BTreeFree(_root);
	_root = newRoot;
}

void
FloorplanMgr::setBlockMap(Block* r, unordered_map<string, Block*>& m)
{
	Block* left = r -> getLeft();
	Block* right = r -> getRight();
	if( left ) setBlockMap(left, m);
	if( right ) setBlockMap(right, m);
	m.insert({r -> getName(), r});
}

double
FloorplanMgr::getCost()
{
	double cost = 0;
	unsigned maxX = BTreeGetMaxX();
	unsigned maxY = BTreeGetMaxY();	
	unsigned area = maxX * maxY;
	unsigned length = BTreeGetWireLength();
	cost += _alpha * double(area) / _avgArea;
	cost += (1 - _alpha ) * double(length) / _avgWireLength;
	if( maxX > _outlineWidth ) cost += 100000 * (maxX - _outlineWidth);
	if( maxY > _outlineHeight ) cost += 100000 * (maxY - _outlineHeight);
//	if( cost < THRESHOLD ) {
//		cout << "fit: " << isFit() << ", maxX: " << maxX << ", outlineW: " << _outlineWidth << ", maxY: " << maxY << ", outlineH: " << _outlineHeight << endl;
//		cout << "area: " << area << ", length: " << length << endl;
//	}
	return cost;
}

bool
FloorplanMgr::isFit()
{
	unsigned maxX = BTreeGetMaxX();
	unsigned maxY = BTreeGetMaxY();
	return (maxX <= _outlineWidth && maxY <= _outlineHeight);
}

void
FloorplanMgr::writeOutput(string s)
{
	ofstream file;
	file.open(s);
	unsigned maxX = BTreeGetMaxX();
	unsigned maxY = BTreeGetMaxY();
	unsigned area = maxX * maxY;
	unsigned length = BTreeGetWireLength();
	double cost = _alpha * (double)area + (1 - _alpha) * (double)length;
	file << fixed << cost << endl;
	file << fixed << length << endl;
	file << fixed << area << endl;
	file << maxX << " " << maxY << endl;
	file << fixed << getTime() << endl;
	for( unsigned i = 0; i < _blockList.size(); ++i ) {
		Block* b = _blockList[i];
		file << b -> getName() << " " << b -> getX() << " " << b -> getY()
			 << " " << b -> getRightX() << " " << b -> getUpY() << endl;
	}
	file.close();
}

void
FloorplanMgr::writeLog(string s)
{
	ofstream file;
	file.open(s);
	unsigned maxX = BTreeGetMaxX();
	unsigned maxY = BTreeGetMaxY();
	unsigned area = maxX * maxY;
	unsigned length = BTreeGetWireLength();
	double cost = _alpha * (double)area + (1 - _alpha) * (double)length;
	file << "var log = {" << endl;
	file << "\"boundary\": [" << _outlineWidth << ", " << _outlineHeight << "]," << endl;
	file << "\"trace\": [" << endl;
	file << "{" << endl;
	file << "\"temp\": \"T\"," << endl;
	file << "\"cost\": \"" << cost << "\"," << endl;
	file << "\"packing\": [" << endl;
	for( unsigned i = 0; i < _blockList.size(); ++i ) {
		Block* b = _blockList[i];
		file << "[\"" << b -> getName() << "\", " << b -> getX() << ", " << b -> getY() << ", " << b -> getRightX() << ", " << b -> getUpY() << "]" << (i == _blockList.size() - 1 ? "" : ",") << endl;
	}
	file << "]" << endl;
	file << "}" << endl;
	file << "]}" << endl;
	file.close();
}

void
FloorplanMgr::BTreePacking()
{
	FloorplanMgr::reset();
	updateContour(_root);
	BTreePackingLeftRec(_root -> getLeft());
	BTreePackingRightRec(_root -> getRight());
	//reportBTree();
	//reportHcontour();
	//reportVcontour();
}

void
FloorplanMgr::updateHcontour(Block* b)
{
	unsigned xLeft = b -> getX();
	unsigned xRight = b -> getRightX();
	unsigned yValue = b -> getUpY();
	//cout << "update H: " << b -> getName() << ", range: " << xLeft << " ~ " << xRight << ", with y: " << yValue << endl;
	for( auto it = _Hcontour.begin(); it != _Hcontour.end(); ++it ) {
		Xrange xrange = it -> first;
		//cout << "checking: " << xrange.first << " ~ " << xrange.second << endl;
		unsigned oriValue = it -> second;
		if( xLeft >= xrange.second ) continue;
		if( xRight <= xrange.first ) return;
		if( yValue > oriValue ) {
			if( xLeft <= xrange.first && xRight >= xrange.second ) {				
				it -> second = yValue;
			}
			else if( xLeft <= xrange.first && xRight < xrange.second ) {
				Xrange tmp = {xrange.first, xRight};
				_Vcontour.insert(it, {tmp, yValue});	
				(it -> first).first = xRight;				
				//it -> second = oriValue;
			}
			else {
				Xrange tmp = {xrange.first, xLeft};
				_Vcontour.insert(it, {tmp, oriValue});		
				(it -> first).first = xLeft;					
				it -> second = yValue;
			}
		}
	}
}

void 
FloorplanMgr::updateVcontour(Block* b)
{
	unsigned yDown = b -> getY();
	unsigned yUp = b -> getUpY();
	unsigned xValue = b -> getRightX();
	//cout << "update V: " << b -> getName() << ", range: " << yDown << " ~ " << yUp << ", with x: " << xValue << endl;
	for( auto it = _Vcontour.begin(); it != _Vcontour.end(); ++it ) {
		Yrange yrange = it -> first;
		//cout << "checking: " << yrange.first << " ~ " << yrange.second << endl;
		unsigned oriValue = it -> second;
		if( yDown >= yrange.second ) continue;
		if( yUp <= yrange.first ) return;
		if( xValue > oriValue ) {
			if( yDown <= yrange.first && yUp >= yrange.second ) {				
				it -> second = xValue;
			}
			else if( yDown <= yrange.first && yUp < yrange.second ) {
				Yrange tmp = {yrange.first, yUp};
				_Vcontour.insert(it, {tmp, xValue});	
				(it -> first).first = yUp;		
				//it -> second = oriValue;
			}
			else {
				Yrange tmp = {yrange.first, yDown};
				_Vcontour.insert(it, {tmp, oriValue});	
				(it -> first).first = yDown;		
				it -> second = xValue;
			}
		}
	}
}

void
FloorplanMgr::BTreePackingLeftRec(Block* b)
{
	if( b == NULL ) return;
	Block* parent = b -> getParent();
	b -> setY(parent -> getY());					// this is not a good packing !!!
	unsigned maxX = VcontourFindMaxX(b);
	b -> setX(maxX);
	updateContour(b);
	//reportHcontour();
	//reportVcontour();

	BTreePackingLeftRec(b -> getLeft());
	BTreePackingRightRec(b -> getRight());
}

void
FloorplanMgr::BTreePackingRightRec(Block* b)
{
	if( b == NULL ) return;
	Block* parent = b -> getParent();
	b -> setX(parent -> getX());
	unsigned maxY = HcontourFindMaxY(b);
	b -> setY(maxY);
	updateContour(b);
	//reportHcontour();
	//reportVcontour();

	BTreePackingLeftRec(b -> getLeft());
	BTreePackingRightRec(b -> getRight());
}

unsigned
FloorplanMgr::HcontourFindMaxY(Block* b)
{
	unsigned maxY = 0;
	unsigned xLeft = b -> getX();
	unsigned xRight = b -> getRightX();
	for( auto it = _Hcontour.begin(); it != _Hcontour.end(); ++it ) {
		Xrange xrange = it -> first;
		unsigned y = it -> second;
		if( xLeft >= xrange.second ) continue;
		if( xRight <= xrange.first ) return maxY;
		if( y > maxY ) maxY = y;
	}
	return maxY;
}

unsigned
FloorplanMgr::VcontourFindMaxX(Block* b)
{
	unsigned maxX = 0;
	unsigned yDown = b -> getY();
	unsigned yUp = b -> getUpY();
	for( auto it = _Vcontour.begin(); it != _Vcontour.end(); ++it ) {
		Yrange yrange = it -> first;
		unsigned x = it -> second;
		if( yDown >= yrange.second ) continue;
		if( yUp <= yrange.first ) return maxX;
		if( x > maxX ) maxX = x;
	}
	return maxX;
}

unsigned
FloorplanMgr::BTreeGetArea(bool& legal)
{
	unsigned maxX = BTreeGetMaxX();
	unsigned maxY = BTreeGetMaxY(); 
	if( maxX > _outlineWidth || maxY > _outlineHeight ) legal = false;
	else legal = true;
	return maxX * maxY;
}

unsigned
FloorplanMgr::BTreeGetMaxX()
{
	unsigned maxX = 0;
	for( auto it = _Vcontour.begin(); it != _Vcontour.end(); ++it ) {
		unsigned x = it -> second;
		if( x > maxX ) maxX = x;
	}
	return maxX;
}

unsigned
FloorplanMgr::BTreeGetMaxY()
{
	unsigned maxY = 0;
	for( auto it = _Hcontour.begin(); it != _Hcontour.end(); ++it ) {
		unsigned y = it -> second;
		if( y > maxY ) maxY = y;
	}
	return maxY;
}

unsigned
FloorplanMgr::BTreeGetWireLength()
{
	unsigned length = 0;
	for( size_t i = 0; i < _netList.size(); ++i ) {
		length += getNetLength(_netList[i]);
	}
	return length;
}

unsigned
FloorplanMgr::getNetLength(vector<Block*> n)
{
	unsigned maxX = 0, maxY = 0;
	unsigned minX = INT_MAX, minY = INT_MAX;
	for( size_t i = 0; i < n.size(); ++i ) {
		Block* b = n[i];
		unsigned x = b -> getX() + b -> getWidth() / 2;
		unsigned y = b -> getY() + b -> getHeight() / 2;
		if( x > maxX ) maxX = x;
		if( x < minX ) minX = x;
		if( y > maxY ) maxY = y;
		if( y < minY ) minY = y;
	}
	return (maxX - minX) + (maxY - minY);
}

unsigned
FloorplanMgr::BlockRotate()
{
	unsigned idx = rand() % _blockList.size();
	//cout << "rotating " << _blockList[idx] -> getName() << endl;
	_blockList[idx] -> rotate();
	return idx;
}

Block*
FloorplanMgr::BlockDeleteAndInsert(bool d)
{
	Block* DuplicatedRoot = NULL;
	if( d ) {
		DuplicatedRoot = BTreeDuplicate();
	}
	unsigned idxD = rand() % _blockList.size();
	unsigned idxI;
	deleteBlock(_blockList[idxD]);
	//cout << _blockList[idxD] -> getName() << " deleted ... " << endl;
	while( 1 ) {
		idxI = rand() % _blockList.size();
		Block* i = _blockList[idxI];
		if( idxD != idxI && !(i -> isFull()) ) {
			_blockList[idxD] -> setParent(i);
			if( i -> isLeaf() ) {
				if( rand() % 2 == 0 ) i -> setLeft(_blockList[idxD]);
				else i -> setRight(_blockList[idxD]);
			}
			else {
				if( i -> getLeft() == NULL ) i -> setLeft(_blockList[idxD]);
				else i -> setRight(_blockList[idxD]);
			}
			//cout << "inserting to: " << i -> getName() << endl;
			return DuplicatedRoot;
		}	
	}
}

void
FloorplanMgr::deleteAndInsert(Block* d, Block* i)
{
/*
	unsigned side;
	if( d -> getParent() == i ) {
		if( i -> getLeft() == d ) side = 0;		// record side info before deleting
		else side = 1;
		deleteBlock(d);
		if( side == 0 ) i -> setRight(d);
		else i -> setLeft(d);	
	}
	else if( i -> getParent() == d  && d -> isFull() && !i -> isLeaf() ) {		// special case
	// i is a child of d
		deleteBlock(d, i);
		if( i -> getLeft() == NULL ) i -> setLeft(d);
		else i -> setRight(d); 
	}
	else {
		deleteBlock(d);
		if( i -> isLeaf() ) {
			if( rand() % 2 == 0 ) i -> setLeft(d);
			else i -> setRight(d);
		}
		else if( i -> getLeft() == NULL ) i -> setLeft(d);
		else i -> setRight(d);
	}
	d -> setParent(i);
*/
}

void
FloorplanMgr::deleteBlock(Block* b)
{
	Block* p = b -> getParent();
	if( b -> isLeaf() ) {					// leaf node
		if( p -> getLeft() == b ) p -> setLeft(NULL);
		else p -> setRight(NULL);
	}
	else if( b -> isFull() ) {				// node with two children
		unsigned side = rand() % 2;
		Block* movingNode = (side == 0 ? b -> getLeft() : b -> getRight());
		Block* stayingNode = (side == 0 ? b -> getRight() : b -> getLeft());
		movingNode -> setParent(p);
		stayingNode -> setParent(movingNode);
		BlockMoveUp(movingNode, side);
		if( side == 0 ) movingNode -> setRight(stayingNode);
		else movingNode -> setLeft(stayingNode);
		if( b != _root ) {
			if( p -> getLeft() == b ) p -> setLeft(movingNode);		// parent point to moving node
			else p -> setRight(movingNode);
		}
		else _root = (side == 0 ? b -> getLeft() : b -> getRight());
	}
	else {									// node with one child
		Block* c = (b -> getLeft() == NULL ? b -> getRight() : b -> getLeft());
		c -> setParent(p);
		if( b != _root ) {
			if( p -> getLeft() == b ) p -> setLeft(c);
			else p -> setRight(c);
		}
		else _root = (b -> getLeft() == NULL ? b -> getRight() : b -> getLeft());
	}
	b -> setToLeaf();
}

void
FloorplanMgr::deleteBlock(Block* d, Block* i)
{
// i is a child of d
/*
	Block* p = d -> getParent();
	unsigned side = (d -> getLeft() == i ? 0 : 1);
	Block* t = (side == 0 ? d -> getRight() : d -> getLeft());
	t -> setParent(p);
	i -> setParent(t);
	BlockMoveUp(t, (side == 0 ? 1 : 0));	// the moving up node is on other side
	if( d != _root ) {
		if( p -> getLeft() == d ) p -> setLeft(t);
		else p -> setRight(t);
	}
	else _root = t;
	if( side == 0 ) t -> setLeft(i);
	else t -> setRight(i);
	d -> setToLeaf();
*/
}

void
FloorplanMgr::BlockMoveUp(Block* b, unsigned upSide)
{
	if( b -> isFull() ) {
		unsigned side = rand() % 2;
		Block* movingNode = (side == 0 ? b -> getLeft() : b -> getRight());
		Block* stayingNode = (side == 0 ? b -> getRight() : b -> getLeft());
		stayingNode -> setParent(movingNode);
		BlockMoveUp(movingNode, side);
		if( side == 0 ) movingNode -> setRight(stayingNode);
		else movingNode -> setLeft(stayingNode);
		if( upSide == 0 ) b -> setLeft(movingNode);		// point back to moving node
		else b -> setRight(movingNode);
	}
	else if( !b -> isLeaf() ) {			// block with one child
		if( upSide == 0 && b -> getLeft() == NULL ) {			// moving right child to left
			b -> setLeft(b -> getRight());
			b -> setRight(NULL);
		}
		else if( upSide == 1 && b -> getRight() == NULL ) {		// moving left child to right
			b -> setRight(b -> getLeft());
			b -> setLeft(NULL);
		}
	}
}

Block*
FloorplanMgr::BTreeDuplicate()
{
	Block* newRoot = new Block(_root);
	BTreeDuplicateRec(_root, newRoot);
	unsigned count = 0;
	//reportBTreeRec(newRoot, 0, count);
	return newRoot;
}

void
FloorplanMgr::BTreeDuplicateRec(Block* t, Block* d)
{
	Block* _left = t -> getLeft();
	Block* _right = t -> getRight();
	if( _left ) {
		Block* newLeft = new Block(_left);
		d -> setLeft(newLeft);
		newLeft -> setParent(d);
		BTreeDuplicateRec(_left, newLeft);
	}
	if( _right ) {
		Block* newRight = new Block(_right);
		d -> setRight(newRight);
		newRight -> setParent(d);
		BTreeDuplicateRec(_right, newRight);
	}
}

void
FloorplanMgr::BTreeFree(Block* b)
{
	Block* left = b -> getLeft();
	Block* right = b -> getRight();
	if( left ) BTreeFree(left);
	if( right ) BTreeFree(right);
	delete b;
}


pair<unsigned, unsigned>
FloorplanMgr::BlockSwap()
{
	unsigned idxA = rand() % _blockList.size();
	unsigned idxB;
	while( 1 ) {
		idxB = rand() % _blockList.size();
		if( idxA != idxB ) break;
	}
	//cout << "swapping " << _blockList[idxA] -> getName() << " and " << _blockList[idxB] -> getName() << endl;
	FloorplanMgr::swap(_blockList[idxA], _blockList[idxB]);
	//reportBTree();
	return {idxA, idxB};
}

void
FloorplanMgr::swap(Block* a, Block* b)
{
	if( b -> getParent() == a ) {
		unsigned side = (a -> getLeft() == b ? 0 : 1);
		Block* bLeft = b -> getLeft();
		Block* bRight = b -> getRight();
		Block* aParent = a -> getParent();							// b -> a
		Block* aChild = (side ? a -> getLeft() : a -> getRight());
		b -> setParent(aParent);									// take care of a's parent
		if( a == _root ) _root = b;
		else if( aParent -> getLeft() == a ) aParent -> setLeft(b);
		else aParent -> setRight(b);
		if( side ) { b -> setLeft(aChild); b -> setRight(a); }		// take care of a's another chid
		else { b -> setRight(aChild); b -> setLeft(a); }
		if( aChild ) aChild -> setParent(b);		a -> setParent(b);
		a -> setLeft(bLeft);		if( bLeft ) bLeft -> setParent(a);			// a -> b
		a -> setRight(bRight);		if( bRight ) bRight -> setParent(a);	
	}
	else if( a -> getParent() == b ) {
		FloorplanMgr::swap(b, a);
	}
	else if( a -> getParent() == b -> getParent() ) {
		Block* p = a -> getParent();
		Block* aLeft = a -> getLeft();
		Block* aRight = a -> getRight();
		Block* bLeft = b -> getLeft();
		Block* bRight = b -> getRight();
		if( p -> getLeft() == a ) { p -> setRight(a); p -> setLeft(b); }
		else { p -> setLeft(a); p -> setRight(b); }
		a -> setLeft(bLeft);		if( bLeft ) bLeft -> setParent(a);
		a -> setRight(bRight);		if( bRight ) bRight -> setParent(a);	
		b -> setLeft(aLeft);		if( aLeft ) aLeft -> setParent(b);
		b -> setRight(aRight);		if( aRight ) aRight -> setParent(b);
	}
	else {
		if( b == _root ) FloorplanMgr::swap(b, a);
		Block* aLeft = a -> getLeft();
		Block* aRight = a -> getRight();
		Block* aParent = a -> getParent();
		Block* bLeft = b -> getLeft();
		Block* bRight = b -> getRight();
		Block* bParent = b -> getParent();
		b -> setParent(aParent);
		if( a == _root ) _root = b;
		else if( aParent -> getLeft() == a ) aParent -> setLeft(b);
		else aParent -> setRight(b);
		b -> setLeft(aLeft);		if( aLeft ) aLeft -> setParent(b);
		b -> setRight(aRight);		if( aRight ) aRight -> setParent(b);
		a -> setParent(bParent);
		if( bParent -> getLeft() == b ) bParent -> setLeft(a);
		else bParent -> setRight(a);
		a -> setLeft(bLeft);		if( bLeft ) bLeft -> setParent(a);
		a -> setRight(bRight);		if( bRight ) bRight -> setParent(a);
	}
}

void
FloorplanMgr::setToEdgeInfo(Block* b, Block* t)
{
/*
	b -> setParent(t -> getParent());
	b -> setLeft(t -> getLeft());
	b -> setRight(t -> getRight());
*/
}

void
FloorplanMgr::maintainEdge(Block* a, Block* b)
{
/*
	Block* t;
	if( t = a -> getParent() ) {
		if( t -> getLeft() == b ) t -> setLeft(a);
		else t -> setRight(a);
	}
	else _root = a;
	if( t = b -> getParent() ) {
		if( t -> getLeft() == a ) t -> setLeft(b);
		else t -> setRight(b);
	}
	else _root = b;
	if( t = a -> getLeft() ) t -> setParent(a);
	if( t = a -> getRight() ) t -> setParent(a);
	if( t = b -> getLeft() ) t -> setParent(b);
	if( t = b -> getRight() ) t -> setParent(b);
*/
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
	_outlineHeight = stoi(tokens[2]);
	getline(file, line);
	tokens = getTokens(line);
	_blockList.reserve(stoi(tokens[1]));
	getline(file, line);					// getting the line of NumTerminals
	//tokens = getTokens(line);
	while( getline(file, line) ) {
		tokens = getTokens(line);
		if( tokens.size() == 3 ) {
			Block* b = new Block(tokens[0], stoi(tokens[1]), stoi(tokens[2]));
			blockMap.insert({tokens[0], b});	
			_blockList.push_back(b);
		}
		else if( tokens.size() == 4 ) { 
			Block* b = new Block(tokens[0]); 
			b -> setX(stoi(tokens[2])); 
			b -> setY(stoi(tokens[3])); 
			blockMap.insert({tokens[0], b});
			// should be removed
		}
	}
	file.close();
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
				line = line.substr(0, line.find_first_of('\r'));
				net.push_back(blockMap.at(line));
			}
			_netList.push_back(net);
		}
	}
	file.close();
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
FloorplanMgr::reset()
{
	for( size_t i = 0; i < _blockList.size(); ++i )
		_blockList[i] -> setToOrigin();
	_Hcontour.clear();
	_Vcontour.clear();
	_Hcontour.push_back({{0, INT_MAX}, 0});		// initial horizontal contour
	_Vcontour.push_back({{0, INT_MAX}, 0});
}

//===================================
//		reporting function
//===================================

void
FloorplanMgr::reportBlockList()
{
	cout << "=====================" << endl;
	cout << " reporting BlockList " << endl;
	cout << "=====================" << endl;
	for( size_t i = 0; i < _blockList.size(); ++i) {
		Block* b = _blockList[i];
		cout << b -> getName() << "\tWidth: " << b -> getWidth() << "\tHeight: " << b -> getHeight() << endl;
	}
	cout << endl;
}

void
FloorplanMgr::reportNetList()
{
	cout << "===================" << endl;
	cout << " reporting NetList " << endl;
	cout << "===================" << endl;
	for( size_t i = 0; i < _netList.size(); ++i) {
		vector<Block*> net = _netList[i];
		cout << "NET:" << i << endl;
		for( size_t j = 0; j < net.size(); ++j) {
			//cout << " " << "name: " << net[j] -> getName() << ", x: " << net[j] -> getX() << ", y: " << net[j] -> getY() << endl;
			cout << " " << "name: " << net[j] -> getName() << ", w: " << net[j] -> getWidth() << ", h: " << net[j] -> getHeight() << endl;
		}
		cout << endl;
	}
	cout << endl;
}

void
FloorplanMgr::reportBTree()
{
	cout << "=================" << endl;
	cout << " reporting BTree " << endl;
	cout << "=================" << endl;
	unsigned count = 0;
	reportBTreeRec(_root, 0, count);	
	cout << endl << "# Node: " << count << endl;
	cout << endl;
}

void
FloorplanMgr::reportBTreeRec(Block* b, unsigned level, unsigned& count)
{
	for( unsigned i = 0; i < level; ++i )
		cout << "   "; 
	cout << "[" << level << "] ";
	if( b == NULL ) { cout << "NULL" << endl; return; }
	++count;
	//cout << b -> getName() << " @ ( " << b -> getX() << ", " << b -> getY() << " ), Width: "
	//	 << b -> getWidth() << ", Height: " << b -> getHeight() << endl;
	cout << b -> getName() << ", p: " << (b -> getParent() ? b -> getParent() -> getName() : "NULL")
		 << ", l: " << (b -> getLeft() ? b -> getLeft() -> getName() : "NULL") << ", r: " << (b -> getRight() ? b -> getRight() -> getName() : "NULL") << endl;
	reportBTreeRec(b -> getLeft(), level + 1, count);
	reportBTreeRec(b -> getRight(), level + 1, count);
}

void
FloorplanMgr::reportHcontour()
{
	cout << "====================" << endl;
	cout << " reporting Hcontour " << endl;
	cout << "====================" << endl;
	for( auto it = _Hcontour.begin(); it != _Hcontour.end(); ++it ) {
		Xrange xrange = it -> first;
		cout << xrange.first << " ~ " << xrange.second << ": " << it -> second << endl;
	}
	cout << endl;
}

void
FloorplanMgr::reportVcontour()
{
	cout << "====================" << endl;
	cout << " reporting Vcontour " << endl;
	cout << "====================" << endl;
	for( auto it = _Vcontour.begin(); it != _Vcontour.end(); ++it ) {
		Yrange yrange = it -> first;
		cout << yrange.first << " ~ " << yrange.second << ": " << it -> second << endl;
	}
	cout << endl;
}

void
FloorplanMgr::test()
{
	bool legal;
	BTreeInit();
	BTreePacking();
	//reportBTree();
	//reportNetList();
	for( int i = 0; i < 1000; ++i ) {
	BlockSwap();
	BTreePacking();
	//reportBTree();
	reportHcontour();
	reportVcontour();
	cout << "maxX: " << BTreeGetMaxX() << ", outlineW: " << _outlineWidth << endl;
	cout << "maxY: " << BTreeGetMaxY() << ", outlineH: " << _outlineHeight << endl;
	cout << "fit: " << isFit() << endl;
	}
	//reportNetList();
}
