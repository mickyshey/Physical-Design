main: main.o FM_partition.o NetCell.o
		g++ -std=c++11 -O3 -o FM main.o FM_partition.o NetCell.o

NetCell.o: NetCell.cpp NetCell.h
		g++ -std=c++11 -O3 -c NetCell.cpp

FM_partition.o: FM_partition.cpp FM_partition.h NetCell.cpp NetCell.h
		g++ -std=c++11 -O3 -c FM_partition.cpp

main.o: main.cpp FM_partition.cpp FM_partition.h NetCell.cpp NetCell.h
		g++ -std=c++11 -O3 -c main.cpp

ctags:	
		ctags *.cpp *.h

clean:
		rm main.o FM_partition.o NetCell.o
