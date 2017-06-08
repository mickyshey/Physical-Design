#CFLAGS = -O3 -DNDEBUG -std=c++11
CFLAGS = -O2 -Wall -std=c++11

main: main.o Router.o Util.o
		g++ $(CFLAGS) -o mlllk main.o Router.o	Util.o
main.o: main.cpp Router.h Util.h
		g++ $(CFLAGS) -c main.cpp
Util.o: Util.h Util.cpp
		g++ $(CFLAGS) -c Util.cpp
Router.o: Router.h Router.cpp Util.h
		g++ $(CFLAGS) -c Router.cpp

ctags:
		ctags *.cpp *.h
clean:
		rm *.o
