// Filename [NetCell.cpp]

#include <iostream>

#include "NetCell.h"

using namespace std;

unsigned Cell::_globalRef = 0;

void
NetArray::report()
{
	//cout << "size: " << _CellsInNet.size() << endl;
	for( auto it = _CellsInNet.begin(); it != _CellsInNet.end(); ++it ) {
		cout << "Cells in Net " << it -> first << " : ";
		for( auto itV = it -> second -> begin(); itV != it -> second -> end(); ++itV ) {
			Cell* c = *itV;
			cout << c -> getName() << " ";
		}
		cout << endl;
	}

}

void
CellArray::report()
{
	//cout << "size: " << _NetsInCell.size() << endl;
	for( auto it = _NetsInCell.begin(); it != _NetsInCell.end(); ++it ) {
		cout << "Nets in Cell  " << it -> first << " : ";
		for( auto itV = it -> second -> begin(); itV != it -> second -> end(); ++itV ) {
			Net* n = *itV;
			cout << n -> getName() << " ";
		}
		cout << endl;
	}
}
