#include <iostream>
#include <fstream>
#include <algorithm>
#include <mpi.h>
using namespace std;

//infinity constant as shown in the sample input
#define infinity 10000000

void floydAlgorithm(int * matrix, int N){

    int k,i,j;
    int ij,ik,kj;

    for(k=0; k<N; k++){
        for(i=0; i<N; i++){
            for (j=0; j<N; j++){

                ij = i * N + j;
                ik = i * N + k;
                kj = k * N + j;

                if(i==j){
                        matrix[ij] = 0;
                }else{
                    if(matrix[ij] == 0) 
                        matrix[ij] = infinity;
                    if(matrix[ik]+matrix[kj]< matrix[ij]){
                            matrix[ij] = matrix[ik]+matrix[kj];
                    }
                }
            }
        }
    }
}

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

void printMatrix(int * matrix, int matrixColRowLength){
    
    ofstream outputFile;
    outputFile.open ("serialOutput.txt");
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

int main(int argc, char * argv[]){

    //just used for the time function.
    MPI_Init(&argc, &argv);

    char * matrixInputFile;
    int matrixColRowLength;

    if(argc < 1){      
        cout << "Please supply a filename" << endl;
        MPI_Finalize();
        exit(1);
    }else{
        matrixInputFile = argv[1];
    }
    
    //only 1 process
    double timeStart = MPI_Wtime();
    
    int * currentMaxtrix = readMatrixFile(matrixInputFile);
    
    ifstream matrixInput(matrixInputFile, ios::in);
    matrixInput >> matrixColRowLength;
    matrixInput.close();
    
    floydAlgorithm(currentMaxtrix,matrixColRowLength);

    printMatrix(currentMaxtrix, matrixColRowLength);
    //only 1 process
    double timeStop = MPI_Wtime();
    
    cout << "Elapsed time: "<< timeStop - timeStart << endl; 
    
    delete[] currentMaxtrix;
    MPI_Finalize();
    return 0;
}
