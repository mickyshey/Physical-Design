// File name [ FloorplanMgr.cpp ]

#include <iostream>
#include <fstream>
#include <string>
#include <queue>
#include <climits>
#include <cstdlib>

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
	//reportBTree();
}

void
FloorplanMgr::BTreePacking()
{
	FloorplanMgr::reset();
	updateContour(_root);
	BTreePackingLeftRec(_root -> getLeft());
	BTreePackingRightRec(_root -> getRight());

	cout << "Area of this B* tree: " << BTreeGetArea() << endl;
	cout << "Wire length of this B* tree: " << BTreeGetWireLength() << endl;
}

void
FloorplanMgr::updateHcontour(Block* b)
{
	unsigned xLeft = b -> getX();
	unsigned xRight = b -> getRightX();
	unsigned yValue = b -> getUpY();
	//cout << b -> getName() << ", range: " << xLeft << " ~ " << xRight << ", with y: " << yValue << endl;
	for( auto it = _Hcontour.begin(); it != _Hcontour.end(); ++it ) {
		Xrange xrange = it -> first;
		//cout << "checking: " << xrange.first << " ~ " << xrange.second << endl;
		unsigned oriValue = it -> second;
		if( xLeft >= xrange.second ) continue;
		if( xRight <= xrange.first ) return;
		if( xLeft >= xrange.first && xLeft < xrange.second && xRight > xrange.first && xRight <= xrange.second ) {				
			Xrange tmp;
			if( xrange.first != xLeft && xrange.second != xRight ) {	
					// target: 55 ~ 65, range: 50 ~ 70
				tmp = {xrange.first, xLeft};
				_Hcontour.insert(it, {tmp, oriValue});		// 50 ~ 55: ori
				tmp = {xLeft, xRight};
				_Hcontour.insert(it, {tmp, yValue});		// 55 ~ 65: new
				(it -> first).first = xRight;				// 65 ~ 70: ori	
			}
			else if( xRight != xrange.second ) {
					// target: 55 ~ 65, range: 55 ~ 70
				tmp = {xLeft, xRight};
				_Hcontour.insert(it, {tmp, yValue});		// 55 ~ 65: new
				(it -> first).first = xRight;				// 65 ~ 70: ori
			}
			else {
					// target: 55 ~ 65, range: 50 ~ 65
				if( xrange.first != xLeft) {
					tmp = {xrange.first, xLeft};
					_Hcontour.insert(it, {tmp, oriValue});	// 50 ~ 55: ori
				}
				(it -> first).first = xLeft;				// 55 ~ 65: new
				it -> second = yValue;
			}
			return;
		}
		else if( xLeft >= xrange.first && xLeft < xrange.second ) {
			if( xLeft == xrange.first ) {
					// target: 55 ~ 65, range: 55 ~ 60
				it -> second = yValue;
			}
			else {
					// target: 55 ~ 65, range: 53 ~ 60
				Xrange tmp = {xrange.first, xLeft};
				_Hcontour.insert(it, {tmp, oriValue});		// 53 ~ 55: ori
				(it -> first).first = xLeft;				// 55 ~ 60: new
				it -> second = yValue;
			}
		}
		else {
			if( xRight == xrange.second ) {
					// target: 55 ~ 65, range: 65 ~ 70
			}
			else {
					// target: 55 ~ 65, range: 63 ~ 70
				Xrange tmp = {xrange.first, xRight};
				_Hcontour.insert(it, {tmp, yValue});		// 63 ~ 65: new
				(it -> first).first = xRight;				// 65 ~ 70: ori
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
	//cout << b -> getName() << ", range: " << yDown << " ~ " << yUp << ", with y: " << xValue << endl;
	for( auto it = _Vcontour.begin(); it != _Vcontour.end(); ++it ) {
		Yrange yrange = it -> first;
		//cout << "checking: " << yrange.first << " ~ " << yrange.second << endl;
		unsigned oriValue = it -> second;
		if( yDown >= yrange.second ) continue;
		if( yUp <= yrange.first ) return;
		if( yDown >= yrange.first && yDown < yrange.second && yUp > yrange.first && yUp <= yrange.second ) {				
			Yrange tmp;
			if( yrange.first != yDown && yrange.second != yUp ) {	
					// target: 55 ~ 65, range: 50 ~ 70
				tmp = {yrange.first, yDown};
				_Vcontour.insert(it, {tmp, oriValue});		// 50 ~ 55: ori
				tmp = {yDown, yUp};
				_Vcontour.insert(it, {tmp, xValue});		// 55 ~ 65: new
				(it -> first).first = yUp;					// 65 ~ 70: ori	
			}
			else if( yUp != yrange.second ) {
					// target: 55 ~ 65, range: 55 ~ 70
				tmp = {yDown, yUp};
				_Vcontour.insert(it, {tmp, xValue});		// 55 ~ 65: new
				(it -> first).first = yUp;					// 65 ~ 70: ori
			}
			else {
					// target: 55 ~ 65, range: 50 ~ 65
				if( yrange.first != yDown ) {
					tmp = {yrange.first, yDown};
					_Vcontour.insert(it, {tmp, oriValue});	// 50 ~ 55: ori
				}
				(it -> first).first = yDown;				// 55 ~ 65: new
				it -> second = xValue;
			}
			return;
		}
		else if( yDown >= yrange.first && yDown < yrange.second ) {
			if( yDown == yrange.first ) {
					// target: 55 ~ 65, range: 55 ~ 60
				it -> second = xValue;
			}
			else {
					// target: 55 ~ 65, range: 53 ~ 60
				Yrange tmp = {yrange.first, yDown};
				_Vcontour.insert(it, {tmp, oriValue});		// 53 ~ 55: ori
				(it -> first).first = yDown;				// 55 ~ 60: new
				it -> second = xValue;
			}
		}
		else {
			if( yUp == yrange.second ) {
					// target: 55 ~ 65, range: 65 ~ 70
			}
			else {
					// target: 55 ~ 65, range: 63 ~ 70
				Yrange tmp = {yrange.first, yUp};
				_Vcontour.insert(it, {tmp, xValue});		// 63 ~ 65: new
				(it -> first).first = yUp;					// 65 ~ 70: ori
			}
		}
	}
}

void
FloorplanMgr::BTreePackingLeftRec(Block* b)
{
	if( b == NULL ) return;
	Block* parent = b -> getParent();
	unsigned max = VcontourFindMaxX(b);
	b -> setX(max);
	b -> setY(parent -> getY());
	updateContour(b);

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
	b -> setY(max);
	updateContour(b);

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

unsigned
FloorplanMgr::VcontourFindMaxX(Block* b)
{
	unsigned max = 0;
	unsigned yDown = b -> getY();
	unsigned yUp = b -> getUpY();
	for( auto it = _Vcontour.begin(); it != _Vcontour.end(); ++it ) {
		Yrange yrange = it -> first;
		unsigned x = it -> second;
		if( yDown >= yrange.second ) continue;
		if( yUp <= yrange.first ) return max;
		if( x > max ) max = x;
	}
}

unsigned
FloorplanMgr::BTreeGetArea()
{
	unsigned maxX = 0, maxY = 0; 
	for( auto it = _Vcontour.begin(); it != _Vcontour.end(); ++it ) {
		unsigned x = it -> second;
		if( x > maxX ) maxX = x;
	}
	for( auto it = _Hcontour.begin(); it != _Hcontour.end(); ++it ) {
		unsigned y = it -> second;
		if( y > maxY ) maxY = y;
	}
	return maxX * maxY;
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
	//cout << "X: " << minX << " ~ " << maxX << ", Y: " << minY << " ~ " << maxY << endl;
	return (maxX - minX) + (maxY - minY);
}

void
FloorplanMgr::BlockRotate()
{
	unsigned idx = rand() % _blockList.size();
	cout << "rotating " << _blockList[idx] -> getName() << endl;
	_blockList[idx] -> rotate();
}

void
FloorplanMgr::BlockDeleteAndInsert()
{
	unsigned idxD = rand() % _blockList.size();
	unsigned idxI;
	while( 1 ) { 
		idxI = rand() % _blockList.size();
		cout << "checing: " << _blockList[idxI] -> getName() << endl;
		if( idxI == idxD ) continue;
		if( _blockList[idxD] -> getParent() == _blockList[idxI] && _blockList[idxI] -> isFull() ) continue;
		if( !(_blockList[idxI] -> isFull()) ) break;	
		else cout << _blockList[idxI] -> getName() << " is full" << endl;
	}
	cout << "deleting " << _blockList[idxD] -> getName() << endl;
	cout << "inserting to " << _blockList[idxI] -> getName() << endl;
	deleteAndInsert(_blockList[idxD], _blockList[idxI]);
}

void
FloorplanMgr::deleteAndInsert(Block* d, Block* i)
{
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
		if( b == _root ) _root = (side == 0 ? b -> getLeft() : b -> getRight());
		Block* movingNode = (side == 0 ? b -> getLeft() : b -> getRight());
		Block* stayingNode = (side == 0 ? b -> getRight() : b -> getLeft());
		movingNode -> setParent(p);
		stayingNode -> setParent(movingNode);
		BlockMoveUp(movingNode);
		if( side == 0 ) movingNode -> setRight(stayingNode);
		else movingNode -> setLeft(stayingNode);
	}
	else {									// node with one child
		Block* c = (b -> getLeft() == NULL ? b -> getRight() : b -> getLeft());
		c -> setParent(p);
		if( p -> getLeft() == b ) p -> setLeft(c);
		else p -> setRight(c);
	}
	b -> setToLeaf();
}

void
FloorplanMgr::deleteBlock(Block* d, Block* i)
{
// i is a child of d
	Block* p = d -> getParent();
	unsigned side = (d -> getLeft() == i ? 0 : 1);
	Block* t = (side == 0 ? d -> getRight() : d -> getLeft());
	if( d == _root ) _root = t;
	t -> setParent(p);
	i -> setParent(t);
	BlockMoveUp(t);	
	if( side == 0 ) t -> setLeft(i);
	else t -> setRight(i);
	d -> setToLeaf();
}

void
FloorplanMgr::BlockMoveUp(Block* b)
{
	if( b -> isFull() ) {
		unsigned side = rand() % 2;
		Block* movingNode = (side == 0 ? b -> getLeft() : b -> getRight());
		Block* stayingNode = (side == 0 ? b -> getRight() : b -> getLeft());
		stayingNode -> setParent(movingNode);
		BlockMoveUp(movingNode);
		if( side == 0 ) movingNode -> setRight(stayingNode);
		else movingNode -> setLeft(stayingNode);
	}
}

void
FloorplanMgr::BlockSwap()
{
	unsigned idxA = rand() % _blockList.size();
	unsigned idxB;
	while( 1 ) {
		idxB = rand() % _blockList.size();
		if( idxA != idxB ) break;
	}
	cout << "swapping " << _blockList[idxA] -> getName() << " and " << _blockList[idxB] -> getName() << endl;
	FloorplanMgr::swap(_blockList[idxA], _blockList[idxB]);
}

void
FloorplanMgr::swap(Block* a, Block* b)
{
	Block* tmp = new Block;
	tmp -> setEqual(a);				// change the data member except edges
	a -> setEqual(b);
	b -> setEqual(tmp);
	delete tmp;
	/*Block* tmp = new Block(a);
	if( a -> getParent() == b ) {}
	else if( b -> getParent() == a ) {}
	else {
	setToEdgeInfo(a, b);			// maintain edge info "from" the node
	setToEdgeInfo(b, tmp);
	}	
	maintainEdge(a, b);				// maintain edge info "to" the node
	delete tmp;
	*/
}

void
FloorplanMgr::setToEdgeInfo(Block* b, Block* t)
{
	b -> setParent(t -> getParent());
	b -> setLeft(t -> getLeft());
	b -> setRight(t -> getRight());
}

void
FloorplanMgr::maintainEdge(Block* a, Block* b)
{
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
	getline(file, line);
	tokens = getTokens(line);
	// still don't know what to do with # terminals
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
		cout << "NET:";
		for( size_t j = 0; j < net.size(); ++j) {
			cout << " " << net[j] -> getName();
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
	reportBTreeRec(_root, 0);	
	cout << endl;
}

void
FloorplanMgr::reportBTreeRec(Block* b, unsigned level)
{
	for( unsigned i = 0; i < level; ++i )
		cout << "   "; 
	cout << "[" << level << "] ";
	if( b == NULL ) { cout << "NULL" << endl; return; }
	cout << b -> getName() << " @ ( " << b -> getX() << ", " << b -> getY() << " ), Width: "
		 << b -> getWidth() << ", Height: " << b -> getHeight() << endl;
	reportBTreeRec(b -> getLeft(), level + 1);
	reportBTreeRec(b -> getRight(), level + 1);
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
