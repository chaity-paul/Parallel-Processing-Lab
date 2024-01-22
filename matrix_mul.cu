
#include<bits/stdc++.h>
using namespace std;


__global__ void matrix_mul(int *A, int *B, int *C, int N , int M , int P, int n, int core){
    // n -> number of matrix
    int i = threadIdx.x; // koy number thread

    int start_index = i * (n / core);
    int end_index = (i + 1) * (n / core);
    if(i == core - 1){
        end_index = n;
    }
    for(int x  = start_index; x < end_index; x++){
        int add_a = x * N * M;
        int add_b = x * M * P;
        int add_c = x * N * P;

        for(int i = 0; i < N;i++){
            for(int j =0 ; j < P ; j++){
                int sum = 0;
                for(int k = 0; k < M; k++){
                    //sum += A[i][k] * B[k][j];
                    // sum += A[i * M + k] * B [k * P + j];
                    sum += A[add_a + i * M + k] * B[add_b + k * P + j];
                }
                // C[i][j] = sum;
                C[add_c + i * P + j] = sum;
            }
        }
    }

}
int main(int argc, char* argv[]){
    int N , M , P;
        cout << "Enter the dimenssion of matrices...." << endl;
        cin >> N >> M >> P;

        int number_of_matrix; 
        cout << "Enter the number of matrix" << endl; 
        cin >> number_of_matrix;

        cout << "Enter matrix A: " << endl;

        int *A;
        int a_element = number_of_matrix * N * M;

        A = new int[a_element];
        for(int i = 0; i < a_element ; i++){
            //cin >> A[i];  // jodi input file thake.
             A[i] = 1;

        }
        cout << "Enter the matrix B: " << endl;

        int b_element = number_of_matrix * M * P;
        int *B;
        B = new int[b_element];
        for(int i = 0; i < b_element; i++){
            //cin >> B[i];
            B[i] = 1;
        }
        // cout << "Input taken" << endl;
        // input done;

        int *C;
        int c_element = number_of_matrix * N * P;

        C = new int[c_element];

        // A, B C ram array
        // DA, DB, DC device array
      int *DA, *DB, *DC;
        cudaMalloc(&DA, a_element * sizeof(int));
        cudaMalloc(&DB, b_element * sizeof(int));
        cudaMalloc(&DC, c_element * sizeof(int));

        cudaMemcpy(DA, A, a_element * sizeof(int), cudaMemcpyHostToDevice);
        cudaMemcpy(DB, B, b_element * sizeof(int), cudaMemcpyHostToDevice);

        int number_of_core = 2;
        cout << "enter number of core: " << endl;
        cin >> number_of_core;
        cudaEvent_t start, end;
        cudaEventCreate(&start);
        cudaEventCreate(&end);
        cudaEventRecord(start);
        matrix_mul<<<1, number_of_core>>> (DA, DB, DC, N , M, M, number_of_matrix, number_of_core);
// fuction name<<<block, ekta block e koto core thakbe>>>
        cudaDeviceSynchronize();
        cudaEventRecord(end);
        cudaEventSynchronize(end);
        cudaMemcpy(C, DC, c_element * sizeof(int), cudaMemcpyDeviceToHost);

        float mili;
        cout << "function done" << endl;

        cudaEventElapsedTime(&mili, start, end);

        cout << mili << " time passed" << endl;
        for(int i = 0;i < N; i++){
            for(int j = 0; j < P; j++){
                cout << C[i * P + j] << " ";
            }
            cout << endl;
        }


}