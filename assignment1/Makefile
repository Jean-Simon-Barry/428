CC = mpicc
CFLAGS = -Wall
LIBS = -lmpi

all : calculatePi calculateSpawn parallelCalculatePi worker

calculatePi : calculatePi.c
	        $(CC)  $(CFLAGS) -o calculatePi calculatePi.c $(LIBS)

calculateSpawn : calculateSpawn.c
	        $(CC)  $(CFLAGS) -o calculateSpawn calculateSpawn.c $(LIBS)

parallelCalculatePi : parallelCalculatePi.c
	        $(CC)  $(CFLAGS) -o parallelCalculatePi parallelCalculatePi.c $(LIBS)

worker : worker.c
	        $(CC)  $(CFLAGS) -o worker worker.c $(LIBS)


clean : 
	rm calculatePi calculateSpawn parallelCalculatePi worker

