/* 
 * File:   worker.c
 * Author: hansolo
 *
 * Created on September 26, 2014, 9:41 AM
 */


void srandom (unsigned seed);
double dboard (int darts);
#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#define DARTS 5000     /* number of throws at dartboard */
#define ROUNDS 100      /* number of times "darts" is iterated */
#define MASTER 0        /* task ID of master task */

int	taskid,	        /* task ID - also used as seed number */
	numtasks,       /* number of tasks */
	rc,             /* return code */
	i,
            err[4];         //error code

double	homepi,         /* value of pi calculated by current task */
	pisum,	        /* sum of tasks' pi values */
	pi,	        /* average of pi after "darts" is thrown */
	avepi;	

/*
 * 
 */
int main(int argc, char** argv) {

    MPI_Comm parent;
    MPI_Init(&argc, &argv);MPI_Comm_get_parent(&parent);
    MPI_Comm_size(MPI_COMM_WORLD,&numtasks);
    MPI_Comm_rank(MPI_COMM_WORLD, &taskid);
    MPI_Comm_get_parent(&parent);
    srandom (taskid);
    for (i = 0; i < ROUNDS; i++) {
        /* All tasks calculate pi using dartboard algorithm */
        homepi = dboard(DARTS);

	//take the sum 
        rc = MPI_Reduce(&homepi, &pisum, 1, MPI_DOUBLE, MPI_SUM,
                        MASTER, MPI_COMM_WORLD);
        if (rc != MPI_SUCCESS)
           printf("%d: failure on mpc_reduce\n", taskid);
}
    avepi = pisum/numtasks; 
     
    if(taskid == MASTER){
        //send homepi to rank 0 processes of parent group
         MPI_Send(&avepi, 1, MPI_DOUBLE, 0, 0, parent);
    }
    MPI_Finalize();
    return (EXIT_SUCCESS);
}


double dboard(int darts)
{
#define sqr(x)	((x)*(x))
long random(void);
double x_coord, y_coord, pi, r; 
int score, n;
unsigned int cconst;  /* must be 4-bytes in size */
/*************************************************************************
 * The cconst variable must be 4 bytes. We check this and bail if it is
 * not the right size
 ************************************************************************/
if (sizeof(cconst) != 4) {
   printf("Wrong data size for cconst variable in dboard routine!\n");
   printf("See comments in source file. Quitting.\n");
   exit(1);
   }
   /* 2 bit shifted to MAX_RAND later used to scale random number between 0 and 1 */
   cconst = 2 << (31 - 1);
   score = 0;

   /* "throw darts at board" */
   for (n = 1; n <= darts; n++)  {
      /* generate random numbers for x and y coordinates */
      r = (double)random()/cconst;
      x_coord = (2.0 * r) - 1.0;
      r = (double)random()/cconst;
      y_coord = (2.0 * r) - 1.0;

      /* if dart lands in circle, increment score */
      if ((sqr(x_coord) + sqr(y_coord)) <= 1.0)
           score++;
      }

/* calculate pi */
pi = 4.0 * (double)score/(double)darts;
return(pi);
} 

