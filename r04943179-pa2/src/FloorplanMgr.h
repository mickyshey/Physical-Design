// File name [ FloorplanMgr.h ]

#ifndef FLOORPLAN_H
#define FLOORPLAN_H

#include <iostream>
#include <vector>
#include <unordered_map>
#include <list>
#include <ctime>

#include "Block.h"

using namespace std;

typedef pair<unsigned, unsigned> Xrange;
typedef pair<unsigned, unsigned> Yrange;

class FloorplanMgr
{
public:
	FloorplanMgr() {}
	FloorplanMgr(double a): _alpha(a) {} 
	~FloorplanMgr() {}

	void readInput(string s1, string s2);
    void BTreeInit();
	void computeAvg();
	void BTreePacking();
	void simAnnealing();
	double getCost();
	bool isFit();
	void writeOutput(string s);
	void writeLog(string s);
	double getTime() { return (double)(clock() - _start) / CLOCKS_PER_SEC; }
	void test();

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
	unsigned BTreeGetMaxX();
	unsigned BTreeGetMaxY();
	unsigned BTreeGetWireLength();
	unsigned getNetLength(vector<Block*>);
	void deleteAndInsert(Block* d, Block* i);
	void deleteBlock(Block* b);
	void deleteBlock(Block* d, Block* i);			// dealing with special case
	void BlockMoveUp(Block* b, unsigned upSide);
	void swap(Block* a, Block* b);
	void swap(unsigned a, unsigned b) { FloorplanMgr::swap(_blockList[a], _blockList[b]); }
	void setToEdgeInfo(Block* b, Block* t);
	void maintainEdge(Block* a, Block* b);
	void reset();
	Block* BTreeDuplicate();
	void BTreeDuplicateRec(Block* t, Block* d);
	void BTreeFree(Block* b);
	void BTreeExchange(Block* newRoot);
	void setBlockMap(Block* r, unordered_map<string, Block*>& m);
	void undoRotate(unsigned idx) { _blockList[idx] -> rotate(); BTreePacking(); } 
	void undoDeleteAndInsert(Block* root) { BTreeExchange(root); BTreePacking(); }
	void undoSwap(pair<unsigned, unsigned> swapIdx) { FloorplanMgr::swap(swapIdx.second, swapIdx.first); BTreePacking(); }

	double							_alpha;
	unsigned						_outlineWidth;
	unsigned						_outlineHeight;
	double							_avgArea;
	double							_avgWireLength;
	vector<Block*>					_blockList;
	vector<vector<Block*>> 			_netList;
	Block*							_root;			// root of B* tree
	list<pair<Xrange, unsigned>>	_Hcontour;		// < x range, y value >
	list<pair<Yrange, unsigned>>	_Vcontour;		// < y range, x value >
	clock_t							_start;
};

#endif
