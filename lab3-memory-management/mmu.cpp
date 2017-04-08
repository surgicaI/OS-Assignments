#include <iostream>
#include <unistd.h>
#include <fstream>
#include <sstream>

using namespace std;

/*-------------------------------------------------------
Class definitions
---------------------------------------------------------*/
class PageTableEntry{
    unsigned present    :   1;
    unsigned modified   :   1;
    unsigned referenced :   1;
    unsigned pagedout   :   1;
    unsigned frameidx   :   6;
};

class Algorithm{
public:
    virtual int selectFrame(PageTableEntry &p) = 0;
    virtual void update(PageTableEntry &p) = 0;
};

class FIFO: public Algorithm{
public:
    FIFO(){
        //constructor
    }
    int selectFrame(PageTableEntry &p){
        return 0;
    }
    void update(PageTableEntry &p){

    }
private:
    int entries;
};

/*-------------------------------------------------------
Method declarations
---------------------------------------------------------*/
void initSimulation(string,string);

/*-------------------------------------------------------
Variable declarations
---------------------------------------------------------*/
const bool LOGS_ENABLED = false;
int num_frames = -1;
PageTableEntry page_table[64];

/*-------------------------------------------------------
Main function
---------------------------------------------------------*/
int main(int argc, char* argv[]){
    //Reading input from console and initializing
    int c;
    char *s_value = NULL;
    string input_file = "";
    string random_file = "";
    string options = "";
    string algo = "";
    while ((c = getopt(argc, argv, "a:o:f:")) != -1){
        switch (c){
            case 'a':
                algo = optarg;
                break;
            case 'o':
                options = optarg;
                break;
            case 'f':
                num_frames = stoi(optarg);
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

    if(optind+1>=argc){
        cout << "'Input file' or 'random file' not provided" <<endl;
        return 0;
    }
    input_file =  argv[optind];
    random_file = argv[optind+1];

    if(LOGS_ENABLED){
        cout << algo << endl;
        cout << options << endl;
        cout << num_frames << endl;
        cout << input_file << endl;
        cout << random_file << endl;
    }

    initSimulation(input_file,random_file);

    return 0;
}

/*-------------------------------------------------------
Method definitions
---------------------------------------------------------*/
void initSimulation(string input_file, string random_file){
    ifstream file(input_file);
    string str;
    int read_or_write;
    int virtual_page_index;
    while (getline(file, str))
    {
        //ignoring the lines with # in input file
        size_t found = str.find_first_of("#");
        if (found!=string::npos)
            continue;
        if(LOGS_ENABLED)
            cout<<str<<endl;

        stringstream stream(str);
        stream >> read_or_write;
        stream >> virtual_page_index;
        
        //cout << read_or_write << " " << virtual_page_index <<endl;

        
    }
}

