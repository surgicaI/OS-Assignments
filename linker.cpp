#include <iostream>
#include <unordered_map>
#include <fstream>
#include <vector>
using namespace std;

//Classes definition
class SymbolTable{
    unordered_map<string, int> symbols;

    //returns false if symbol is already present and true otherwise
    public:
    bool addSymbol(string symbol, int value){
        if(symbols.find(symbol) != symbols.end()){
            //Symbol already present in symbol table
            return false;
        }
        symbols[symbol] = value;
        return true;
    }

    //prints the symbol table in formatting, given in requirements
    void print(){
        cout<<"Symbol Table"<<endl;
        unordered_map<string,int>::iterator it;
        for (it = symbols.begin(); it != symbols.end(); ++it ){
            cout << it->first << "=" << it->second<<endl;
        }
    }
};

//method declarations
void pass_1(string);
void pass_2(string);
void parseDefinitions();
vector<int> parseUseList();
void parseProgramText(vector<int>&);

//variables
int numberOfModules = 0;
vector<int> moduleOffsets;
SymbolTable mSymbolTable;
ifstream fin;

int main ( int argc, char *argv[] )
{
    if (argc == 2){
       string fileName = argv[1];
       pass_1(fileName);
       //pass_2(fileName);
    }
    else
        cout << "Please provide the filename in arguments"<<endl;
    return 0;
}

//Method definitions
void pass_1(string filename){
    fin.open(filename);
    while(!fin.eof()){
        numberOfModules++;
        parseDefinitions();
        mSymbolTable.print();
        vector<int> tokens = parseUseList();
        parseProgramText(tokens);
    }
    fin.close();
}

void parseDefinitions(){
    int noOfDefinitions = 0;
    string symbol = "";
    int value = 0;
    fin >> noOfDefinitions;
    for(int i=0;i<noOfDefinitions;i++){
        fin >> symbol >> value;
        mSymbolTable.addSymbol(symbol,value);
    }

}

vector<int> parseUseList(){
    int noOfTokens = 0;
    int token = 0;
    vector<int> tokens;
    fin >> noOfTokens;
    for(int i=0;i<noOfTokens;i++){
        fin>>token;
        tokens.insert(tokens.end(),token);
    }
    return tokens;
}

void parseProgramText(vector<int>& tokens){
    int noOfInstructions = 0;
    string instructionType = "";
    int instruction = 0;
    fin>>noOfInstructions;
    for(int i=0;i<noOfInstructions;i++){
        fin >> instructionType >> instruction;
        cout << instructionType << " " << instruction <<endl;
    }
}