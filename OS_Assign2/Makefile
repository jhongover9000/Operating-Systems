CFLAGS = -O
CC = g++

all: myhie coord sorter merger

myhie: root.o
	$(CC) $(CFLAGS) -o myhie root.o

coord: coord.o
	$(CC) $(CFLAGS) -o coord coord.o

sorter: sorter.o
	$(CC) $(CFLAGS) -o sorter sorter.o

merger: merger.o
	$(CC) $(CFLAGS) -o merger merger.o

root.o: root.cpp
	$(CC) $(CFLAGS) -c root.cpp

coord.o: coord.cpp
	$(CC) $(CFLAGS) -c coord.cpp

sorter.o: sorter.cpp
	$(CC) $(CFLAGS) -c sorter.cpp

merger.o: merger.cpp
	$(CC) $(CFLAGS) -c merger.cpp

clean:
	rm -f *.o *~