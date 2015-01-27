#include "mpi.h"
#include <stdio.h>
#include <stdlib.h>

double dboard (int darts);
#define DARTS 50000     /* number of throws at dartboard */
#define ROUNDS 100      /* number of times "darts" is iterated */
#define MASTER 0        /* task ID of master task */


int main (int argc, char *argv[])
{
  double	pisum;	        /* sum of tasks' pi values */
int	taskid,	        /* task ID - also used as seed number */
	numtasks,       /* number of tasks */
            err[4];         //error code

 double timeStart, 
            timeStop;


/* Obtain number of tasks and task ID */
MPI_Init(&argc,&argv);
MPI_Comm_size(MPI_COMM_WORLD,&numtasks);
MPI_Comm_rank(MPI_COMM_WORLD,&taskid);
MPI_Comm children;
printf ("MPI task %d has started...\n", taskid);

//start time. Create barrier so all processes reach this point before continuing
MPI_Barrier(MPI_COMM_WORLD);
timeStart = MPI_Wtime();

//spawn 4 minions that execute "worker" program
MPI_Comm_spawn("worker", NULL, 4, MPI_INFO_NULL, 0, MPI_COMM_WORLD, &children, err);

if (taskid == MASTER) {
    //recev the average pi value from the worker of rank 0 of children and store it in &pisum
      MPI_Recv(&pisum, 1, MPI_DOUBLE, 0, 0, children, MPI_STATUS_IGNORE);    
      printf("   After %8d throws, average value of pi = %10.8f\n",
              (DARTS * ROUNDS),pisum); 
      printf ("\nReal value of PI: 3.1415926535897 \n");
   }
//time measurement again with a barrier to make sure all processes reach this point.
MPI_Barrier(MPI_COMM_WORLD); 
timeStop = MPI_Wtime();

MPI_Finalize();

if (taskid == MASTER) { /* use time on master node */
    printf("\nRuntime = %10.8f\n", timeStop-timeStart);
}
return 0;
}
