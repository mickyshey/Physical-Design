// File name [ floorplan.h ]

#ifndef FLOORPLAN_H
#define FLOORPLAN_H

#include <iostream>
#include <vector>
#include <unordered_map>
#include <list>

#include "Block.h"

using namespace std;

typedef pair<unsigned, unsigned> Xrange;
typedef pair<unsigned, unsigned> Yrange;

class FloorplanMgr
{
public:
	FloorplanMgr() {}
	FloorplanMgr(double r): _ratio(r) {} 
	~FloorplanMgr() {}

	void readInput(string s1, string s2);
	double getRatio() { return _ratio; }
    void BTreeInit();
	void BTreePacking();

//		Perturbation function
	void BlockRotate();
	void BlockDeleteAndInsert();
	void BlockSwap();

//		Reporting function
	void reportBlockList(); 
	void reportNetList();
	void reportBTree();
	void reportHcontour();
	void reportVcontour();

private:
	void readInputBlock(string s, unordered_map<string, Block*>& blockMap);
	void readInputNet(string s, unordered_map<string, Block*>& blockMap);
	void reportBTreeRec(Block* b, unsigned level);
	void BTree_insert(Block* target, Block* parent);
	void BTree_insertLeft(Block* target, Block* parent);
	void BTree_insertRight(Block* target, Block* parent);
	void updateContour(Block* b) { updateHcontour(b); updateVcontour(b); }
	void updateHcontour(Block* b);
	void updateVcontour(Block* b);
	void BTreePackingLeftRec(Block* b);
	void BTreePackingRightRec(Block* b);
	unsigned HcontourFindMaxY(Block* b);
	unsigned VcontourFindMaxX(Block* b);
	unsigned BTreeGetArea();
	unsigned BTreeGetWireLength();
	unsigned getNetLength(vector<Block*>);
	void deleteAndInsert(Block* d, Block* i);
	void deleteBlock(Block* b);
	void deleteBlock(Block* d, Block* i);			// dealing with special case
	void BlockMoveUp(Block* b);
	void swap(Block* a, Block* b);
	void setToEdgeInfo(Block* b, Block* t);
	void maintainEdge(Block* a, Block* b);
	void reset();

	double							_ratio;
	unsigned						_outlineWidth;
	unsigned						_outlineHeight;
	vector<Block*>					_blockList;
	vector<vector<Block*>> 			_netList;
	Block*							_root;			// root of B* tree
	list<pair<Xrange, unsigned>>	_Hcontour;		// < x range, y value >
	list<pair<Yrange, unsigned>>	_Vcontour;		// < y range, x value >
};

#endif
