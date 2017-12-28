/**
 * Student Name: Recep Deniz Aksoy
 * Student Number: 2014400150
 * Project Number: #1
 */
#include <iostream>
#include <fstream>
#include <cstring>
#include <queue>
#include <cstdio>


/**
 * In this project we re asked to implement an OS, starting from
 * process scheduler which executes using round robin algorithm.
 *
 */

#define QUANT 100 //Time quantum.

using namespace std;

/** Process struct:
 *  instr -> Holds the instructions' lengths
 *  name -> Name of the process
 *  file -> Name of the code file
 *  waits -> Holds consecutive integers that corresponds the wait semaphore numbers
 *  signals -> Holds consecutive integers that corresponds the signal semaphore numbers
 *
 * Processes are defined as structs that have name and file name
 * of the process. Also process' arrival time is stored. An
 * integer vector holds the all instructions' execution time
 * consecutive.
 */
struct Process{
    queue<string> instruction;
    queue<int> instr;
    queue<int> waits;
    queue<int> signals;
    int arrival_time = 0;
    string name;
    string file;
    Process(int arr, string f, string n){
        arrival_time = arr;
        file = f;
        name = n;
    }
};
/** Semaphore
 * wait_q -> Wait queue for the semaphore
 * res -> To prevent starvation, res(reservation) string variable
 * that reserved the resorce
 * lock -> 1 if open, 0 if closed.
 */
struct Semaphore{
    queue<Process> wait_q;
    string res = "";
    int lock;
    Semaphore(int i){
        lock = i;
    }
};


/** print_queue function:
 *  int t -> Time
 *  queue<Process> ps_queue -> Process queue to be wrote in file.
 *  ofstream & out -> Output stream for writing in file.
 *
 */

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

/** print_wait_queue function:
 *  int t -> Time
 *  queue<Process> ps_queue -> Process queue to be wrote in file.
 *  int o -> Semaphore number.
 *
 */

void print_wait_queue(int t, queue<Process> ps_queue, int o){
    ofstream out("output_" + to_string(o) + ".txt", ios_base::app | ios_base::out);
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

/** rr_scheduler function:
 *  queue<Process> & ps_q -> Initial queue for process'
 *                           ordered in arrival times.
 *
 *  ofstream & o -> Output stream for writing in file.
 *                  That will be passed into print_queue
 *                  function.
 *
 *  Basic round robin algorithm. Steps described inline below.
 */

class Cache{
public:
    int c1 = -1;
    int c2 = -1;
    int lru = 0;
    bool accessCache(int x){
        if(x == c1){
            lru = 1;
            return true;
        }
        else if(x == c2){
            lru = 2;
            return true;
        }
        else{
            if(lru == 1){
                c2 = x;
                lru = 2;
            }
            else{
                c1 = x;
                lru = 1;
            }
            return false;
        }
    }
    Cache(){}
};

class Memory{
public:
    queue<Process> mem;
    Cache c;
    int mem_exit = INT_MAX;
    Memory(){
    }
    bool giveMemory(Process p, int time, int x){
        if(c.accessCache(x)){
            return false;
        }
        else{
            mem_exit = time;
            mem.push(p);
            print_wait_queue(time  - p.instr.front(), mem, 12);
            return true;
        }
    }
    Process getMemory(int time){
        Process p = mem.front();
        mem.pop();
        print_wait_queue(time, mem, 12);
        if(!mem.empty()){
            mem_exit = time + mem.front().instr.front();
        }
        else{
            mem_exit = INT_MAX;
        }
        p.instr.pop();
        p.instruction.pop();
        return p;
    }
};

class Display{
public:
    vector<queue<Process>> disp;
    vector<int> disp_exit = {INT_MAX,INT_MAX};
    Display(){
        queue<Process> disp1;
        queue<Process> disp2;
        disp.push_back(disp1);
        disp.push_back(disp2);
    }
    void giveDisplay(Process p, int time, int x){
        if(disp[x].empty()){
            disp_exit[x] = time;
        }
        disp[x].push(p);
        print_wait_queue(time - p.instr.front(), disp[x], (10 + x));
    }
    Process getDisplay(int x, int time){
        Process p = disp[x].front();
        disp[x].pop();
        print_wait_queue(time, disp[x], (10 + x));
        if(!disp[x].empty()){
            disp_exit[x] = time + disp[x].front().instr.front();
        }
        else{
            disp_exit[x] = INT_MAX;
        }
        p.instr.pop();
        p.instruction.pop();
        return p;
    }
};

void rr_scheduler(queue<Process> & ps_q, ofstream & o){
    
    Display display;
    Memory memory;
    int time = 0;
    vector<Semaphore> sems;
    for(int i = 0; i < 10; i++){
        Semaphore s(1);
        sems.push_back(s);
    }
    //Process queue.
    queue<Process> ps_queue;
    //Initial process has arrived.
    Process s = ps_q.front();
    ps_queue.push(s);
    //Printing the queue in first arrival.
    print_queue(time, ps_queue, o);
    ps_q.pop();
    //While loop executes until all processes has been processed.
    while (!ps_queue.empty()) {
        int q_count = 0;
        while (q_count < QUANT && !ps_queue.front().instr.empty()) {
            
            
            //Executing instructions.
            if(ps_queue.front().instr.front() > -1){
                int t = 0;
                string instruction = ps_queue.front().instruction.front();
                
                if(instruction[0] == 'd'){
                    int n = stoi(instruction.substr(6));
                    display.giveDisplay(ps_queue.front(), time + ps_queue.front().instr.front(), n);
                    ps_queue.pop();
                    print_queue(time, ps_queue, o);
                    q_count = 0;
                }
                else if (instruction[0] == 'r'){
                    int n = stoi(instruction.substr(6));
                    if(memory.giveMemory(ps_queue.front(), time + ps_queue.front().instr.front(), n)){
                        ps_queue.pop();
                        print_queue(time, ps_queue, o);
                        q_count = 0;
                    }
                    else{
                        ps_queue.front().instr.pop();
                        ps_queue.front().instruction.pop();
                    }
                }
                else if (instruction[0] == 'i' || instruction[0] == 'e'){
                    t = ps_queue.front().instr.front();
                    ps_queue.front().instr.pop();
                    ps_queue.front().instruction.pop();
                }
                time += t;
                if(!ps_q.empty()){
                    if(ps_q.front().arrival_time <= time){
                        //Process has arrived.
                        Process t = ps_q.front();
                        ps_queue.push(t);
                        ps_q.pop();
                    }
                }
                q_count += t;
            }
            
            if(display.disp_exit[0] <= time){
                ps_queue.push(display.getDisplay(0, time));
            }
            if(display.disp_exit[1] <= time){
                ps_queue.push(display.getDisplay(1, time));
            }
            if(memory.mem_exit <= time){
                ps_queue.push(memory.getMemory(time));
            }
            
            //Wait check
            else if(ps_queue.front().instr.front() == -1){
                int sem_num = ps_queue.front().waits.front();
                if(sems[sem_num].lock == 1 && (sems[sem_num].res == "" || sems[sem_num].res == ps_queue.front().name )){
                    cout <<  ps_queue.front().name << " lock " << sem_num << " " << time << endl;
                    sems[sem_num].lock = 0;
                    ps_queue.front().waits.pop();
                    ps_queue.front().instr.pop();
                }
                else{
                    Process t = ps_queue.front();
                    ps_queue.pop();
                    sems[sem_num].wait_q.push(t);
                    print_wait_queue(time, sems[sem_num].wait_q, sem_num);
                    print_queue(time, ps_queue, o);
                    q_count = 0;
                }
            }
            //Signal check
            else if(ps_queue.front().instr.front() == -2){
                ps_queue.front().instr.pop();
                int sem_num = ps_queue.front().signals.front();
                ps_queue.front().signals.pop();
                if(!sems[sem_num].wait_q.empty()){
                    Process t = sems[sem_num].wait_q.front();
                    sems[sem_num].res = t.name;
                    sems[sem_num].wait_q.pop();
                    ps_queue.push(t);
                    print_queue(time, ps_queue, o);
                    print_wait_queue(time, sems[sem_num].wait_q, sem_num);
                }
                else{
                    sems[sem_num].res = "";
                }
                sems[sem_num].lock = 1;
            }
        }
        
        if(ps_queue.front().instr.empty()){
            ps_queue.pop();
            print_queue(time, ps_queue, o);
        }
        else{
            Process t = ps_queue.front();
            ps_queue.pop();
            ps_queue.push(t);
            print_queue(time, ps_queue, o);
        }
    }
}

int main(int argc, const char * argv[]) {
    //If arguments not matched throw error.
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
            ifstream instruction_file(argv[1] + pdest);
            string s, t;
            if(instruction_file.is_open()){
                while(instruction_file.peek() != EOF){
                    instruction_file >> s;
                    instruction_file >> t;
                    if(s[0] == 'w'){
                        p.instr.push(-1);
                        p.instruction.push(s);
                        p.waits.push(s[6] - '0');
                    }
                    else if(s[0] == 's'){
                        p.instr.push(-2);
                        p.instruction.push(s);
                        p.signals.push(s[6] - '0');
                    }
                    else{
                        p.instruction.push(s);
                        p.instr.push(stoi(t));
                    }
                }
            }
            ps_queue.push(p);
        }
        myfile.close();
    }
    //ofstream out(argv[2]);
    ofstream out(argv[2]);
    //Send process arrival queue to rr_scheduler.
    rr_scheduler(ps_queue, out);
    out.close();
    return 0;
}
