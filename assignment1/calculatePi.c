#include <mpi.h>
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <time.h>



void calculatePiSequential(int nbPointsIn){
    
    srand(time(0));
    double circ_count = 0;
    int i =0;
    for( i=0; i<nbPointsIn; i++){
        double xPos = ((double) rand() / (RAND_MAX));
        double yPos = ((double) rand() / (RAND_MAX));
        double circleEdge = sqrt(pow(xPos, 2) + pow(yPos,2));
        
        if(circleEdge<=1)
            circ_count++;    
    }
    double pi = 4.0 * circ_count / nbPointsIn;
    printf("Calculated pi is %10.8f",pi );
}

double timeStart, timeStop;

int main(int argc, char *argv[]) {
    
    int taskID, 
      numTasks;
    
    MPI_Init( &argc, &argv );
     
    MPI_Comm_rank(MPI_COMM_WORLD, &taskID);
    MPI_Comm_size(MPI_COMM_WORLD, &numTasks); 
    
    //wait for all processes to reach this point before calculating time.
    MPI_Barrier(MPI_COMM_WORLD); 
    timeStart = MPI_Wtime();
    
    calculatePiSequential(100000);
    
    //wait for processes to reach this point before stopping time.
    MPI_Barrier(MPI_COMM_WORLD); 
    timeStop = MPI_Wtime();
    MPI_Finalize();
    
    if (taskID == 0) { /* use time on master node */
    printf("\nRuntime = %10.8f\n", timeStop-timeStart);
    }
    return 0;
}
