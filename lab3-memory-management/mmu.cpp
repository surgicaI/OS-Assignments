#include "mmu.h"

/*-------------------------------------------------------
Variable declarations
---------------------------------------------------------*/


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
    int num_frames = -1;
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

    //analysing non-option arguments
    if(optind+1>=argc){
        cout << "'Input file' or 'random file' not provided" <<endl;
        return 0;
    }

    input_file =  argv[optind];
    random_file = argv[optind+1];

    cout << algo << endl;
    cout << options << endl;
    cout << num_frames << endl;
    cout << input_file << endl;
    cout << random_file << endl;

    return 0;
}

/*-------------------------------------------------------
Method definitions
Description
---------------------------------------------------------*/


