// FileName [ FM_partition.h ]

#ifndef FM_PAR_H
#define FM_PAR_H

#include <vector>
#include <map>
#include <list>


#include "NetCell.h"

using namespace std;

class FM_partition
{
	public:
		FM_partition() {}
		~FM_partition() {}

		void readFile(const string&);
		void writeFile(const string&);
		void report() { _nets -> report(); _cells -> report(); }
		void initGrp();
		void computeCellGain();
		Cell* getMaxGainCell();
		void updateCellGain(Cell*);
		void exchangeCell(Cell*);
		void exchangeCellAndUpdateNetSize(Cell*);
		void reset();
		unsigned computeCutSize();

	private:
		double 				_balFactor;
		NetArray*			_nets;
		CellArray*			_cells;
		//unsigned			_tmpSizeA;
		//unsigned			_tmpSizeB;
		unordered_map< string, Net* >	_nHash;
		unordered_map< string, Cell* > 	_cHash;
		unordered_map< string, Cell* > 	_grpA;
		unordered_map< string, Cell* >	_grpB;
		map< int, list< Cell* >* >		_gainBucket;

		void setBalFactor(double b) { _balFactor = b; }
		const double getBalFactor() const { return _balFactor; }
		bool continueSetUp(CELL*, ifstream&);
		CellArray* convertNets2Cells();
		void setnArray(NetArray* n) { _nets = n; }
		void setcArray(CellArray* c) { _cells = c; }
		void calculateFT(Cell*, Net*, bool, unsigned&, unsigned&);
		void removeCellFromBucket(Cell *);
		void increaseGain4AllFreeCells(Net*);
		void decreaseGain4AllFreeCells(Net*);
		void decreaseGain4OnlyTCell(Net*, bool);
		void increaseGain4OnlyFCell(Cell*, Net*, bool);
		void removeCellAndUpdateBucket(Cell*);
		bool isBalanced(Cell*);
		void updateGainBucket(Cell*, int);
		//void setGrpSize() { _tmpSizeA = _grpA.size(); _tmpSizeB = _grpB.size(); }

		//	Debugging function
		void reportNetArray() { _nets -> report(); }
		void reportCellArray() { _cells -> report(); }
		void reportGrpA() { cout << "GrpA:"; for( auto it = _grpA.begin(); it != _grpA.end(); ++it ) cout << " " << it -> first; cout << endl; }
		void reportGrpB() { cout << "GrpB:"; for( auto it = _grpB.begin(); it != _grpB.end(); ++it ) cout << " " << it -> first; cout << endl; }
		void reportNetSize() { for( auto it = _nHash.begin(); it != _nHash.end(); ++ it ) cout << it -> first << ", sizeInA: " << it -> second -> getSizeInA() << ", sizeInB: " << it -> second -> getSizeInB() << endl; }
		void reportGain() { for( auto it = _cHash.begin(); it != _cHash.end(); ++it ) cout << it -> first << ", gain: " << it -> second -> getGain() << endl; }
		void reportBucket() { for( auto it = _gainBucket.rbegin(); it != _gainBucket.rend(); ++it) cout << "gain: " << it -> first << ", size: " << it -> second -> size() << endl; }
};

#endif
