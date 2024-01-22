#include<bits/stdc++.h>
#include "mpi.h"
using namespace std;
void send_a_number(int number, int to){
    MPI_Send(&number, 1, MPI_INT, to, 0, MPI_COMM_WORLD);
}

void send_a_string(string str, int to){
    int length = str.size() + 1;
    send_a_number(length, to);
    if(length == 0) return;
    MPI_Send(&str[0], length, MPI_CHAR, to, 0, MPI_COMM_WORLD);
}

int receive_a_number(int from){
    int length;
    MPI_Recv(&length, 1, MPI_INT, from, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    return length;
}

string receive_a_string(int from){
    int length  = receive_a_number(from);
    if(length == 0) return "";
    char *buffer = new char[length];
    MPI_Recv(buffer, length, MPI_CHAR, from, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    return string(buffer);
}

string generate_string(vector<string> &str, int start_position, int end_position){
    string ans = "";

    for(int i = start_position; i < end_position; i++){
        ans += str[i] + "\n";
    }
    return ans;
}
vector<string> get_string_list(string str){
    stringstream ss(str);
    vector<string> ans;
    string word;
    while(getline(ss, word)){
        ans.push_back(word);
    }
    return ans;
}

bool check(string text, string pattern){
    // jodi text pattern er sathe match kore to print kore dibo.
    // otherwise ignore

    int n = text.size();
    int m = pattern.size();
    int  cnt = 0;
    for(int i = 0; i < n; i++){
        cnt = 0;
        for(int j = 0; j < m ; j++){
            if((text[i + j] == pattern[j]) || (text[i + j] == (pattern[j] xor 32))){
                cnt++;
            }
            else break;
        }
        if(cnt == m) return true;
    }
    return false;
}
int32_t main(int argc, char* argv[]){
    MPI_Init(&argc, &argv);

    int number_of_task;
    MPI_Comm_size(MPI_COMM_WORLD, &number_of_task);

    int task_id;
    MPI_Comm_rank(MPI_COMM_WORLD, &task_id);
    
    if(task_id == 0){
   
        vector<string> contact_list;
        string pattern = argv[1];

        for(int i = 2; i < argc; i++){
            ifstream buf(argv[i]); // taking the  file
            string contact;
            while(getline(buf, contact)){
                contact_list.push_back(contact); // take input  from the file
            }
        }
        
        int n = contact_list.size();
        // cout << " for checking :" << endl;
        // for(int i = 0; i< n ; i++){
        //     cout << contact_list[i] << endl;
        // }
        // cout << "input taken " << endl;
        clock_t start, end;
        start = clock();
       // data send korbo.

       for(int i =  1; i < number_of_task; i++){
        int start_position = i *  (n / number_of_task);
        int end_position = (i + 1) * (n / number_of_task);

        string for_send_name = generate_string(contact_list, start_position, end_position);
        send_a_string(for_send_name, i);
        send_a_string(pattern, i);
       }

       string work_for = generate_string(contact_list, 0 , n/ number_of_task);
       vector<string> work_list = get_string_list(work_for);
       vector<string> temp;
       set<string>ans;
       for(int i = 0; i < n / number_of_task; i++){
        if(check(contact_list[i], pattern)){
            temp.push_back(contact_list[i]);
        }
       }
       work_for = generate_string(temp, 0, temp.size());
       work_list = get_string_list(work_for);
       for(int i = 0; i < work_list.size(); i++){
        ans.insert(work_list[i]);
       }

       for(int i = 1; i < number_of_task; i++){
        string rec = receive_a_string(i);
        vector<string> list_rec = get_string_list(rec);
        int x = list_rec.size();
        for(int j = 0; j < x; j++){
            ans.insert(list_rec[j]);
        }
       }
       n = ans.size();
       for(set<string>::iterator it = ans.begin() ; it != ans.end(); it++){
        cout << *it << endl;
       }
        
        end = clock();
        double time_taken = double(end - start) / double(CLOCKS_PER_SEC);
        cout << "Time taken for: " << number_of_task <<" process is : "<< time_taken << endl;
    }
    else{
        string name = receive_a_string(0);
        vector<string> contact_list = get_string_list(name);
        string pattern = receive_a_string(0);

        vector<string>ans;
        int n = contact_list.size();
        for(int i = 0; i < n; i++){
            if(check(contact_list[i], pattern)){
                ans.push_back(contact_list[i]);
            }
        }
        string for_send = generate_string(ans, 0, ans.size());
        send_a_string(for_send, 0);

    }
   

    MPI_Finalize();
}