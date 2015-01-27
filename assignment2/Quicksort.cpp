/* 
 * File:   main.cpp
 * Author: Jean-Simon Barry
 *
 * Created on October 17, 2014, 3:55 PM
 */

#include <cstdlib>
#include "mpi.h"
#include <vector>
#include <algorithm> 
#include <utility>
#include <bitset>
#include <string>
#include <math.h>
#include <iostream>
#include <fstream>
#include <iterator>

#define MASTER 0 
#define MAX_BUFFER_SIZE 10000000

void parallelQuicksort(int input[], int size, int procID, int dimension);
int pivot = 0;
int	taskid,	        /* task ID - also used as seed number */
	numtasks,      /* number of tasks */
                    messageCount;

double timeStart, 
            timeStop;

MPI_Status status;

std::vector<int> array;

bool lessThanPivot(int value)
{return value < pivot;}

bool greaterThanPivot(int value)
{return value >= pivot;}

void parallelQuicksort(std::vector<int> &input, int size, int procID, const int dimension){
    srand(time(0));
    std::vector<int> b1 = input ,b2 = input;
    std::vector<int>::iterator vecItr;
    std::vector<int> receiveVector(MAX_BUFFER_SIZE);
    
    //convert ID to binary string and check first bit
    
    int currentDimPos;
    int otherProcID;

    for(int i=0; i<dimension; i++){
        
        //leadingCharAtDimension = string.at(string.length() - (1 + i));
        //get the other processor ID by doing an XOR with the dimension and the ID
        //ie 011 XOR 100 = 111 (processor 3 (011) sends to processor 7 (111) along the 3rd dimension (100))
        currentDimPos = pow(2, i);
        otherProcID = procID ^ currentDimPos;
        
        //get the pivot by randomly selecting a pivot from the input
        int randIndex = 0;//rand()%(int)(input.size()); 
        pivot = 0;//input[randIndex];
        
        //partition
        //copy just those smaller than pivot by removing those greater than pivot
        vecItr = std::remove_if(b1.begin(),b1.end(),greaterThanPivot);
        b1.resize(vecItr - b1.begin());    
        
        //copy just those greater than pivot by removing those smaller than pivot
        vecItr = std::remove_if(b2.begin(),b2.end(),lessThanPivot);
        b2.resize(vecItr - b2.begin());
        
        if((procID & currentDimPos) == 0){
            
            MPI_Send(&b2.front(), b2.size(), MPI_INT, otherProcID, dimension, MPI_COMM_WORLD);
            MPI_Recv(&receiveVector[0], receiveVector.size(), MPI_INT, otherProcID, dimension, MPI_COMM_WORLD, &status);
            MPI_Get_count(&status, MPI_INT, &messageCount);
            receiveVector.resize(messageCount);
            
            //concatenate the received vector
            b1.insert(b1.end(), receiveVector.begin(), receiveVector.end());
            
            //make input equal the concatenation
	    if(b1.size() != 0){
	      input.clear();
	      input = b1;
	    }
              
        }
        else{
            
            MPI_Send(&b1.front(), b1.size(), MPI_INT, otherProcID, dimension, MPI_COMM_WORLD);
            MPI_Recv(&receiveVector[0], receiveVector.size(), MPI_INT, otherProcID, dimension, MPI_COMM_WORLD, &status);
            MPI_Get_count(&status, MPI_INT, &messageCount);
            receiveVector.resize(messageCount);
            
            //concatenate the received vector
            b2.insert(b2.end(), receiveVector.begin(), receiveVector.end());
            
	    if(b2.size() != 0){
	      input.clear();
	      input = b2;
	    }
        }
        receiveVector.clear();
        receiveVector.resize(MAX_BUFFER_SIZE);
        
        

    }
    std::sort(input.begin(), input.end());
    
}

int main(int argc, char** argv) {

    
    MPI_Init(&argc,&argv);
    MPI_Comm_size(MPI_COMM_WORLD,&numtasks);
    MPI_Comm_rank(MPI_COMM_WORLD,&taskid);
    
    if (taskid == MASTER){
        
        std::ifstream is("input.txt");
        std::istream_iterator<double> start(is), end;
        std::vector<int> numbers(start, end);
        array = numbers;
        std::cout << "Read " << numbers.size() << " numbers" << std::endl;
    }
    std::cout << "number of processes " << numtasks<< std::endl;
     
    //time measurement
    MPI_Barrier(MPI_COMM_WORLD); 
    timeStart = MPI_Wtime();
    
    parallelQuicksort(array, 10, taskid, 3);
    
    //time measurement
    MPI_Barrier(MPI_COMM_WORLD); 
    timeStop = MPI_Wtime();

    MPI_Finalize();

    if (taskid == MASTER) { /* use time on master node */
        printf("\nRuntime = %10.8f\n", timeStop-timeStart);
        for(int i=0; i<array.size(); i++)
            std::cout << array[i] << " ";
        std::cout << std::endl;
    }
    
}

