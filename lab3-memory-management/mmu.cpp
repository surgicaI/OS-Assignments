#include <iostream>
#include <unistd.h>
#include <fstream>
#include <sstream>
#include <queue>
#include <vector>

using namespace std;

/*-------------------------------------------------------
Variable declarations
---------------------------------------------------------*/
const bool LOGS_ENABLED = false;

const int READ = 0;
const int WRITE = 1;
const int NUM_CLASSES = 4;
const int PAGE_TABLE_SIZE = 64;
int num_frames = -1;
long long int instruction_counter = 0;
int *frame_table;
bool option_O = false;
int randvals_size;
int *randvals;
int ofs = 0;
int page_replacement_request_id = 0;

/*-------------------------------------------------------
Method declarations
---------------------------------------------------------*/
void initSimulation(string,string);
void initAlgorithm(string,string);
void parseOptions(string);
void initRandomVals(string);
int getrand(int);

/*-------------------------------------------------------
Class definitions
---------------------------------------------------------*/
class PageTableEntry{
public:
    unsigned present    :   1;
    unsigned modified   :   1;
    unsigned referenced :   1;
    unsigned pagedout   :   1;
    unsigned frameidx   :   6;
    PageTableEntry(){
        present = 0;
        modified = 0;
        referenced = 0;
        pagedout = 0;
    }
} page_table[PAGE_TABLE_SIZE];

class Algorithm{
public:
    virtual int getFrame() = 0;
    virtual void update(int frame) = 0;
};

class FIFO: public Algorithm{
private:
    queue<int> myqueue;
public:
    FIFO(){
        for(int i=0;i<num_frames;i++){
            myqueue.push(i);
        }
    }
    int getFrame(){
        int frame = myqueue.front();
        if(!myqueue.empty())
            myqueue.pop();
        myqueue.push(frame);
        return frame;
    }
    void update(int frame){
        //do nothing
    }
};

class NRU: public Algorithm{
private:
    vector<int> my_classes[NUM_CLASSES];
    bool empty_frames_available;
public:
    NRU(){
        empty_frames_available = true;
    }
    int getFrame(){
        PageTableEntry *pte;
        int page_index = 0;
        int class_num = -1;
        for(int i=0;i<NUM_CLASSES;i++){
            my_classes[i].clear();
        }
        for(int frame=0;frame<num_frames;frame++){
            page_index = frame_table[frame];
            if(page_index==-1){
                //if frame is empty then return this frame;
                return frame;
            }
            pte = &page_table[page_index];
            if(pte->referenced==0){
                //when page reference bit is not set
                if(pte->modified==0){
                    //when page modified bit is not set
                    class_num = 0;
                }else{
                    //when page modified bit is set
                    class_num = 1;
                }
            }else{
                //when page reference bit is set
                if(pte->modified==0){
                    //when page modified bit is not set
                    class_num = 2;
                }else{
                    //when page modified bit is set
                    class_num = 3;
                }
            }
            my_classes[class_num].push_back(frame);
        }
        for(int i=0;i<NUM_CLASSES;i++){
            if(my_classes[i].size()==0)
                continue;
            int index = getrand(my_classes[i].size());
            return my_classes[i][index];
        }
        return -1;
    }
    void update(int frame){
        //do nothing
    }
};

class Random: public Algorithm{
private:
    bool empty_frames_available;
public:
    Random(){
        empty_frames_available = true;
    }
    int getFrame(){
        if(empty_frames_available){
            for(int frame=0;frame<num_frames;frame++){
                if(frame_table[frame]==-1)
                    return frame;
            }
            empty_frames_available = false;
        }
        return getrand(num_frames);
    }
    void update(int frame){
        //do nothing
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
    char *s_value = NULL;
    string input_file = "";
    string random_file = "";
    string algo = "";
    string options = "";
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
    frame_table = new int[num_frames];
    for(int i=0;i<num_frames;i++){
        frame_table[i] = -1;
    }
    initAlgorithm(algo, random_file);
    parseOptions(options);
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
    PageTableEntry *pte;
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
        if(option_O){
            cout<< "==> inst: "<< read_or_write << " " << virtual_page_index <<endl;
            pte = &page_table[virtual_page_index];
            if(pte->present!=1){
                int frame = my_algorithm->getFrame();
                //frame is not empty
                if(frame_table[frame]!=-1){
                    cout<< instruction_counter << ": UNMAP   "<< frame_table[frame] << "   " << frame <<endl;
                    PageTableEntry *old_pte = &page_table[frame_table[frame]];
                    old_pte->present = 0;
                    if(old_pte->modified==1){
                        cout<< instruction_counter << ": OUT    "<< frame_table[frame] << "   " << frame <<endl;
                        old_pte->pagedout = 1;
                        old_pte->modified = 0;
                    }
                }
                if(pte->pagedout==1){
                    cout<< instruction_counter << ": IN     "<< virtual_page_index << "   " << frame <<endl;
                }else{
                    cout<< instruction_counter << ": ZERO        "<< frame <<endl;
                }
                cout<< instruction_counter << ": MAP     "<< virtual_page_index << "   " << frame <<endl;
                pte->present = 1;
                pte->frameidx = frame;
                frame_table[frame] = virtual_page_index;
            }else{
                my_algorithm->update(pte->frameidx);
            }
            if(read_or_write==WRITE)
                pte->modified = 1;
            pte->referenced = 1;
        }

        instruction_counter++;
    }
}

void initAlgorithm(string algo, string random_file){
    if(algo=="f"){
        my_algorithm = new FIFO();
    }else if(algo=="N"){
        my_algorithm = new NRU();
        initRandomVals(random_file);
    }else if(algo=="r"){
        my_algorithm = new Random();
        initRandomVals(random_file);
    }
}

void parseOptions(string options){
    size_t found = options.find_first_of("O");
    if (found!=string::npos)
        option_O = true;
}

void initRandomVals(string random_file){
    ifstream fin(random_file);
    fin  >> randvals_size;
    randvals = new int[randvals_size];
    int index = 0, num = 0;
    while(index<randvals_size){
        fin>>num;
        randvals[index] = num;
        index++;
    }
}

int getrand(int n){
    int result = randvals[ofs] % n;
    if (++ofs >= randvals_size)
        ofs = 0;
    return result;
}
