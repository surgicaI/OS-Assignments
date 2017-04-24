#include <iostream>
#include <unistd.h>
#include <queue>
#include <stack>
using namespace std;

/*-------------------------------------------------------
Variable declarations
---------------------------------------------------------*/
const bool LOGS_ENABLED = false;



/*-------------------------------------------------------
Method declarations
---------------------------------------------------------*/



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
    FIFO();
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
    SSTF();
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
    SCAN();
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
    CSCAN();
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
    FSCAN();
    IOEvent getEvent(int trackloc){
        return IOEvent();
    }
    void setEvent(IOEvent e){

    }
};


/*-------------------------------------------------------
Object declarations
---------------------------------------------------------*/



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

    return 0;
}



/*-------------------------------------------------------
Method definitions
---------------------------------------------------------*/


