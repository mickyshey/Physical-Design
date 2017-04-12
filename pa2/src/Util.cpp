// File name [ Util.cpp ]

#include <string>
#include <vector>

#include "Util.h"

using namespace std;

vector<string>
getTokens(const string& s)
{
	vector<string> tmp;
	size_t left = 0, right = 0;

	while(	right != string::npos ) {
		right = s.find_first_of(" \t", left);
		string token = s.substr(left, right - left);
		tmp.push_back(token);
		left = right + 1;
	}
	return tmp;
}
