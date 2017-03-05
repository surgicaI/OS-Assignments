#include "simulation.h"

/*-------------------------------------------------------
Variable declarations
---------------------------------------------------------*/
Scheduler* my_scheduler;
priority_queue<Process*, vector<Process*>, ProcessComparator> event_queue;
priority_queue<Process*, vector<Process*>, FinishedProcessesComparator> finished_process_queue;
priority_queue<pair<int,int>, vector<pair<int,int> >, IOTimePairComparator> io_util_queue;
int randvals_size = 0;
int* randvals;
int ofs = 0; //offset for random values 
int process_id = 0;
int current_time = 0;
int insert_order = 0;
bool process_running_in_cpu = false;

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
    printStatistics();
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
    process->IT = 0;
    process->CW = 0;
    process->time_left = total_cpu_time;
    process->priority = getrand(4);
    process->event_time = arrival_time;
    process->state = STATE_CREATED;
    process->previous_state = "";
    process->state_start_time = arrival_time;
    process->insert_order = insert_order++;
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
    while(!event_queue.empty() || !my_scheduler->isEmpty()){
        Process* process = event_queue.top();
        event_queue.pop();
        current_time = process->event_time;
        int time_in_previous_state = current_time - process->state_start_time;
        if(process->state==STATE_CREATED || process->state==STATE_BLOCKED){
            if(process->state==STATE_BLOCKED){
                process->IT = process->IT + time_in_previous_state;
            }
            process->previous_state = process->state;
            process->state = STATE_READY;
            process->state_start_time = current_time;
            my_scheduler->setEvent(process);
            if(verbose_output){
                cout<<current_time<<" "<<process->id<<" "<<time_in_previous_state;
                cout<<": "<<process->previous_state<<" -> "<<process->state<<endl;
            }
            /*-------------------------------------------------------
            Rule: Processes with the same arrival time should be 
            entered into the run queue in the order of their
            occurrence in the input file
            Rule: You must process all events at a given time stamp 
            before invoking the scheduler/dispatcher
            ---------------------------------------------------------*/
            if(!event_queue.empty()){
                process = event_queue.top();
                if(process->event_time==current_time)
                    continue;
            }
        }else if(process->state==STATE_RUNNING){
            process->previous_state = process->state;
            process->time_left = process->time_left - process->cpu_burst;
            if(process->time_left>0){
                process->state = STATE_BLOCKED;
                process->io_burst = getrand(process->IO);
                process->event_time = current_time+process->io_burst;
                process->state_start_time = current_time;
                process->insert_order = insert_order++;
                event_queue.push(process);
                addIOTime(current_time,current_time+process->io_burst);
                if(verbose_output){
                    cout<<current_time<<" "<<process->id<<" "<<time_in_previous_state;
                    cout<<": "<<process->previous_state<<" -> "<<process->state;
                    cout<<" ib="<<process->io_burst<<" rem="<<process->time_left<<endl;
                }
            }else {
                process->FT = current_time;
                process->TT = process->FT - process->AT;
                finished_process_queue.push(process);
                if(verbose_output)
                    cout<<current_time<<" "<<process->id<<" "<<time_in_previous_state<<": Done"<<endl;
            }
            process_running_in_cpu = false;
            /*-------------------------------------------------------
            Rule: You must process all events at a given time stamp 
            before invoking the scheduler/dispatcher
            ---------------------------------------------------------*/
            if(!event_queue.empty()){
                process = event_queue.top();
                if(process->event_time==current_time)
                    continue;
            }
        }else if(process->state==STATE_PREMPT){
            //do something
        }
        //from STATE_READY to STATE_RUNNING
        if(!my_scheduler->isEmpty() && !process_running_in_cpu){
            process = my_scheduler->getEvent();
            process->previous_state = process->state;
            process->state = STATE_RUNNING;
            process->cpu_burst = getrand(process->CB);
            if(process->time_left<process->cpu_burst){
                process->cpu_burst = process->time_left;
            }
            process->event_time = current_time + process->cpu_burst;
            time_in_previous_state = current_time - process->state_start_time;
            process->CW = process->CW + time_in_previous_state;
            process->state_start_time = current_time;
            process->insert_order = insert_order++;
            event_queue.push(process);
            process_running_in_cpu=true;
            if(verbose_output){
                cout<<current_time<<" "<<process->id<<" "<<time_in_previous_state;
                cout<<": "<<process->previous_state<<" -> "<<process->state;
                cout<<" cb="<<process->cpu_burst<<" rem="<<process->time_left<<" prio="<<process->priority-1<<endl;
            }
        }
    }
}

void printStatistics(){
    cout << my_scheduler->getName()<<endl;
    Process* process;
    double number_of_process = 0;
    double cpu_util = 0;
    double io_util = calcIOUtil();
    double avg_turnaround = 0;
    double avg_waittime = 0.0;
    double throughput = 0;
    while(!finished_process_queue.empty()){
        process = finished_process_queue.top();
        finished_process_queue.pop();
        //calculations
        number_of_process++;
        cpu_util += process->TC;
        avg_turnaround += process->TT;
        avg_waittime += process->CW;
        //printing
        cout<<setfill('0') << setw(4) << process->id << ":";
        cout<<setfill(' ') << setw(5) << process->AT;
        cout<<setfill(' ') << setw(5) << process->TC;
        cout<<setfill(' ') << setw(5) << process->CB;
        cout<<setfill(' ') << setw(5) << process->IO;
        cout<<setfill(' ') << setw(2) << process->priority;
        cout<<" |";
        cout<<setfill(' ') << setw(6) << process->FT;
        cout<<setfill(' ') << setw(6) << process->TT;
        cout<<setfill(' ') << setw(6) << process->IT;
        cout<<setfill(' ') << setw(6) << process->CW;
        cout<<endl;
    }
    cpu_util = (cpu_util / current_time)*100;
    io_util = (io_util / current_time)*100;
    avg_turnaround = avg_turnaround/number_of_process;
    avg_waittime = avg_waittime/number_of_process;
    throughput = (number_of_process*100)/current_time;
    printf("SUM: %d %.2lf %.2lf %.2lf %.2lf %.3lf\n",
           current_time,
           cpu_util,
           io_util,
           avg_turnaround,
           avg_waittime, 
           throughput);
}

void addIOTime(int start_time,int end_time){
    pair<int,int> io_time = make_pair(start_time,end_time);
    io_util_queue.push(io_time);
}

int calcIOUtil(){
    int result = 0;
    while(!io_util_queue.empty()){
        pair<int,int> io_time = io_util_queue.top();
        io_util_queue.pop();
        while(!io_util_queue.empty()){
            pair<int,int> io_time_next = io_util_queue.top();
            if(io_time_next.first<=io_time.second){
                io_util_queue.pop();
                io_time.second = max(io_time.second,io_time_next.second);
            }else
                break;
        }
        result = result + io_time.second - io_time.first;
    }
    return result;
}




