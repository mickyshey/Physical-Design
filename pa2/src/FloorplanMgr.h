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
	unsigned BlockRotate();						// return the chosen idx in BlockList
	Block* BlockDeleteAndInsert(bool d);		// return the _root of the duplicated BTree
	pair<unsigned, unsigned> BlockSwap();		// return the pair of idx being swapped

//		Reporting function
	void reportBlockList(); 
	void reportNetList();
	void reportBTree();
	void reportHcontour();
	void reportVcontour();

private:
	void readInputBlock(string s, unordered_map<string, Block*>& blockMap);
	void readInputNet(string s, unordered_map<string, Block*>& blockMap);
	void reportBTreeRec(Block* b, unsigned level, unsigned& count);
	void BTree_insert(Block* target, Block* parent);
	void updateContour(Block* b) { updateHcontour(b); updateVcontour(b); }
	void updateHcontour(Block* b);
	void updateVcontour(Block* b);
	void BTreePackingLeftRec(Block* b);
	void BTreePackingRightRec(Block* b);
	unsigned HcontourFindMaxY(Block* b);
	unsigned VcontourFindMaxX(Block* b);
	unsigned BTreeGetArea(bool& legal);
	unsigned BTreeGetWireLength();
	unsigned getNetLength(vector<Block*>);
	void deleteAndInsert(Block* d, Block* i);
	void deleteBlock(Block* b);
	void deleteBlock(Block* d, Block* i);			// dealing with special case
	void BlockMoveUp(Block* b, unsigned upSide);
	void swap(Block* a, Block* b);
	void setToEdgeInfo(Block* b, Block* t);
	void maintainEdge(Block* a, Block* b);
	void reset();
	Block* BTreeDuplicate();
	void BTreeDuplicateRec(Block* t, Block* d);
	void BTreeFree(Block* b);

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
