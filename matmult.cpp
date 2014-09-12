#include <iostream>
#include <stdlib.h>
#include <time.h>

using namespace std;

int main(int argc, char** argv) {
    if(argc<2) {
        cout<<"Usage: "<<argv[0]<<" <matrix dimension>"<<endl;
        exit(-1);
    }
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
    for(int i=0;i<n;i++){
        for(int j=0;j<n;j++){
            C[i][j]=0;
            for(int k=0;k<n;k++){
                C[i][j] += (A[i][k] * B[k][j]);
            }
        }
    }

    cout<<"A:"<<endl;
    for(int i=0; i<n; i++) {
        for(int j=0; j<n; j++) {
            cout<<A[i][j]<<" ";
        }
        cout<<endl;
    }
    cout<<"B:"<<endl;
    for(int i=0; i<n; i++) {
        for(int j=0; j<n; j++) {
            cout<<B[i][j]<<" ";
        }
        cout<<endl;
    }
    cout<<"C:"<<endl;
    for(int i=0; i<n; i++) {
        for(int j=0; j<n; j++) {
            cout<<C[i][j]<<" ";
        }
        cout<<endl;
    }

    return 0;
}
