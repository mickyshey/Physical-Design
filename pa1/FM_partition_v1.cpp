// Filename [FM_partition.cpp]

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <cassert>
#include <unordered_map>
#include <unordered_set>

#include "FM_partition.h"
#include "NetCell.h"

using namespace std;

void
FM_partition::readFile(const string& f)
{
	NetArray* nArray = new NetArray;

	ifstream file;
	file.open(f);
	string s;
	getline( file, s );										// get balancefactor
	setBalFactor( stod(s) ); 
	while( getline( file, s) ) {							// NET n1        c1 c2 c3 ;
		//cout << s << endl;
		if( s.substr(0, 3) == "NET" ) {
			size_t left = 4;									// starting from 'n'
			size_t right = s.find_first_of(' ', left);
			string word = s.substr(left, right - left);			// n1
			Net* n = new Net(word);
			CELL* vCell = new vector< Cell* >;
			while(1) {
				left = s.find_first_not_of(' ', right + 1);
				right = s.find_first_of(' ', left);
				if( right == string::npos ) word = s.substr(left);
				else word = s.substr(left, right - left);			// c1, c2, c3
				if ( _cHash.find(word) == _cHash.end() ) {
					Cell* c = new Cell(word);
					_cHash.insert({word, c});
					vCell -> push_back(c);
				}
				else {
					vCell -> push_back(_cHash.at(word));
				}
				if( right == string::npos ) {
					//cout << "not the end ..." << endl;
					while( continueSetUp(vCell, file) ) {} 
					nArray -> insertvCell(n -> getName(), vCell);
					_nHash.insert({n -> getName(), n});
					break;
				}
				else if ( s[right + 1] == ';' ) {
					nArray -> insertvCell(n -> getName(), vCell);
					_nHash.insert({n -> getName(), n});
					break;
				}
			}
		}
	}
	setnArray(nArray);
	CellArray* cArray = convertNets2Cells();
	setcArray(cArray);
}

void
FM_partition::writeFile(const string& s)
{
	ofstream file;
	file.open(s);
	unsigned cutSize = computeCutSize();
	file << "Cutsize = " << cutSize << endl;
	file << "G1 " << _grpA.size() << endl;
	for( auto it = _grpA.begin(); it != _grpA.end(); ++it )
		file << it -> second -> getName() << " ";
	file << ';' << endl;
	file << "G2 " << _grpB.size() << endl;
	for( auto it = _grpB.begin(); it != _grpB.end(); ++it )
		file << it -> second -> getName() << " ";
	file << ';' << endl;
}

void
FM_partition::initGrp()
{
	unsigned max = _cHash.size();
	unsigned i = 0;
	for( auto it = _cHash.begin(); it != _cHash.end(); ++it ) {
		Cell* c = it -> second;
		if( i < max/2 ) {
			c -> setInA(true);
			_grpA.insert({it -> first, it -> second});
			NET* vNet = _cells -> getvNet(it -> first);
			for( auto itV = vNet -> begin(); itV != vNet -> end(); ++itV ) {
				Net* n = *itV;
				n -> increaseSizeInA();
			}
		}
		else {
			c -> setInA(false);
			_grpB.insert({it -> first, it -> second});
			NET* vNet = _cells -> getvNet(it -> first);
			for( auto itV = vNet -> begin(); itV != vNet -> end(); ++itV ) {
				Net* n = *itV;
				n -> increaseSizeInB();
			}
		}
		++i;
	}
	//reportGrpA();
	//reportGrpB();
	//reportNetSize();
}

void
FM_partition::computeCellGain()
{
	//cout << "computing cell gain ..." << endl;
	for( auto it = _cHash.begin(); it != _cHash.end(); ++it ) {
		Cell* c = it -> second;
		//bool inA = (_grpA.find(c -> getName()) != _grpA.end());
		bool inA = c -> isInA();
		NET* vNet = _cells -> getvNet(it -> first);
		for( auto itV = vNet -> begin(); itV != vNet -> end(); ++itV ) {
			Net* n = *itV;
			unsigned numInF = (inA ? n -> getSizeInA() : n -> getSizeInB());
			unsigned numInT = (inA ? n -> getSizeInB() : n -> getSizeInA());
			if( numInF == 1 )
				c -> increaseGain();
			else if( numInT == 0 )
				c -> decreaseGain();
		}
		if( _gainBucket.find(c -> getGain()) == _gainBucket.end() ) {		// create a list
			list< Cell* >* lC = new list< Cell* >;
			lC -> push_front(c);
			c -> setList(lC);
			c -> setIt(lC -> begin());
			_gainBucket.insert({c -> getGain(), lC});
		}
		else {
			list< Cell* >* lC = _gainBucket.at(c -> getGain());
			lC -> push_front(c);
			c -> setList(lC);
			c -> setIt(lC -> begin());
		}
	}
	//reportGain();
	//reportBucket();
}

Cell* 
FM_partition::getMaxGainCell()
{
	for( auto it = _gainBucket.rbegin(); it != _gainBucket.rend(); ++it ) {
		list< Cell* >* l = it -> second;
		for( auto itL = l -> begin(); itL != l -> end(); ++itL ) {
			Cell* c = *itL;
			if( isBalanced(c) ) {
				// diff from ori
				l -> erase(itL);	
				if( l -> begin() == l -> end() ) {
					delete l;
					_gainBucket.erase(--(it.base()));
				}
				///////////////
				return c;
			}
		}
	}
	return NULL;
}

void
FM_partition::updateCellGain(Cell* c)
{
	//cout << "moving cell " << c -> getName() << " and update neighbors' gains ..." << endl;
	c -> setToGlobalRef();
	//bool inA = (_grpA.find(c -> getName()) != _grpA.end());
	bool inA = c -> isInA();
	NET* vNet = _cells -> getvNet(c -> getName());	
	for( auto itV = vNet -> begin(); itV != vNet -> end(); ++itV ) {
		Net* n = *itV;
		unsigned numInF = (inA ? n -> getSizeInA() : n -> getSizeInB());
		unsigned numInT = (inA ? n -> getSizeInB() : n -> getSizeInA());
		if( numInT == 0 ) increaseGain4AllFreeCells(n);					// should check criticality first
		else if( numInT == 1 ) decreaseGain4OnlyTCell(n, inA);		// 'only' function could be slow
		--numInF; ++numInT;
		if( numInF == 0 ) decreaseGain4AllFreeCells(n); 
		else if( numInF == 1 ) increaseGain4OnlyFCell(c, n, inA);

		if( inA ) { n -> decreaseSizeInA(); n -> increaseSizeInB(); }	// update netSize
		else { n -> decreaseSizeInB(); n -> increaseSizeInA(); }
	}
	exchangeCell(c);
	//removeCellAndUpdateBucket(c);		// update bucket could be slow
	//reportGain();
	//reportBucket();
}

void
FM_partition::exchangeCell(Cell* c)
{
	//bool inA = (_grpA.find(c -> getName()) != _grpA.end());
	bool inA = c -> isInA();
	if( inA ) {
		_grpA.erase(_grpA.find(c -> getName()));
		_grpB.insert({c -> getName(), c});	
		c -> setInA(false);
	}
	else {
		_grpB.erase(_grpB.find(c -> getName()));
		_grpA.insert({c -> getName(), c});
		c -> setInA(true);
	}
}

void
FM_partition::exchangeCellAndUpdateNetSize(Cell* c)
{
	bool inA = c -> isInA();
	if( inA ) {
		_grpA.erase(_grpA.find(c -> getName()));
		_grpB.insert({c -> getName(), c});	
		c -> setInA(false);
	}
	else {
		_grpB.erase(_grpB.find(c -> getName()));
		_grpA.insert({c -> getName(), c});
		c -> setInA(true);
	}
	NET* vNet = _cells -> getvNet(c -> getName());
	for( auto it = vNet -> begin(); it != vNet -> end(); ++it ) {
		Net* n = *it;
		if( inA ) {
			n -> decreaseSizeInA();
			n -> increaseSizeInB();
		}
		else {
			n -> decreaseSizeInB();
			n -> increaseSizeInA();
		}
	}
}

void
FM_partition::reset()
{
	for( auto it = _cHash.begin(); it != _cHash.end(); ++it ) {
		Cell* c = it -> second;
		c -> resetGain();
	}
	for( auto it = _gainBucket.begin(); it != _gainBucket.end(); ++it ) {
		list< Cell* >* l = it -> second;
		delete l;
	}
	_gainBucket.clear();
}

//============================
//	private member functions
//============================

bool
FM_partition::continueSetUp(CELL* vCell, ifstream& file)
{
	string s;
	getline(file, s);
	size_t left = s.find_first_not_of(' ');
	if( s[left] == ';' ) return false;
	size_t right;
	while(1) {
		string word;
		right = s.find_first_of(' ', left);
		if( right == string::npos ) word = s.substr(left);
		else word = s.substr(left, right - left);
		if ( _cHash.find(word) == _cHash.end() ) {
			Cell* c = new Cell(word);
			_cHash.insert({word, c});
			vCell -> push_back(c);
		}
		else {
			vCell -> push_back(_cHash.at(word));
		}
		if( right == string::npos )	return true;
		else if( s[right + 1] == ';' ) return false;
		left = right + 1;
	}
}

CellArray*
FM_partition::convertNets2Cells()
{
	unordered_map < string, CELL* > nets = _nets -> getRef();
	unordered_map < string, NET* > cells;
	for( auto it = nets.begin(); it != nets.end(); ++it ) {
		string netName = it -> first;
		CELL* vC = it -> second;
		for( auto itV = vC -> begin(); itV != vC -> end(); ++itV ) {
			Cell* c = *itV;
			if( cells.find(c -> getName()) == cells.end() ) {		// create a vector for it
				NET* vN = new vector< Net* >;
				vN -> push_back(_nHash.at(netName));
				assert(vN -> size() == 1);
				cells.insert({c -> getName(), vN});
			}
			else {
				cells.at(c -> getName()) -> push_back(_nHash.at(netName));
			}
		}
	}
	
	CellArray* cArray = new CellArray;
	for (auto it = cells.begin(); it != cells.end(); ++it) {
		cArray->insertvNet(it -> first, it -> second);
	}
	return cArray;
}

void
FM_partition::increaseGain4AllFreeCells(Net* n)
{
	CELL* vCell = _nets -> getvCell(n -> getName());
	for( auto itV = vCell -> begin(); itV != vCell -> end(); ++itV ) {
		Cell* c = *itV;
		if( !(c -> isGlobalRef()) ) { 
			int oldGain = c -> getGain();
			c -> increaseGain();
			updateGainBucket(c, oldGain);	
		}
	}
}

void
FM_partition::decreaseGain4AllFreeCells(Net* n)
{
	CELL* vCell = _nets -> getvCell(n -> getName());
	for( auto itV = vCell -> begin(); itV != vCell -> end(); ++itV ) {
		Cell* c = *itV;
		if( !(c -> isGlobalRef()) ) {
			int oldGain = c -> getGain();
			c -> decreaseGain();
			updateGainBucket(c, oldGain);
		}
	}
}

void 
FM_partition::decreaseGain4OnlyTCell(Net* n, bool inA)
{
	if( inA ) {
		CELL* vCell = _nets -> getvCell(n -> getName());
		for( auto itV = vCell -> begin(); itV != vCell -> end(); ++itV ) {
			Cell* c = *itV;
			if( _grpB.find(c -> getName()) != _grpB.end() ) {
				if( !(c -> isGlobalRef()) ) {
					int oldGain = c -> getGain();
					c -> decreaseGain();
					updateGainBucket(c, oldGain);
				}
				return;	
			}	
		}	
	}
	else {
		CELL* vCell = _nets -> getvCell(n -> getName());
		for( auto itV = vCell -> begin(); itV != vCell -> end(); ++itV ) {
			Cell* c = *itV;
			if( _grpA.find(c -> getName()) != _grpA.end() ) {
				if( !(c -> isGlobalRef()) ) {
					int oldGain = c -> getGain();
					c -> decreaseGain();
					updateGainBucket(c, oldGain);
				}
				return;
			}	
		}	
	}
}

void 
FM_partition::increaseGain4OnlyFCell(Cell* c, Net* n, bool inA)
{
	//unsigned count = 0;
	if( inA ) {
		CELL* vCell = _nets -> getvCell(n -> getName());
		for( auto itV = vCell -> begin(); itV != vCell -> end(); ++itV ) {
			Cell* cc = *itV;
			if( _grpA.find(cc -> getName()) != _grpA.end() ) {
				//++count;
				//if( cc != c ) {
					if( !(cc -> isGlobalRef()) ) {
						int oldGain = cc -> getGain();
						cc -> increaseGain();
						updateGainBucket(cc, oldGain);
					}
					//if( count == 2 ) return;
					//return;
				//}
			}	
		}	
	}
	else {
		CELL* vCell = _nets -> getvCell(n -> getName());
		for( auto itV = vCell -> begin(); itV != vCell -> end(); ++itV ) {
			Cell* cc = *itV;
			if( _grpB.find(cc -> getName()) != _grpB.end() ) {
				//++count;
				//if( cc != c ) {
					if( !(cc -> isGlobalRef()) ) {
						int oldGain = cc -> getGain();
						cc -> increaseGain();
						updateGainBucket(cc, oldGain);
					}
					//if( count == 2 ) return;
					//return;
				//}
			}	
		}	
	}
}

void
FM_partition::removeCellAndUpdateBucket(Cell* c)
{
//	removeCellFromBucket(c);
/*	map< int, list< Cell* >* > newBucket;
	for( auto it = _gainBucket.begin(); it != _gainBucket.end(); ++it ) {		// update gain
		list< Cell* >* l = it -> second;
		for( auto itL = l -> begin(); itL != l -> end(); ++itL ) {
			Cell* cc = *itL;
			if( newBucket.find(cc -> getGain()) == newBucket.end() ) {			// create a list
				list< Cell* >* lC = new list< Cell* >;
				lC -> push_back(cc);
				newBucket.insert({cc -> getGain(), lC});
			}
			else {
				newBucket.at(cc -> getGain()) -> push_back(cc);
			}
		}
		delete l;
	}	
	_gainBucket.clear();							// should delete list inside first
	for( auto it = newBucket.begin(); it != newBucket.end(); ++it ) {
		_gainBucket.insert({it -> first, it -> second});

	}
*/
}

bool
FM_partition::isBalanced(Cell* c)
{
	//bool inA = (_grpA.find(c -> getName()) != _grpA.end()); 
	bool inA = c -> isInA();
	unsigned sizeA, sizeB;
	if( inA ) {
		sizeA = _grpA.size() - 1;
		sizeB = _grpB.size() + 1;
	}
	else {
		sizeA = _grpB.size() - 1;
		sizeB = _grpA.size() + 1;
	}
	double lowerBound = (1 - _balFactor) * _cHash.size() / 2;
	double upperBound = (1 + _balFactor) * _cHash.size() / 2;
	return (sizeA >= lowerBound) && (sizeA <= upperBound) && (sizeB >= lowerBound) && (sizeB <= upperBound);
}

unsigned
FM_partition::computeCutSize()
{
	unsigned size = 0;
	for( auto it = _nHash.begin(); it != _nHash.end(); ++it ) {
		Net* n = it -> second;
		if( (n -> getSizeInA()) && (n -> getSizeInB()) ) ++size;
	}
	return size;
}

void 
FM_partition::updateGainBucket(Cell* c, int oldGain)
{
	list< Cell* >* l = c -> getList();
	l -> erase(c -> getIt());
	if( l -> begin() == l -> end() ) {
		delete l;
		_gainBucket.erase(_gainBucket.find(oldGain));
	}
	if( _gainBucket.find(c -> getGain()) == _gainBucket.end() ) {
		list< Cell* >* lC = new list< Cell* >;							// create a list
		lC -> push_front(c);
		c -> setList(lC);
		c -> setIt(lC -> begin());
		_gainBucket.insert({c -> getGain(), lC});
	}
	else {
		list< Cell* >* lC = _gainBucket.at(c -> getGain());
		lC -> push_front(c);
		c -> setList(lC);
		c -> setIt(lC -> begin());
	}
/*	for( auto it = _gainBucket.rbegin(); it != _gainBucket.rend(); ++it ) {
		list< Cell* >* l = it -> second;
		for( auto itL = l -> begin(); itL != l -> end(); ++itL ) {
			Cell* cc = *itL;
			if( cc == c ) {
				l -> erase(itL);													// erase from old bucket
				if( l -> begin() == l -> end() ) {									// if list is empty, delete
					delete l;
					_gainBucket.erase(--(it.base()));
				}
				if( _gainBucket.find(c -> getGain()) == _gainBucket.end() ) {		// update to new bucket	
					list< Cell* >* lC = new list< Cell* >;							// create a list
					lC -> push_back(c);
					_gainBucket.insert({c -> getGain(), lC});
				}
				else {
					_gainBucket.at(c -> getGain()) -> push_back(c);
				}
//				for( auto itB = _gainBucket.rbegin(); itB != _gainBucket.rend(); ++itB ) {
//					list< Cell* >* ll = itB -> second;
//					cout << "gain: " << itB -> first << ",";
//					for( auto itLL = ll -> begin(); itLL != ll -> end(); ++itLL ) {
//						Cell* ccc = *itLL;
//						cout << " " << ccc -> getName();
//					}
//					cout << endl;
//				}
//				cout << "done ..." << endl;
//				return;
			}
		}
	}
*/
}

// End of File
