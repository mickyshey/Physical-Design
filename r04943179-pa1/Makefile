main: main.o FM_partition.o NetCell.o
		g++ -std=c++11 -o FM main.o FM_partition.o NetCell.o

NetCell.o: NetCell.cpp NetCell.h
		g++ -std=c++11 -c NetCell.cpp

FM_partition.o: FM_partition.cpp FM_partition.h NetCell.cpp NetCell.h
		g++ -std=c++11 -c FM_partition.cpp

main.o: main.cpp FM_partition.cpp FM_partition.h NetCell.cpp NetCell.h
		g++ -std=c++11 -c main.cpp

ctags:	
		ctags *.cpp *.h

clean:
		rm FM main.o FM_partition.o NetCell.o
