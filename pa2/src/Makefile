CFLAGS = -O2 -std=c++11

main: main.o FloorplanMgr.o Block.o Util.o
		g++ $(CFLAGS) -o floorplan main.o FloorplanMgr.o Block.o Util.o
main.o: main.cpp FloorplanMgr.h
		g++ $(CFLAGS) -c main.cpp
FloorplanMgr.o: FloorplanMgr.cpp FloorplanMgr.h Block.h Util.h
		g++ $(CFLAGS) -c FloorplanMgr.cpp
Block.o: Block.cpp Block.h
		g++ $(CFLAGS) -c Block.cpp
Util.o: Util.cpp Util.h
		g++ $(CFLAGS) -c Util.cpp

ctags:
		ctags *.cpp *.h
clean:
		rm *.o floorplan
