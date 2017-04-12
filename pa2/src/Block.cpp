// File name [ Block.cpp ]

#include <string>

#include "Block.h"

using namespace std;

void
Block::setEqual(Block* b)
{
	_name = b -> getName();
	_width = b -> getWidth();
	_height = b -> getHeight();
}
