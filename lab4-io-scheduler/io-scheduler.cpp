#include <iostream>
#include <unistd.h>
#include <queue>
#include <stack>
#include <fstream>
#include <sstream>
#include <iomanip>
using namespace std;

/*-------------------------------------------------------
Variable declarations
---------------------------------------------------------*/
const bool LOGS_ENABLED = false;
bool verbose_output = false;
int head_position = 0;

/*-------------------------------------------------------
Class definitions
---------------------------------------------------------*/
class IOEvent{
public:
    int time_step, eventid, tracknum, finish_time;
    IOEvent(int id, int t, int track){
        eventid = id;
        time_step = t;
        tracknum = track;
        finish_time = -1;
    }
};

class Algorithm{
public:
    virtual IOEvent* getEvent() = 0;
    virtual void setEvent(IOEvent* e) = 0;
    virtual bool empty() = 0;
};

class FIFO: public Algorithm{
private:
    queue<IOEvent*> myqueue;
public:
    IOEvent* getEvent(){
        IOEvent *event = myqueue.front();
        myqueue.pop();
        return event;
    }
    void setEvent(IOEvent* e){
        myqueue.push(e);
    }
    bool empty(){
        return myqueue.empty();
    }
};

class SSTF: public Algorithm{
private:
    queue<IOEvent> myqueue;
public:
    SSTF(){

    }
    IOEvent* getEvent(){
        return NULL;
    }
    void setEvent(IOEvent* e){

    }
    bool empty(){
        return myqueue.empty();
    }
};

class SCAN: public Algorithm{
private:
    queue<IOEvent> myqueue;
public:
    SCAN(){

    }
    IOEvent* getEvent(){
        return NULL;
    }
    void setEvent(IOEvent* e){

    }
    bool empty(){
        return myqueue.empty();
    }
};

class CSCAN: public Algorithm{
private:
    queue<IOEvent> myqueue;
public:
    CSCAN(){

    }
    IOEvent* getEvent(){
        return NULL;
    }
    void setEvent(IOEvent* e){

    }
    bool empty(){
        return myqueue.empty();
    }
};

class FSCAN: public Algorithm{
private:
    queue<IOEvent> myqueue;
public:
    FSCAN(){

    }
    IOEvent* getEvent(){
        return NULL;
    }
    void setEvent(IOEvent* e){

    }
    bool empty(){
        return myqueue.empty();
    }
};

class Statistics{
public:
    int curr_time, tot_movement, max_waittime;
    double avg_turnaround, avg_waittime;

    Statistics(){
        curr_time = 0;
        tot_movement = 0;
        avg_turnaround = 0;
        avg_waittime = 0;
        max_waittime = 0;
    }

    void printSummary(){
        printf("SUM: %d %d %.2lf %.2lf %d\n", 
        curr_time, 
        tot_movement, 
        avg_turnaround, 
        avg_waittime,  
        max_waittime); 
    }
};

/*-------------------------------------------------------
Method declarations
---------------------------------------------------------*/
void initAlgorithm(string);
void parseInput(string);
void runSimulation();
void issueDiscRequest(IOEvent*);
void finishDiscRequest();

/*-------------------------------------------------------
Object declarations
---------------------------------------------------------*/
Algorithm *my_algorithm;
Statistics statistics;
queue<IOEvent*> io_events;
IOEvent *event_in_process = NULL;

/*-------------------------------------------------------
Main function
---------------------------------------------------------*/
int main(int argc, char* argv[]){
    //Reading input from console and initializing
    int c;
    string input_file = "";
    string algo = "";
    while ((c = getopt(argc, argv, "vs:")) != -1){
        switch (c){
            case 'v':
                verbose_output = true;
                break;
            case 's':
                algo = optarg;
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

    if(optind>=argc){
        cout << "Please provide input file." <<endl;
        return 0;
    }
    input_file =  argv[optind];

    if(LOGS_ENABLED){
        cout << "algo      :" << algo << endl;
        cout << "input file:" << input_file << endl;
        cout << "verbose   :" << verbose_output <<endl;
    }

    initAlgorithm(algo);
    parseInput(input_file);
    runSimulation();
    statistics.printSummary();

    return 0;
}

/*-------------------------------------------------------
Method definitions
---------------------------------------------------------*/
void initAlgorithm(string algo){
    if(algo=="i"){
        my_algorithm = new FIFO();
    }else if(algo=="j"){
        my_algorithm = new SSTF();
    }else if(algo=="s"){
        my_algorithm = new SCAN();
    }else if(algo=="c"){
        my_algorithm = new CSCAN();
    }else if(algo=="f"){
        my_algorithm = new FSCAN();
    }else{
        my_algorithm = new FIFO();
    }
}

void parseInput(string input_file){
    ifstream file(input_file);
    string str;
    int time_step = 0;
    int track = 0;
    int eventid = -1;
    while (getline(file, str))
    {
        //ignoring the lines with # in input file
        size_t found = str.find_first_of("#");
        if (found!=string::npos)
            continue;
        if(LOGS_ENABLED)
            cout<<str<<endl;

        stringstream stream(str);
        stream >> time_step;
        stream >> track;
        eventid++;

        io_events.push(new IOEvent(eventid, time_step, track));
    }
}

void runSimulation(){
    if(verbose_output)
        cout<< "TRACE" <<endl;
    while(true){
        if(!io_events.empty() && statistics.curr_time == io_events.front()->time_step){
            if(verbose_output){
                cout << left << setfill(' ') << setw(8) << to_string(io_events.front()->time_step)+":";
                cout << io_events.front()->eventid << " add ";
                cout << io_events.front()->tracknum <<endl;
            }
            my_algorithm->setEvent(io_events.front());
            io_events.pop();
        }
        if(event_in_process == NULL && !my_algorithm->empty()){
            issueDiscRequest(my_algorithm->getEvent());
        }

        if(event_in_process && event_in_process->finish_time==statistics.curr_time)
            finishDiscRequest();
        else
            statistics.curr_time++;
            
        if(io_events.empty() && my_algorithm->empty() && !event_in_process){
            break;
        }
    }
}

void issueDiscRequest(IOEvent *io_event){
    int track_requested = io_event->tracknum;

    if(verbose_output){
        cout << left << setfill(' ') << setw(8) << to_string(statistics.curr_time)+":";
        cout << io_event->eventid << " issue ";
        cout << io_event->tracknum << " " << head_position <<endl;
    }

    event_in_process = io_event;
    event_in_process->finish_time = statistics.curr_time + 
                    abs(head_position - event_in_process->tracknum);
    head_position = event_in_process->tracknum;
    if(LOGS_ENABLED){
        cout<<"finish time:" << event_in_process->finish_time<<endl;
        cout<<"head_position:"<<head_position<<"track:"<<event_in_process->tracknum <<endl;
    }
}

void finishDiscRequest(){
    if(verbose_output){
        cout << left << setfill(' ') << setw(8) << to_string(event_in_process->finish_time)+":";
        cout << event_in_process->eventid << " finish ";
        cout << event_in_process->finish_time - event_in_process->time_step <<endl;
    }
    delete event_in_process;
    event_in_process = NULL;
}

