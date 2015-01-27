CCP = mpicxx
LIBS= -lmpi
CFLAGS = -Wall
LIBS = -lmpi

all: parallelFloyd serialFloyd


parallelFloyd: parallelFloyd.cpp

	$(CCP) $(CFLAGS) parallelFloyd.cpp -o parallelFloyd $(LIBS)
	touch output.txt

serialFloyd: serialFloyd.cpp

	$(CCP) $(CFLAGS) serialFloyd.cpp -o serialFloyd $(LIBS)
	touch serialOutput.txt

clean:
	rm parallelFloyd
	rm serialFloyd
	rm output.txt
	rm serialOutput.txt
