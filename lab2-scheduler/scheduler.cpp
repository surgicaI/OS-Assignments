#include <iostream>
#include <unistd.h>
#include <stdio.h>
#include <fstream>
#include <queue>
using namespace std;

bool verbose_output = false;
const bool LOGS_ENABLED = false;

//Classes definitions

class Process{

};

/*-------------------------------------------------------
parent class for all types of schedulers
---------------------------------------------------------*/
class Scheduler{
public:
    virtual Process* getEvent() = 0;
    virtual void setEvent(Process* p) = 0;
    virtual bool isEmpty() = 0;
    virtual string getName() = 0;
};

/*-------------------------------------------------------
Implementation of first come first serve scheduler
---------------------------------------------------------*/
class FCFS: public Scheduler{
    queue<Process*> ready_queue;
public:
    Process* getEvent(){
        if(!isEmpty()){
            Process *process;
            process = ready_queue.front();
            ready_queue.pop();
            return process;
        }
        return NULL;
    }
    void setEvent(Process* p){
        ready_queue.push(p);
    }
    bool isEmpty(){
        return ready_queue.empty();
    }
    string getName(){
        return "FCFS";
    }
};

/*-------------------------------------------------------
Implementation of last come first serve scheduler
---------------------------------------------------------*/
class LCFS: public Scheduler{
    queue<Process*> ready_queue;
public:
    Process* getEvent(){
        if(!isEmpty()){
            Process *process;
            process = ready_queue.front();
            ready_queue.pop();
            return process;
        }
        return NULL;
    }
    void setEvent(Process* p){
        ready_queue.push(p);
    }
    bool isEmpty(){
        return ready_queue.empty();
    }
    string getName(){
        return "LCFS";
    }
};

class SJF: public Scheduler{
    queue<Process*> ready_queue;
public:
    Process* getEvent(){
        if(!isEmpty()){
            Process *process;
            process = ready_queue.front();
            ready_queue.pop();
            return process;
        }
        return NULL;
    }
    void setEvent(Process* p){
        ready_queue.push(p);
    }
    bool isEmpty(){
        return ready_queue.empty();
    }
    string getName(){
        return "SJF";
    }
};

class RR: public Scheduler{
private:
    queue<Process*> ready_queue;
    int quantum;
public:
    RR(int n){
        quantum = n;
    }
    Process* getEvent(){
        if(!isEmpty()){
            Process *process;
            process = ready_queue.front();
            ready_queue.pop();
            return process;
        }
        return NULL;
    }
    void setEvent(Process* p){
        ready_queue.push(p);
    }
    bool isEmpty(){
        return ready_queue.empty();
    }
    string getName(){
        return "RR " + to_string(quantum);
    }
};

class PRIO: public Scheduler{
private:
    queue<Process*> ready_queue;
    int quantum;
public:
    PRIO(int n){
        quantum = n;
    }
    Process* getEvent(){
        if(!isEmpty()){
            Process *process;
            process = ready_queue.front();
            ready_queue.pop();
            return process;
        }
        return NULL;
    }
    void setEvent(Process* p){
        ready_queue.push(p);
    }
    bool isEmpty(){
        return ready_queue.empty();
    }
    string getName(){
        return "PRIO " + to_string(quantum);
    }
};


//Method declarations
void init_random_vals(string);

//variables
Scheduler* my_scheduler;
int* randvals;
int randvals_size = 0;

//main function
int main(int argc, char* argv[]){
    //Reading input from console and initializing
    int c;
    char *s_value = NULL;
    string input_file = "";
    while ((c = getopt(argc, argv, "vs:")) != -1){
        switch (c){
            case 'v':
                verbose_output = true;
                break;
            case 's':
                s_value = optarg;
                break;
            case '?':
                if (optopt == 's')
                    fprintf (stderr, "Option -%c requires an argument.\n", optopt);
                else if (isprint (optopt))
                    fprintf (stderr, "Unknown option `-%c'.\n", optopt);
                else
                    fprintf (stderr,"Unknown option character `\\x%x'.\n", optopt);
                return 1;
            default:
                abort ();
        }
    }

    //analysing non-option arguments
    if(optind+1>=argc){
        cout << "'Input file' or 'random file' not provided" <<endl;
        return 0;
    }
    input_file = argv[optind];

    //Create scheduler object corresponding to the argument
    if(s_value==NULL){
        cout << "Scheduler specification not provided" << endl;
        return 0;
    } else if (*s_value == 'F'){
        my_scheduler = new FCFS();
    } else if (*s_value == 'L'){
        my_scheduler = new LCFS();
    } else if (*s_value == 'S'){
        my_scheduler = new SJF();
    } else if (*s_value == 'R'){
        s_value++;
        int x = atoi(s_value);
        my_scheduler = new RR(x);
    } else if (*s_value == 'P'){
        s_value++;
        int x = atoi(s_value);
        my_scheduler = new PRIO(x);
    } else{
        cout << "Unknown scheduler argument" <<endl;
    }

    init_random_vals(argv[optind+1]);
    return 0;
}

//method definitions
void init_random_vals(string rand_file_name){
    ifstream fin(rand_file_name);
    fin  >> randvals_size;
    randvals = new int[randvals_size];
    int index = 0, num = 0;
    while(fin>>num){
        randvals[index] = num;
        index++;
    }
}
