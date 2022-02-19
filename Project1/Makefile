# *-* Makefile *-*

all: mthreads
	
mthreads: mthreads.o
	g++ -pthread mthreads.o -o mthreads -lm
	
mthreads.o: mthreads.cpp
	g++ -c mthreads.cpp
clean:
	rm -f *.o
