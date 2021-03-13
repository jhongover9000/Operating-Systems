CFLAGS = -O
CC = g++

mvote: main.o structures.o
	$(CC) $(CFLAGS) -o mvote main.o structures.o

main.o: main.cpp
	$(CC) $(CFLAGS) -c main.cpp

structures.o: structures.cpp voter.cpp
	$(CC) $(CFLAGS) -c structures.cpp voter.cpp
 
clean:
	rm -f *.o *~