#include <iostream>
#include <map>
#include <fstream>
#include <vector>
#include <iomanip>
using namespace std;

const bool LOGS_ENABLED = false;

//Classes definition
class SymbolTable{
    map<string, int> symbols;

    //returns false if symbol is already present and true otherwise
    public:
    bool addSymbol(string symbol, int value){
        if(symbols.find(symbol) != symbols.end()){
            if(LOGS_ENABLED)
                cout<<"Symbol already present in symbol table"<<endl;
            return false;
        }
        symbols[symbol] = value;
        return true;
    }

    int getSymbolGlobalAddress(string symbol){
        map<string,int>::const_iterator it = symbols.find(symbol);
        if(it != symbols.end()){
            return it->second;
        }
        if(LOGS_ENABLED)
            cout<<"Symbol not present in symbol table"<<endl;
        return 0;
    }

    //prints the symbol table in formatting, given in requirements
    void print(){
        cout<<"Symbol Table"<<endl;
        map<string,int>::iterator it;
        for (it = symbols.begin(); it != symbols.end(); ++it ){
            cout << it->first << "=" << it->second<<endl;
        }
        cout <<endl;
    }
};
class MemoryMap{
    vector<int> memoryMap;
    public:
        void addInstructionCode(int instructionCode){
            memoryMap.insert(memoryMap.end(),instructionCode);
        }
        void print(){
            int index = 0;
            cout<<"Memory Map"<<endl;
            for (vector<int>::iterator it=memoryMap.begin();it!= memoryMap.end();++it){
                cout << setfill('0') << setw(3) << index << ": " << *it<<endl;
                index++;
            }
        }
};

//method declarations
void pass_1(string);
void pass_2(string);
void parseDefinitions();
void parseSingleDefinition();
void parseDefinitionsPass2();
void parseSingleDefinitionPass2();
vector<string> parseUseList();
void parseToken(vector<string>&);
vector<string> parseUseListPass2();
void parseTokenPass2(vector<string>&);
void parseProgramText(vector<string>&);
void parseInstruction(vector<string>&);
void parseProgramTextPass2(vector<string>&);
void parseInstructionPass2(vector<string>&);
void updateModuleCount();
void updateInstructionAndAddToMemoryMap(string,int,vector<string>&);

//variables
int numberOfModules = 0;
int currentModuleOffset = 0;
int currentModule = 0;
vector<int> moduleOffsets;
SymbolTable mSymbolTable;
MemoryMap mMemoryMap;
ifstream fin;

int main ( int argc, char *argv[] )
{
    if (argc == 2){
       string fileName = argv[1];
       //First Pass
       pass_1(fileName);
       //Printing Symbol Table
       mSymbolTable.print();
       //Second Pass
       pass_2(fileName);
       //Printing Memory Map
       mMemoryMap.print();
    }
    else
        cout << "Please provide the filename in arguments"<<endl;
    return 0;
}

//Method definitions
void pass_1(string filename){
    fin.open(filename);
    while(!fin.eof()){
        //parsing inpput file
        parseDefinitions();
        vector<string> tokens = parseUseList();
        parseProgramText(tokens);
    }
    fin.close();
}

void parseDefinitions(){
    int noOfDefinitions = 0;
    char c = ' ';
    while ((c==' ' || c=='\n' || c== '\t') && !fin.eof()) fin.get(c);
    if(!fin.eof()){
        updateModuleCount();
    }
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
    value += currentModuleOffset;
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
    if(LOGS_ENABLED)
        cout<<token<<endl;
    tokens.insert(tokens.end(),token);
}

void parseProgramText(vector<string>& tokens){
    int noOfInstructions = 0;
    char c = ' ';
    while ((c==' ' || c=='\n' || c== '\t') && !fin.eof()) fin.get(c);
    while ((c!=' ' && c!='\n' && c!= '\t') && !fin.eof()){
        noOfInstructions = noOfInstructions*10 + (c-'0');
        fin.get(c);
    }
    currentModuleOffset += noOfInstructions;
    for(int i=0;i<noOfInstructions;i++)
        parseInstruction(tokens);
}

void parseInstruction(vector<string>& tokens){
    string instructionType = "";
    int instruction = 0;
    char c = ' ';
    while ((c==' ' || c=='\n' || c== '\t') && !fin.eof()) fin.get(c);
    while ((c!=' ' && c!='\n' && c!= '\t') && !fin.eof()){
        instructionType = instructionType + c;
        fin.get(c);
    }
    while ((c==' ' || c=='\n' || c== '\t') && !fin.eof()) fin.get(c);
    while ((c!=' ' && c!='\n' && c!= '\t') && !fin.eof()){
        instruction = instruction*10 + (c-'0');
        fin.get(c);
    }
    if(LOGS_ENABLED)
        cout << instructionType << " " << instruction <<endl;
}

void updateModuleCount(){
    numberOfModules++;
    moduleOffsets.insert(moduleOffsets.end(),currentModuleOffset);
    if(LOGS_ENABLED)
        cout<<"Module:"<<numberOfModules<<" has offset of:"<<currentModuleOffset<<endl;     
}

void pass_2(string filename){
    fin.open(filename);
    while(!fin.eof()){
        //parsing inpput file second time
        parseDefinitionsPass2();
        vector<string> tokens = parseUseListPass2();
        parseProgramTextPass2(tokens);
        currentModule++;
    }
    fin.close();
}

void parseDefinitionsPass2(){
    int noOfDefinitions = 0;
    char c = ' ';
    while ((c==' ' || c=='\n' || c== '\t') && !fin.eof()) fin.get(c);
    while ((c!=' ' && c!='\n' && c!= '\t') && !fin.eof()){
        noOfDefinitions = noOfDefinitions*10 + (c-'0');
        fin.get(c);
    }
    for(int i=0;i<noOfDefinitions;i++)
    parseSingleDefinitionPass2();
}

void parseSingleDefinitionPass2(){
    char c = ' ';
    while ((c==' ' || c=='\n' || c== '\t') && !fin.eof()) fin.get(c);
    while ((c!=' ' && c!='\n' && c!= '\t') && !fin.eof()) fin.get(c);
    while ((c==' ' || c=='\n' || c== '\t') && !fin.eof()) fin.get(c);
    while ((c!=' ' && c!='\n' && c!= '\t') && !fin.eof()) fin.get(c);
}

vector<string> parseUseListPass2(){
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
    parseTokenPass2(tokens);
    
    return tokens;
}

void parseTokenPass2(vector<string>& tokens){
    string token = "";
    char c =' ';
    while ((c==' ' || c=='\n' || c== '\t') && !fin.eof()) fin.get(c);
    while ((c!=' ' && c!='\n' && c!= '\t') && !fin.eof()){
        token = token + c;
        fin.get(c);
    }
    if(LOGS_ENABLED)
        cout<<token<<endl;
    tokens.insert(tokens.end(),token);
}

void parseProgramTextPass2(vector<string>& tokens){
    int noOfInstructions = 0;
    char c = ' ';
    while ((c==' ' || c=='\n' || c== '\t') && !fin.eof()) fin.get(c);
    while ((c!=' ' && c!='\n' && c!= '\t') && !fin.eof()){
        noOfInstructions = noOfInstructions*10 + (c-'0');
        fin.get(c);
    }
    for(int i=0;i<noOfInstructions;i++)
        parseInstructionPass2(tokens);
}

void parseInstructionPass2(vector<string>& tokens){
    string instructionType = "";
    int instruction = 0;
    char c = ' ';
    while ((c==' ' || c=='\n' || c== '\t') && !fin.eof()) fin.get(c);
    while ((c!=' ' && c!='\n' && c!= '\t') && !fin.eof()){
        instructionType = instructionType + c;
        fin.get(c);
    }
    while ((c==' ' || c=='\n' || c== '\t') && !fin.eof()) fin.get(c);
    while ((c!=' ' && c!='\n' && c!= '\t') && !fin.eof()){
        instruction = instruction*10 + (c-'0');
        fin.get(c);
    }
    updateInstructionAndAddToMemoryMap(instructionType,instruction,tokens);
    if(LOGS_ENABLED)
        cout << instructionType << " " << instruction <<endl;
}

void updateInstructionAndAddToMemoryMap(string instructionType,int instruction,vector<string>& tokens){
    if(instructionType=="R"){
        instruction += moduleOffsets[currentModule];
    }else if(instructionType=="E"){
        int tokenIndex = instruction%1000;
        instruction = ((instruction/1000)*1000) + mSymbolTable.getSymbolGlobalAddress(tokens[tokenIndex]);
    }
    mMemoryMap.addInstructionCode(instruction);
}