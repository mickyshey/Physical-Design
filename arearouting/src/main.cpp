// File name [ main.cpp ]
#include <iostream>

#include "Router.h"

int main(int argc, char** argv) 
{
	Router router;
	router.readInput(argv[1]);
	router.OASG();
	router.OAST();
}
