#include <iostream>
#include <string>
#include <vector>
#include <climits>

#include "FM_partition.h"
#include "NetCell.h"

using namespace std;

FM_partition* FM = new FM_partition; 

int main( int argc, char** argv ) {

	FM -> readFile(argv[1]);
	FM -> initGrp();
	
	unsigned itCount = 0;
	bool sameCount = false;
	while(1) {
		Cell::setGlobalRef();
		vector< pair< int, Cell* > > gainTmp;	
		static int oldCutSize = INT_MAX;

		FM -> computeCellGain();
		int maxPartialSum = 0;
		int currentPartialSum = 0;
		unsigned count = 0;
		size_t idx = 0;
		while(1) {									// each iteration
			Cell* c = FM -> getMaxGainCell();
			if (!c) break;
			gainTmp.push_back({c -> getGain(), c});
			FM -> updateCellGain(c);
		}
		vector< unsigned > vIdx;	
		for( size_t i = 0; i < gainTmp.size(); ++i ) {
			currentPartialSum += gainTmp[i].first;
			if( currentPartialSum == maxPartialSum ) {
				vIdx.push_back(i);
			}
			else if( currentPartialSum > maxPartialSum ) {
				maxPartialSum = currentPartialSum;
				vIdx.clear();
				vIdx.push_back(i);
			}
		}
		if( maxPartialSum <= 0 ) break;
		for( size_t i = gainTmp.size() - 1; i > vIdx.back(); --i ) 			// undo
			FM -> exchangeCellAndUpdateNetSize(gainTmp[i].second);
			//FM -> exchangeCell(gainTmp[i].second);
		int currentCutSize = FM -> computeCutSize();
		if( sameCount ) break;
		if( currentCutSize >= oldCutSize ) sameCount = true;
		oldCutSize = currentCutSize;
		FM -> reset();
		cout << "iteration: " << ++itCount << "		cutSize: " << currentCutSize << "		\r";
		cout.flush();
	}
	cout << endl;
	FM -> writeFile(argv[2]);
}
