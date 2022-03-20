# *-* Makefile *-*

all: visitor
	
visitor: visitor.o
	g++ -pthread visitor.o -o visitor -lm
	
visitor.o: visitor.cpp
	g++ -c visitor.cpp
clean:
	rm -f *.o	
