//#include <iostream>
#include <stdlib.h>
#include <time.h>

int main(int argc, char** argv) {
    // Removed argv checks. (This can add unnecessary reads and writes)
    // Static init throws segfault (contiguous mem issues).
    int n = atoi(argv[1]);

    int** A = new int*[n];
    int** B = new int*[n];
    int** C = new int*[n];
    for(int i=0; i<n; i++) {
        A[i] = new int[n];
        B[i] = new int[n];
        C[i] = new int[n];
    }

    srand (time(NULL));
    for(int i=0; i<n; i++) {
        for(int j=0; j<n; j++) {
            A[i][j] = rand() % 100;
            B[i][j] = rand() % 100;
        }
    }
    for(int i=0;i<n;i++) {
        for(int j=0;j<n;j++) {
            C[i][j]=0;
            for(int k=0;k<n;k++) {
                C[i][j] += (A[i][k] * B[k][j]);
            }
        }
    }

    // Not printing because that adds unnecessary reads and writes.
    /*std::cout<<"A:"<<std::endl;
    for(int i=0; i<n; i++) {
        for(int j=0; j<n; j++) {
            std::cout<<A[i][j]<<" ";
        }
        std::cout<<std::endl;
    }
    std::cout<<"B:"<<std::endl;
    for(int i=0; i<n; i++) {
        for(int j=0; j<n; j++) {
            std::cout<<B[i][j]<<" ";
        }
        std::cout<<std::endl;
    }
    std::cout<<"C:"<<std::endl;
    for(int i=0; i<n; i++) {
        for(int j=0; j<n; j++) {
            std::cout<<C[i][j]<<" ";
        }
        std::cout<<std::endl;
    }*/

    return 0;
}
