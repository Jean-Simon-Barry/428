#include <mpi.h>
#include <math.h>
#include <iostream>
#include <fstream>
#include <algorithm>
using namespace std;

//used when 2 nodes are not connected at all (as defined in input file example)
#define infinity 10000000

//parallel version of floydAlgorithm which broadcasts the results along the row/column. If processes require them, they will receive them via 
//broadcast. The function also takes as parameters where the start of the process' block is and how many elements it must compute (count)
void floydAlgo(int rank, int * matrix, int matrixColRowLength, int start, int count){

    int k,i,j;
    int ij,ik;
    //array holding the needed 
    int rowk[matrixColRowLength];
    for (k=0;k<matrixColRowLength;k++) {
        int owner = (int) ceil(k/count);
        //if row[k] is needed for this process, we need to calculate it. Else do nothing with it for now.
        if (rank == owner) {
                for(j=0;j<matrixColRowLength;j++)
                        rowk[j]=matrix[k*matrixColRowLength + j];
        }

        MPI_Bcast(&k, 1, MPI_INT, owner, MPI_COMM_WORLD);
        MPI_Bcast(rowk,matrixColRowLength,MPI_INT,owner,MPI_COMM_WORLD);
        for(i=start;i<start+count;i++){
            for(j=0;j<matrixColRowLength;j++){ 
                
                //index to access matrix elements
                ij = i * matrixColRowLength + j;
                //index to access row elements;
                ik = i * matrixColRowLength + k;
                if(i == j){ 
                    //0 along the diagonal since the node distance to itself
                    matrix[ij] = 0;                 
                }
                else{                 
                    if(matrix[ij] == 0) 
                        matrix[ij] = infinity;
                    //take the minimum of 
                    matrix[ij]= min(matrix[ij], matrix[ik]+rowk[j]);
                }
            }
        }
    }
}

//function which reads the matrix input file and populates the array.
int * readMatrixFile(char * file){
    
    ifstream matrixInput(file, ios::in);
    int n,
        nextValue; //temporary value for reading from file
    //the first line in the file is the row/column size o
    matrixInput >> n;  
    //allocate on heap! Must be cleared after use is finished.
    int * matrix = new int[n*n]; 
    for (int x = 0; x < n; x++)
        for (int y = 0; y < n; y++){
            matrixInput >> nextValue;
            matrix[x*n + y] = nextValue;
        }
    return matrix;   
}


//function to print matrix out to output.txt as requested in the example
void printMatrix(int * matrix, int matrixColRowLength){
    
    ofstream outputFile;
    outputFile.open ("output.txt");
    int index;
    for(int i=0;i<matrixColRowLength;i++){
        for (int j=0;j<matrixColRowLength;j++){
            index = i*matrixColRowLength+j;
            if(matrix[index] == infinity)
                outputFile << 0 << ' ';
            else
                outputFile << matrix[index] << ' ';
        }
        outputFile << endl;
    }
     outputFile.close();
}

//initially I wanted to implement some load balancing scheme used for the next pipeline part (based on p483 of the book), but it proved
//much more time consuming than I thought. Here the master processor simply takes care of dispatching the matrix to the workers
//at the end of a workers cycle, the master receives the results and stores it by first comparing and taking the max (taking the min would be cheating)
void dispatcher(int worldSize,char * inputFile, int matrixColRowLength){
    
    MPI_Status status;   
    int * currentMaxtrix = readMatrixFile(inputFile);
    MPI_Bcast (currentMaxtrix, matrixColRowLength*matrixColRowLength, MPI_INT, 0, MPI_COMM_WORLD);

    int count = (int) ceil(matrixColRowLength/worldSize);
    //start the algorithm
    floydAlgo(0,currentMaxtrix,matrixColRowLength,0,count);

    //temp matrix to compare the current and previous iterations of the adjacency matrix
    //it will the store the current workers work
    int t[(matrixColRowLength*matrixColRowLength)];
    
    for(int i=1;i<worldSize;i++){
        //wait for the worker p to finish and receive his section and store it in t[]
        MPI_Recv(&t, (matrixColRowLength*matrixColRowLength), MPI_INT, i, 0, MPI_COMM_WORLD,&status);

        for(int j=0;j<matrixColRowLength*matrixColRowLength;j++){
            //compare the previous iteration with the current updated and take the max to store in data[]
            currentMaxtrix[j] = max(currentMaxtrix[j],t[j]);
        }
    }
    //print the final result after all workers have finished
    printMatrix(currentMaxtrix, matrixColRowLength);
    //delete memory allocation
    delete[] currentMaxtrix;
}


// worker process receives matrix and calls floydAlgo() and sends his work back
void worker(int rank, int worldSize, int matrixColRowLength){  
    
    int matrix[matrixColRowLength * matrixColRowLength];     
    // Receive the matrix sent from dispatcher
    MPI_Bcast (&matrix, matrixColRowLength * matrixColRowLength, MPI_INT, 0, MPI_COMM_WORLD);
    //calculate start and count
    int count = (int) ceil(matrixColRowLength/worldSize);
    //where the process starts it's computation. Some assumptions are made:
    //1) Matrices are assumed to be squares of power of 2 size.
    //2) Processes are multiples of 2 as well.
    int start = rank * count;  
    //perform chunk of work on the adjacency matrix received from dispatcher
    floydAlgo(rank,matrix,matrixColRowLength,start,count);
    //send matrix back with the workers part done
    MPI_Send(matrix,matrixColRowLength * matrixColRowLength,MPI_INT,0,0,MPI_COMM_WORLD);
}

int main(int argc, char * argv[]){
    
    int worldSize,
            rank,
            matrixColRowLength;
    char * inputFile;
    
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &worldSize);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    //make sure the input file was specified
    MPI_Barrier(MPI_COMM_WORLD); 
    double timeStart = MPI_Wtime();
    if(argc < 1){
        
        cout << "failed to supply matrix file" << endl;
        MPI_Finalize();
        exit(1);
    }
    else{
        
        inputFile = argv[1];
        ifstream matrixInput(inputFile, ios::in);
        matrixInput >> matrixColRowLength;
        matrixInput.close();  
    }
    
    if(rank == 0)
        dispatcher(worldSize, inputFile, matrixColRowLength);               
    else        
        worker(rank,worldSize, matrixColRowLength); 
    
    MPI_Barrier(MPI_COMM_WORLD); 
    double timeStop = MPI_Wtime();
    if(rank == 0)
        cout << "Elapsed time: "<< timeStop - timeStart << endl;  
    MPI_Finalize();
}
