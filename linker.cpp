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
void parseSingleDefinition();
vector<string> parseUseList();
void parseToken(vector<string>&);
void parseProgramText(vector<string>&);

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
        vector<string> tokens = parseUseList();
        parseProgramText(tokens);
    }
    mSymbolTable.print();
    fin.close();
}

void parseDefinitions(){
    int noOfDefinitions = 0;
    char c = ' ';
    while ((c==' ' || c=='\n' || c== '\t') && !fin.eof()) fin.get(c);
    while ((c!=' ' && c!='\n' && c!= '\t') && !fin.eof()){
        noOfDefinitions = noOfDefinitions*10 + (c-'0');
        fin.get(c);
    }
    for(int i=0;i<noOfDefinitions;i++)
    parseSingleDefinition();
}

void parseSingleDefinition(){
    string symbol = "";
    int value = 0;
    char c = ' ';
    while ((c==' ' || c=='\n' || c== '\t') && !fin.eof()) fin.get(c);
    while ((c!=' ' && c!='\n' && c!= '\t') && !fin.eof()){
        symbol = symbol + c;
        fin.get(c);
    }
    while ((c==' ' || c=='\n' || c== '\t') && !fin.eof()) fin.get(c);
    while ((c!=' ' && c!='\n' && c!= '\t') && !fin.eof()){
        value = value*10 + (c-'0');
        fin.get(c);
    }
    mSymbolTable.addSymbol(symbol,value);
}

vector<string> parseUseList(){
    int noOfTokens = 0;
    int token = 0;
    vector<string> tokens;
    char c = ' ';
    while ((c==' ' || c=='\n' || c== '\t') && !fin.eof()) fin.get(c);
    while ((c!=' ' && c!='\n' && c!= '\t') && !fin.eof()){
        noOfTokens = noOfTokens*10 + (c-'0');
        fin.get(c);
    }
    for(int i=0;i<noOfTokens;i++)
    parseToken(tokens);
    
    return tokens;
}

void parseToken(vector<string>& tokens){
    string token = "";
    char c =' ';
    while ((c==' ' || c=='\n' || c== '\t') && !fin.eof()) fin.get(c);
    while ((c!=' ' && c!='\n' && c!= '\t') && !fin.eof()){
        token = token + c;
        fin.get(c);
    }
    tokens.insert(tokens.end(),token);
}

void parseProgramText(vector<string>& tokens){
    int noOfInstructions = 0;
    string instructionType = "";
    int instruction = 0;
    fin>>noOfInstructions;
    for(int i=0;i<noOfInstructions;i++){
        fin >> instructionType >> instruction;
        cout << instructionType << " " << instruction <<endl;
    }
}