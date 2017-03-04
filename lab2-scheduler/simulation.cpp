#include "simulation.h"

/*-------------------------------------------------------
Variable declarations
---------------------------------------------------------*/
bool verbose_output = false;
const bool LOGS_ENABLED = false;
Scheduler* my_scheduler;
priority_queue<Process*, vector<Process*>, ProcessComparator> event_queue;
int randvals_size = 0;
int* randvals;
int ofs = 0; //offset for random values 
int process_id = 0;

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

    init_random_vals(argv[optind+1]);
    init_simulation(argv[optind]);
    return 0;
}

/*-------------------------------------------------------
Method definitions
---------------------------------------------------------*/
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

void init_simulation(string input_file){
    int arrival_time = 0;
    int total_cpu_time = 0;
    int cpu_burst = 0;
    int io_burst = 0;
    ifstream fin(input_file);
    while(!fin.eof()){
        fin >> arrival_time >> total_cpu_time >> cpu_burst >> io_burst;
        createProcess(arrival_time,total_cpu_time,cpu_burst,io_burst);
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
    event_queue.push(process);
}

