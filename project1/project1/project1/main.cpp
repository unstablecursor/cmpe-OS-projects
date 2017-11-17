#include <iostream>
#include <fstream>
#include <cstring>
#include <queue>
#include <cstdio>

#define QUANT 100 //Time quantum.

using namespace std;

/** Process struct:
 *  instr -> Holds the instructions' lengths
 *  name -> Name of the process
 *  file -> Name of the code file
 */
struct Process{
    queue<int> instr;
    int arrival_time = 0;
    string name;
    string file;
    Process(int arr, string f, string n){
        arrival_time = arr;
        file = f;
        name = n;
    }
};

void print_queue(int t, queue<Process> ps_queue, ofstream & out){
    out << t << "::HEAD-";
    if(ps_queue.empty()){
        out << "-TAIL" << endl;
        return;
    }
    while (!ps_queue.empty()) {
        out << ps_queue.front().name << "-";
        ps_queue.pop();
    }
    out << "TAIL" << endl;
}

/** Round robin scheduler
 */
void rr_scheduler(queue<Process> & ps_q, ofstream & o){
    int time = 0;
    queue<Process> ps_queue;
    //Initial process has arrived.
    ps_queue.push(ps_q.front());
    print_queue(time, ps_queue, o);
    ps_q.pop();
    while (!ps_queue.empty()) {
        int q_count = 0;
        while (q_count < QUANT && !ps_queue.front().instr.empty()) {
            //Executing instructions.
            int t = ps_queue.front().instr.front();
            ps_queue.front().instr.pop();
            time += t;
            if(!ps_q.empty()){
                if(ps_q.front().arrival_time <= time){
                    //Process has arrived.
                    ps_queue.push(ps_q.front());
                    ps_q.pop();
                }
            }
            q_count += t;
        }
        if(ps_queue.front().instr.empty()){
            ps_queue.pop();
            print_queue(time, ps_queue, o);
        }
        else{
            ps_queue.push(ps_queue.front());
            ps_queue.pop();
            print_queue(time, ps_queue, o);
        }
    }
}

int main(int argc, const char * argv[]) {
    if (argc != 3) {
        cout << "Run the code with the following command: ./project1 [input_file] [output_file]" << endl;
        return 1;
    }
    queue<Process> ps_queue;
    //Reading from file
    ifstream myfile(argv[1]);
    if (myfile.is_open()){
        string pname, pdest, parrival;
        while (myfile.peek()!=EOF){
            myfile >> pname >> pdest >> parrival;
            Process p(stoi(parrival), pdest, pname);
            ifstream instruction_file(pdest);
            string s, t;
            if(instruction_file.is_open()){
                while(instruction_file.peek() != EOF){
                    instruction_file >> s;
                    instruction_file >> t;
                    p.instr.push(stoi(t));
                }
            }
            ps_queue.push(p);
        }
        myfile.close();
    }
    ofstream out(argv[2]);
    rr_scheduler(ps_queue, out);
    out.close();
    return 0;
}
