#include "simulation.h"

/*-------------------------------------------------------
Variable declarations
---------------------------------------------------------*/
Scheduler* my_scheduler;
priority_queue<Process*, vector<Process*>, ProcessComparator> event_queue;
int randvals_size = 0;
int* randvals;
int ofs = 0; //offset for random values 
int process_id = 0;
int current_time = 0;

/*-------------------------------------------------------
Main function
---------------------------------------------------------*/
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

    initRandomVals(argv[optind+1]);
    initSimulation(argv[optind]);
    runSimulation();
    return 0;
}

/*-------------------------------------------------------
Method definitions
---------------------------------------------------------*/
void initRandomVals(string rand_file_name){
    ifstream fin(rand_file_name);
    fin  >> randvals_size;
    randvals = new int[randvals_size];
    int index = 0, num = 0;
    while(fin>>num){
        randvals[index] = num;
        index++;
    }
}

int getrand(int n){
    int result = 1 + (randvals[ofs] % n);
    if (++ofs >= randvals_size)
        ofs = 0;
    return result;
}

void initSimulation(string input_file){
    int arrival_time = 0;
    int total_cpu_time = 0;
    int cpu_burst = 0;
    int io_burst = 0;
    ifstream fin(input_file);
    while((fin >> arrival_time) && (fin >> total_cpu_time) && (fin >> cpu_burst) && (fin >> io_burst)){
        createProcess(arrival_time,total_cpu_time,cpu_burst,io_burst);
    }
    /*-------------------------------------------------------
    For debugging purposes
    ---------------------------------------------------------*/
    if(LOGS_ENABLED){
        cout<<"initSimulation(): start, event_queue:"<<endl;
        priority_queue<Process*, vector<Process*>, ProcessComparator> event_queue2;
        event_queue2 = event_queue;
        while(!event_queue2.empty()){
            Process *p = event_queue2.top();
            event_queue2.pop();
            cout<<p->id<<" "<<p->AT<<" "<<p->TC<<" "<<p->CB<<" "<<p->IO<<endl;
        }
        cout<<"initSimulation(): end, event_queue:"<<endl;
    }
}

void createProcess(int arrival_time, int total_cpu_time, int cpu_burst, int io_burst){
    Process * process = new Process();
    process->id = process_id++;
    process->AT = arrival_time;
    process->TC = total_cpu_time;
    process->CB = cpu_burst;
    process->IO = io_burst;
    process->time_left = total_cpu_time;
    process->priority = getrand(4);
    process->event_time = arrival_time;
    process->state = STATE_CREATED;
    process->previous_state = "";
    event_queue.push(process);
    /*-------------------------------------------------------
    For debugging purposes
    ---------------------------------------------------------*/
    if(LOGS_ENABLED){
        cout<<"createProcess():"<< "arrival_time:" << arrival_time << " ";
        cout<<"total_cpu_time:"<<total_cpu_time<< " ";
        cout<<"cpu_burst:"<<cpu_burst<< " ";
        cout<<"io_burst:"<<io_burst<< " ";
        cout<<"priority:"<<process->priority<< " "<<endl;
    }
}

void runSimulation(){
    //while event queue is not empty
    while(!event_queue.empty()){
        Process* process = event_queue.top();
        event_queue.pop();
        current_time = process->event_time;
        if(process->state==STATE_CREATED || process->state==STATE_BLOCKED){
            process->previous_state = process->state;
            process->state = STATE_READY;
            my_scheduler->setEvent(process);
        }else if(process->state==STATE_RUNNING){
            process->previous_state = process->state;
            process->state = STATE_BLOCKED;
            process->io_burst = getrand(process->IO);
            process->event_time = current_time+process->io_burst;
            event_queue.push(process);
            if(!my_scheduler->isEmpty()){
                process = my_scheduler->getEvent();
                process->previous_state = process->state;
                process->state = STATE_RUNNING;
                process->cpu_burst = getrand(process->CB);
                process->event_time = current_time + process->cpu_burst;
                event_queue.push(process);
            }
        }else if(process->state==STATE_PREMPT){
            //do something
        }
    }
}



