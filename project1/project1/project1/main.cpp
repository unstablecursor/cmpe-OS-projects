#include <iostream>
#include <vector>
#include <map>
#include <fstream>
#include <cstring>

using namespace std;
/** Process struct:
 *  instr -> Holds the instructions' lengths
 *  pc -> The program counter
 */
struct Process{
    vector<int> instr;
    int pc = 0;
    int arrival_time = 0;
};

int main(int argc, const char * argv[]) {
    map<string, Process> psMap;
    string line;
    ifstream myfile ("/Users/unstblecrsr/Desktop/CMPE322/cmpe-OS-projects/project1/project1/project1-supp/definition.txt");
    if (myfile.is_open())
    {
        string pname;
        string pdest;
        string parrival;
        while (myfile.peek()!=EOF){
            myfile >> pname;
            myfile >> pdest;
            myfile >> parrival;
            cout << pname << " " << pdest << " " << parrival << endl;
        
        }
        myfile.close();
    }
    else{
        cout << "cannot open" << endl;
    }
    

    
    std::cout << "Hello, World!\n";
    return 0;
}
