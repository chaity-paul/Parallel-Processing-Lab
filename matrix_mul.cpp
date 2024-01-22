#include <bits/stdc++.h>
using namespace std;
#include <mpi.h>
// void get_random_number()
void send_a_number(int number, int to){
    MPI_Send(&number, 1, MPI_INT, to, 0, MPI_COMM_WORLD);
}
void send_a_array(int *arr, int sz, int to){
    send_a_number(sz, to);
    MPI_Send(arr, sz, MPI_INT, to, 0, MPI_COMM_WORLD);
}
int receive_a_number(int from){
    int length;
    MPI_Recv(&length, 1, MPI_INT, from, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    return length;
}
vector<int> receive_a_array(int from){
    int length = receive_a_number(from);
    vector<int> ans(length);
    MPI_Recv(&ans[0], length, MPI_INT, from, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    return ans;
}

vector<int> multiplication(int *A, int *B, int N, int M, int P, int number_of_matrix){
    vector<int> C(N * P * number_of_matrix);

    for(int x = 0; x < number_of_matrix; x++){
        int add_a = x * N * M;
        int add_b = x * M * P;
        int add_c = x * N * P;

        for(int i = 0; i < N ;i++){
            for(int j = 0; j < P; j++){
                int sum = 0;

                for(int k =0 ; k < M; k++){
                    // A[i, k] * B[k , j]
                    sum += A[add_a + i * M + k] * B[add_b + k * P + j];
                }
                C[add_c + i * P + j] = sum;
            }
        }
    }
    return C;
}
int main(int argc, char *argv[]){
    MPI_Init(&argc, &argv);

    int number_of_task;
    MPI_Comm_size(MPI_COMM_WORLD, &number_of_task);

    int task_id;
    MPI_Comm_rank(MPI_COMM_WORLD, &task_id);
    if(task_id == 0){
        // C = A X B
        // A =  M X N
        // B = N X P
        // C = M X P

        int N , M , P;
        cout << "Enter the dimenssion of matrices...." << endl;
        cin >> N >> M >> P;

        int number_of_matrix; 
        cout << "Enter the number of matrix" << endl; 
        cin >> number_of_matrix;

        cout << "Enter matrix A: " << endl;

        int *A;
        int a_matrix_len = number_of_matrix * N * M;

        A = new int[a_matrix_len];
        for(int i = 0; i < a_matrix_len ; i++){
            //cin >> A[i];  // jodi input file thake.
             A[i] = 1;

        }
        cout << "Enter the matrix B: " << endl;

        int b_matrix_number = number_of_matrix * M * P;
        int *B;
        B = new int[b_matrix_number];
        for(int i = 0; i < b_matrix_number; i++){
            //cin >> B[i];
            B[i] = 1;
        }
        // cout << "Input taken" << endl;
        // input done;
        clock_t start, end;
        start = clock();
        // cout << N << " " << M << " " << P << " " << number_of_matrix << endl;
        // cout << a_matrix_len << endl;
        // cout << b_matrix_number << endl;
        for(int i = 1; i < number_of_task ; i++){

            int start_position = i * (number_of_matrix / number_of_task);
            int end_position = (i + 1) * (number_of_matrix / number_of_task);
            if(i == number_of_task - 1) {
                end_position = number_of_matrix;
            }
            send_a_number(N, i);
            send_a_number(M ,i);
            send_a_number(P, i);
            send_a_number(end_position - start_position, i);
            // [start , end)
            int start_index_for_A = start_position * N * M;
            int end_index_for_A = end_position * N * M;

            send_a_array(A + start_index_for_A, end_index_for_A - start_index_for_A, i);

            int start_index_for_B = start_position * M * P;
            int end_index_for_B = end_position * M * P;
            send_a_array(B + start_index_for_B, end_index_for_B - start_index_for_B, i);

            // cout << start_position << " " << end_position << " : i" << endl;
        }
        vector<int> C = multiplication(&A[0], &B[0], N, M, P, number_of_matrix / number_of_task);
        for(int i = 1; i < number_of_task; i++){
            vector<int> temp = receive_a_array(i);
            int number_of_element = temp.size();
            for(int j = 0; j < number_of_element; j++){
                C.push_back(temp[j]);
            }
        }

        // the ans is stored in C
        for(int i = 0 ;i < N; i++){
            for(int j = 0; j < P; j++){
                cout << C[i * P  + j] << " ";
            }
            cout << endl;
        }
        end = clock();
        double time_taken = double(end - start) / double(CLOCKS_PER_SEC);
        cout << time_taken << endl;
    }
    else{
        clock_t start =clock(), end;
        int N = receive_a_number(0);
        int M = receive_a_number(0);
        int P = receive_a_number(0);
        int number_of_matrix = receive_a_number(0);
        vector<int> A = receive_a_array(0);
        vector<int> B = receive_a_array(0);
        // cout << "data recieeved :" << task_id << endl;
        vector<int> C = multiplication(&A[0], &B[0], N, M, P, number_of_matrix);
        send_a_array(&C[0], C.size(), 0);
        end = clock();
        double time_taken = double(end - start) / double(CLOCKS_PER_SEC);
        cout << time_taken << "sec taken for: "<< task_id << endl;;


    }

    MPI_Finalize();
    return 0;
}