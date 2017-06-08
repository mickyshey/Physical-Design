// File name [ Util.cpp ]

#include <string>
#include <vector>

#include "Util.h"

std::vector<std::string>
getTokens(const std::string& s, const std::string& sep)
{
	std::vector<std::string> tmp;
	size_t left = 0, right = 0;

	while( left != std::string::npos ) {	
		right = s.find_first_of(sep, left);
		std::string token = s.substr(left, right - left);
		tmp.push_back(token);
		left = s.find_first_not_of(sep, right);
	}
	return tmp;
}
