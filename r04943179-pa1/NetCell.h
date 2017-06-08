// Filename [ NetCell.h ]

#ifndef NETCELL_H
#define NETCELL_H

#include <vector>
#include <unordered_map>
#include <string>
#include <list>


using namespace std;

class Net
{
	public:
		Net() { _sizeInA = _sizeInB = 0; }
		Net(string n) { _name = n; _sizeInA = _sizeInB = 0; }
		~Net() {}

		const string& getName() { return _name; }
		unsigned getSizeInA() { return _sizeInA; }
		unsigned getSizeInB() { return _sizeInB; }
		void increaseSizeInA() { ++_sizeInA; }
		void decreaseSizeInA() { --_sizeInA; }
		void increaseSizeInB() { ++_sizeInB; }
		void decreaseSizeInB() { --_sizeInB; }
	private:
		string 				_name;
		unsigned			_sizeInA;
		unsigned			_sizeInB;
};

class Cell
{
	public:
		Cell() { _gain = 0; }
		Cell(string n) { _name = n; _gain = 0; }
		~Cell() {}

		const string& getName() { return _name; }
		void setInA(bool b) { _inA = b; }
		bool isInA() { return _inA; }
		void increaseGain() { ++_gain; }
		void decreaseGain() { --_gain; }
		int getGain() { return _gain; }
		void resetGain() { _gain = 0; }
		static void setGlobalRef() { ++_globalRef; }
		void setToGlobalRef() { _ref = _globalRef; }
		bool isGlobalRef() { return _ref == _globalRef; }
		void setIt(list< Cell* >::iterator it) { _it = it; }
		list< Cell* >::iterator getIt() { return _it; }
		void setList(list< Cell* >* l) { _list = l; }
		list< Cell* >* getList() { return _list; }

	private:
		string				_name;
		bool				_inA;
		int					_gain;
		static unsigned		_globalRef;
		unsigned			_ref;
		list< Cell* >::iterator		_it;
		list< Cell* >*				_list;
};

typedef vector< Cell* > CELL;
typedef vector< Net* > NET;

class NetArray
{
	public:
		void insertvCell(const string& s, CELL* c) { _CellsInNet.insert({s, c}); }
		void report();
	 	unordered_map< string, CELL* >& getRef() { return _CellsInNet; }
		CELL* getvCell(const string& s) { return _CellsInNet.at(s); }

	private:
		unordered_map< string, CELL* >	_CellsInNet;
};

class CellArray
{
	public:
		CellArray() {}
		~CellArray() {}
		void insertvNet(const string& s, NET* n) { _NetsInCell.insert({s, n}); }
		void report();
		unordered_map< string, NET* >& getRef() { return _NetsInCell; }
		NET* getvNet(const string& s) { return _NetsInCell.at(s); }
		
	private:
		unordered_map< string, NET* >	_NetsInCell;
};


#endif
