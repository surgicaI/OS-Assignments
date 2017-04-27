#include <iostream>
#include <unistd.h>
#include <queue>
#include <stack>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <list>
#include <algorithm>
#include <vector>
using namespace std;

/*-------------------------------------------------------
Variable declarations
---------------------------------------------------------*/
const bool LOGS_ENABLED = false;
bool verbose_output = false;
int head_position = 0;
int num_operations = 0;

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
    list<IOEvent*> mylist;
public:
    static bool eventComp(const IOEvent* lhs, const IOEvent* rhs) {
        return abs(lhs->tracknum-head_position) < abs(rhs->tracknum-head_position);
    }
    IOEvent* getEvent(){
        list<IOEvent*>::iterator it = min_element(mylist.begin(), mylist.end(), eventComp);
        IOEvent *event = *it;
        mylist.erase(it);
        return event;
    }
    void setEvent(IOEvent* e){
        mylist.push_back(e);
    }
    bool empty(){
        return mylist.empty();
    }
};

class SCAN: public Algorithm{
private:
    vector<IOEvent*> mylist;
    int up;
public:
    SCAN(){
        up = 1;
    }
    IOEvent* getEvent(){
        IOEvent *event = NULL;
        int position = 0;
        for(int i=0;i<mylist.size();i++){
            int condition = (mylist[i]->tracknum - head_position) * up;
            if(condition >= 0){
                if(event==NULL){
                    event = mylist[i];
                    position = i;
                }else{
                    int curr = (mylist[i]->tracknum - head_position) * up;
                    int prev = (event->tracknum - head_position) * up;
                    if(curr<prev){
                        event = mylist[i];
                        position = i;
                    }
                }
            }
        }
        if(event==NULL){
            up *= -1;
            return this->getEvent();
        }
        mylist.erase(mylist.begin()+position);
        return event;
    }
    void setEvent(IOEvent* e){
        mylist.push_back(e);
    }
    bool empty(){
        return mylist.empty();
    }
};

class CSCAN: public Algorithm{
private:
    vector<IOEvent*> mylist;
    int reset;
public:
    CSCAN(){
        reset = 1;
    }
    IOEvent* getEvent(){
        IOEvent *event = NULL;
        int position = 0;
        for(int i=0;i<mylist.size();i++){
            if(mylist[i]->tracknum - (head_position*reset) >= 0){
                if(event==NULL){
                    event = mylist[i];
                    position = i;
                }else{
                    int curr = mylist[i]->tracknum - (head_position*reset);
                    int prev = event->tracknum - (head_position*reset);
                    if(curr<prev){
                        event = mylist[i];
                        position = i;
                    }
                }
            }
        }
        if(event==NULL){
            reset = 0;
            return this->getEvent();
        }
        mylist.erase(mylist.begin()+position);
        reset = 1;
        return event;
    }
    void setEvent(IOEvent* e){
        mylist.push_back(e);
    }
    bool empty(){
        return mylist.empty();
    }
};

class FSCAN: public Algorithm{
private:
    vector<IOEvent*> *my_active_list;
    vector<IOEvent*> *my_inactive_list;
    int up;
public:
    FSCAN(){
        my_active_list = new vector<IOEvent*>();
        my_inactive_list = new vector<IOEvent*>();
        up = 1;
    }
    IOEvent* getEvent(){
        if(my_active_list->empty()){
            vector<IOEvent*> *temp = my_active_list;
            my_active_list = my_inactive_list;
            my_inactive_list = temp;
            up = 1;
        }
        IOEvent *event = NULL;
        int position = 0;
        for(int i=0;i< my_active_list->size();i++){
            int condition = (my_active_list->at(i)->tracknum - head_position) * up;
            if(condition >= 0){
                if(event==NULL){
                    event = my_active_list->at(i);
                    position = i;
                }else{
                    int curr = ( my_active_list->at(i)->tracknum - head_position) * up;
                    int prev = (event->tracknum - head_position) * up;
                    if(curr<prev){
                        event = my_active_list->at(i);
                        position = i;
                    }
                }
            }
        }
        if(event==NULL){
            up *= -1;
            return this->getEvent();
        }
        my_active_list->erase(my_active_list->begin()+position);
        return event;
    }
    void setEvent(IOEvent* e){
        my_inactive_list->push_back(e);
    }
    bool empty(){
        return my_active_list->empty() && my_inactive_list->empty();
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
        num_operations++;
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
    statistics.avg_turnaround /= num_operations;
    statistics.avg_waittime /= num_operations;
}

void issueDiscRequest(IOEvent *io_event){
    int track_requested = io_event->tracknum;
    int movement = abs(head_position - io_event->tracknum);
    int wait_time = statistics.curr_time - io_event->time_step;
    statistics.avg_waittime += wait_time;
    statistics.max_waittime = max(statistics.max_waittime, wait_time);

    if(verbose_output){
        cout << left << setfill(' ') << setw(8) << to_string(statistics.curr_time)+":";
        cout << io_event->eventid << " issue ";
        cout << io_event->tracknum << " " << head_position <<endl;
    }

    event_in_process = io_event;
    event_in_process->finish_time = statistics.curr_time + movement;

    statistics.tot_movement += movement ;
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
    statistics.avg_turnaround += event_in_process->finish_time - event_in_process->time_step;
    delete event_in_process;
    event_in_process = NULL;
}

