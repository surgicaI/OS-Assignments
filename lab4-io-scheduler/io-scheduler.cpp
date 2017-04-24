#include <iostream>
#include <unistd.h>
#include <queue>
#include <stack>
#include <fstream>
#include <sstream>
using namespace std;

/*-------------------------------------------------------
Variable declarations
---------------------------------------------------------*/
const bool LOGS_ENABLED = false;



/*-------------------------------------------------------
Method declarations
---------------------------------------------------------*/
void initAlgorithm(string);
void initSimulation(string);
void printSummary();

/*-------------------------------------------------------
Class definitions
---------------------------------------------------------*/
class IOEvent{
    int eventid;
    int tracknum;
};

class Algorithm{
public:
    virtual IOEvent getEvent(int trackloc) = 0;
    virtual void setEvent(IOEvent e) = 0;
};

class FIFO: public Algorithm{
private:
    queue<IOEvent> myqueue;
public:
    FIFO(){

    }
    IOEvent getEvent(int trackloc){
        return IOEvent();
    }
    void setEvent(IOEvent e){

    }
};

class SSTF: public Algorithm{
private:
    queue<IOEvent> myqueue;
public:
    SSTF(){

    }
    IOEvent getEvent(int trackloc){
        return IOEvent();
    }
    void setEvent(IOEvent e){

    }
};

class SCAN: public Algorithm{
private:
    queue<IOEvent> myqueue;
public:
    SCAN(){

    }
    IOEvent getEvent(int trackloc){
        return IOEvent();
    }
    void setEvent(IOEvent e){

    }
};

class CSCAN: public Algorithm{
private:
    queue<IOEvent> myqueue;
public:
    CSCAN(){

    }
    IOEvent getEvent(int trackloc){
        return IOEvent();
    }
    void setEvent(IOEvent e){

    }
};

class FSCAN: public Algorithm{
private:
    queue<IOEvent> myqueue;
public:
    FSCAN(){

    }
    IOEvent getEvent(int trackloc){
        return IOEvent();
    }
    void setEvent(IOEvent e){

    }
};


/*-------------------------------------------------------
Object declarations
---------------------------------------------------------*/
Algorithm *my_algorithm;


/*-------------------------------------------------------
Main function
---------------------------------------------------------*/
int main(int argc, char* argv[]){
    //Reading input from console and initializing
    int c;
    string input_file = "";
    string algo = "";
    while ((c = getopt(argc, argv, "s:")) != -1){
        switch (c){
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
    }

    initAlgorithm(algo);
    initSimulation(input_file);

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

void initSimulation(string input_file){
    ifstream file(input_file);
    string str;
    int time_step = 0;
    int track = 0;
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


    }
}

void printSummary(){
    int total_time = 0;
    int tot_movement = 0;
    double avg_turnaround = 0;
    double avg_waittime = 0;
    int max_waittime = 0;
    printf("SUM: %d %d %.2lf %.2lf %d\n", 
        total_time, 
        tot_movement, 
        avg_turnaround, 
        avg_waittime,  
        max_waittime); 
}