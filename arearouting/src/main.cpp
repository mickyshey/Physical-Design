// File name [ main.cpp ]
#include <iostream>

#include "Router.h"

int main(int argc, char** argv) 
{
	std::cout << "hello world ..." << std::endl;
	Router router;
	router.readInput(argv[1]);
}
