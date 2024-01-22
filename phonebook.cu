#include <bits/stdc++.h>
using namespace std;

__global__ void string_matching(char *text, char *pattern,int *index, int p_len, int n , int core){
    int i = threadIdx.x;
    int start_index = i * (n / core);
    int end_index = (i + 1) * (n / core);
    if(i == core -1){
        end_index = n;
    }

    for(int i = start_index; i < end_index; i++){
        int cnt = 0;
        for(int j = index[i]; text[j] != '\n'; j++){
            cnt = 0;
            for(int k = 0; k < p_len; k++){
                if(pattern[k] == text[k + j]){
                    cnt++;
                }else if((pattern[k] - 32) == text[k + j]){
                    cnt++;
                }
                else break;
            }
            if(cnt == p_len){
                index[i] = -1;
                break;
            }
        }
    }
}
int main(int argc, char *argv[]){
    vector<string> contact_list;
    string pattern = argv[1];
    int m = pattern.size();
    for(int i = 0; i < m ;i++){
        pattern[i] = tolower(pattern[i]);
    }

    for(int i = 2; i < argc; i++){
        ifstream buf(argv[i]); // taking the  file
        string contact;
        while(getline(buf, contact)){
            contact_list.push_back(contact); // take input  from the file
        }
    }
    
    int n = contact_list.size();
   
    vector<int> index(n);
    string text = "";

    for(int i = 0; i < n; i++){
        index[i] = text.size();
        text += contact_list[i] + "\n";
    }

    char *DT, *DP;
    cudaMalloc(&DT, (text.size() + 1) * sizeof(char));
    cudaMemcpy(DT, &text[0], (text.size() + 1) * sizeof(char), cudaMemcpyHostToDevice);
    // copy Text to DT

    cudaMalloc(&DP, (pattern.size() + 1) * sizeof(char));
    cudaMemcpy(DP, &pattern[0], (pattern.size() + 1) * sizeof(char), cudaMemcpyHostToDevice);
    // copy pattern to DP

    int *DI; // device index
    cudaMalloc(&DI, n * sizeof(int));
    cudaMemcpy(DI, &index[0], n * sizeof(int), cudaMemcpyHostToDevice);
    cudaEvent_t start, end;
    cudaEventCreate(&start);
    cudaEventCreate(&end);
    cudaEventRecord(start);
    int number_of_core = 2;
    cout << "Enter the number of core: " << endl;
    cin >> number_of_core;

    string_matching<<<1 , number_of_core>>>(DT, DP, DI, pattern.size(), n, number_of_core);
    cudaDeviceSynchronize();
    cudaEventRecord(end);
    cudaEventSynchronize(end);
    cudaMemcpy(&index[0], DI, n * sizeof(int), cudaMemcpyDeviceToHost);

    float mili;
    cout << "function done" << endl;

    cudaEventElapsedTime(&mili, start, end);

    cout << mili << " time passed" << endl;
    set<string> ans;
    for(int i = 0; i < n; i++){
        if(index[i] == -1){
            ans.insert(contact_list[i]);
        }
    }
    for(set<string>::iterator it = ans.begin(); it != ans.end(); it++){
        cout << *it << endl;
    }
    
}