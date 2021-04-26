CFLAGS = -O
CC = g++

all: chef saladmaker

chef: chef.o
	$(CC) $(CFLAGS) -o chef chef.o

saladmaker: saladmaker.o
	$(CC) $(CFLAGS) -o saladmaker saladmaker.o

chef.o: chef.cpp
	$(CC) $(CFLAGS) -c chef.cpp

saladmaker.o: saladmaker.cpp
	$(CC) $(CFLAGS) -c saladmaker.cpp

clean:
	rm -f *.o *~