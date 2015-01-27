CCP = mpicxx
CC = mpicc
CFLAGS = -Wall
LIBS = -lmpi

all : Quicksort 

Quicksort : Quicksort.cpp
	        $(CCP)  $(CFLAGS) -o Quicksort Quicksort.cpp $(LIBS)

clean : 
	rm Quicksort