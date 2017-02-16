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
    map<string, int> symbolsUsedMultipleTimes;

    //returns false if symbol is already present and true otherwise
    public:
    bool addSymbol(string symbol, int value){
        if(symbols.find(symbol) != symbols.end()){
            if(LOGS_ENABLED)
                cout<<"Symbol already present in symbol table"<<endl;
            symbolsUsedMultipleTimes[symbol] = value;
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
            cout << it->first << "=" << it->second;
            if(symbolsUsedMultipleTimes.find(it->first) != symbolsUsedMultipleTimes.end())
                cout << " Error: This variable is multiple times defined; first value used" << endl;
            else
                cout << endl;
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
void parseUseList();
void parseToken();
vector<string> parseUseListPass2();
void parseTokenPass2(vector<string>&);
void parseProgramText();
void parseInstruction();
void parseProgramTextPass2(vector<string>&);
void parseInstructionPass2(vector<string>&);
void updateModuleCount();
void updateInstructionAndAddToMemoryMap(string,int,vector<string>&);
void printParseError(int);
void readChar(char &);

//variables
int numberOfModules = 0;
int currentModuleOffset = 0;
int currentModule = 0;
vector<int> moduleOffsets;
SymbolTable mSymbolTable;
MemoryMap mMemoryMap;
ifstream fin;
int linenum = 1;
int offset =1;
int lastKnowCharPosition[2];

//error codes
static int NUM_EXPECTED = 0;              // Number expect
static int SYM_EXPECTED = 1;              // Symbol Expected
static int ADDR_EXPECTED = 2;             // Addressing Expected which is A/E/I/R
static int SYM_TOO_LONG = 3;              // Symbol Name is too long
static int TO_MANY_DEF_IN_MODULE = 4;     // > 16
static int TO_MANY_USE_IN_MODULE = 5;     // > 16
static int TO_MANY_INSTR = 6;             // total num_instr exceeds memory size (512)

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
    linenum =1;
    offset = 0;
    while(!fin.eof()){
        //parsing inpput file
        parseDefinitions();
        if(fin.eof()) break;
        parseUseList();
        parseProgramText();
    }
    fin.close();
}

void parseDefinitions(){
    int noOfDefinitions = -1;
    char c = ' ';
    while ((c==' ' || c=='\n' || c== '\t') && !fin.eof()) readChar(c);
    if(!fin.eof()){
        //so that noOfDefinitions variable is 0 only when there are still modules left in file to parse
        noOfDefinitions = 0;
        updateModuleCount();
    }
    while ((c!=' ' && c!='\n' && c!= '\t') && !fin.eof()){
        if(c<'0' || c>'9'){
            //cout << "Parse Error, unexpected symbol encountered" <<endl;
            printParseError(NUM_EXPECTED);
        }
        noOfDefinitions = noOfDefinitions*10 + (c-'0');
        readChar(c);
    }
    if(fin.eof() && noOfDefinitions==0){
        //cout << "Parse Error,  use_list expected" <<endl;
        linenum = lastKnowCharPosition[0];
        offset = lastKnowCharPosition[1] + 1;
        printParseError(NUM_EXPECTED);
    }
    if(noOfDefinitions>16){
        printParseError(TO_MANY_DEF_IN_MODULE);
    }
    for(int i=0;i<noOfDefinitions;i++)
        parseSingleDefinition();
    if(LOGS_ENABLED)
        cout<<"Number of Definitions:"<< noOfDefinitions <<endl;
}

void parseSingleDefinition(){
    string symbol = "";
    int value = 0;
    char c = ' ';
    while ((c==' ' || c=='\n' || c== '\t') && !fin.eof()) readChar(c);
    //Symbol not starting from alpha, or symbol missing
    if(((c<'a' || c>'z') && (c<'A' || c>'Z')) || fin.eof()){
        //cout << "Parse Error, symbol expected" <<endl;
        printParseError(SYM_EXPECTED);
    }
    while ((c!=' ' && c!='\n' && c!= '\t') && !fin.eof()){
        symbol = symbol + c;
        if(symbol.length()>16){
            printParseError(SYM_TOO_LONG);
        }
        readChar(c);
    }
    while ((c==' ' || c=='\n' || c== '\t') && !fin.eof()) readChar(c);
    while ((c!=' ' && c!='\n' && c!= '\t') && !fin.eof()){
        if(c<'0' || c>'9'){
            //cout << "Parse Error, symbol value should be numeric" <<endl;
            printParseError(NUM_EXPECTED);
        }
        value = value*10 + (c-'0');
        readChar(c);
    }
    if(fin.eof()){
        //cout << "Parse Error, symbol value expected, or useList missing" <<endl;
        linenum = lastKnowCharPosition[0];
        offset = lastKnowCharPosition[1] + 1;
        printParseError(NUM_EXPECTED);
    }
    value += currentModuleOffset;
    mSymbolTable.addSymbol(symbol,value);
}

void parseUseList(){
    int noOfTokens = 0;
    char c = ' ';
    while ((c==' ' || c=='\n' || c== '\t') && !fin.eof()) readChar(c);
    while ((c!=' ' && c!='\n' && c!= '\t') && !fin.eof()){
        if(c<'0' || c>'9'){
            //cout << "Parse Error, unexpected symbol encountered" <<endl;
            printParseError(NUM_EXPECTED);
        }
        noOfTokens = noOfTokens*10 + (c-'0');
        readChar(c);
    }
    if(LOGS_ENABLED)
        cout<<"Number of tokens:"<< noOfTokens <<endl;
    if(fin.eof()){
        //cout << "Parse Error, missing token" <<endl;
        linenum = lastKnowCharPosition[0];
        offset = lastKnowCharPosition[1] + 1;
        printParseError(SYM_EXPECTED);
    }
    if(noOfTokens>16){
        printParseError(TO_MANY_USE_IN_MODULE);
    }
    for(int i=0;i<noOfTokens;i++)
        parseToken();
}

void parseToken(){
    string token = "";
    char c =' ';
    while ((c==' ' || c=='\n' || c== '\t') && !fin.eof()) readChar(c);
    //Symbol not starting from alpha
    if((c<'a' || c>'z') && (c<'A' || c>'Z')){
        //cout << "Parse Error, symbol expected" <<endl;
        printParseError(SYM_EXPECTED);
    }
    while ((c!=' ' && c!='\n' && c!= '\t') && !fin.eof()){
        token = token + c;
        if(token.length()>16){
            printParseError(SYM_TOO_LONG);
        }
        readChar(c);
    }
    if(fin.eof()){
        //cout << "Parse Error, missing token in use list" <<endl;
        linenum = lastKnowCharPosition[0];
        offset = lastKnowCharPosition[1] + 1;
        printParseError(SYM_EXPECTED);
    }
    if(LOGS_ENABLED)
        cout<<"use list token:"<<token<<endl;
}

void parseProgramText(){
    int noOfInstructions = 0;
    char c = ' ';
    while ((c==' ' || c=='\n' || c== '\t') && !fin.eof()) readChar(c);
    if(fin.eof()){
        //cout << "Parse Error, missing program text" <<endl;
        linenum = lastKnowCharPosition[0];
        offset = lastKnowCharPosition[1] + 1;
        printParseError(NUM_EXPECTED);
    }
    while ((c!=' ' && c!='\n' && c!= '\t') && !fin.eof()){
        if(c<'0' || c>'9'){
            //cout << "Parse Error, unexpected symbol encountered" <<endl;
            printParseError(NUM_EXPECTED);
        }
        noOfInstructions = noOfInstructions*10 + (c-'0');
        readChar(c);
    }
    currentModuleOffset += noOfInstructions;
    for(int i=0;i<noOfInstructions;i++)
        parseInstruction();
    if(LOGS_ENABLED)
        cout<<"Number of instructions:"<< noOfInstructions <<endl;
}

void parseInstruction(){
    string instructionType = "";
    int instruction = 0;
    int instructionLength = 0;
    char c = ' ';
    while ((c==' ' || c=='\n' || c== '\t') && !fin.eof()) readChar(c);
    if(fin.eof()){
        //cout << "Parse Error, missing instruction Type" <<endl;
        linenum = lastKnowCharPosition[0];
        offset = lastKnowCharPosition[1] + 1;
        printParseError(ADDR_EXPECTED);
    }
    while ((c!=' ' && c!='\n' && c!= '\t') && !fin.eof()){
        if((instructionType.length()==0) && (c=='I' || c=='A' || c=='R' || c=='E')){
            instructionType = instructionType + c;
            readChar(c);
        }else{
            //cout << "Parse Error, invalid instruction type" <<endl;
            printParseError(ADDR_EXPECTED);
        }
    }
    while ((c==' ' || c=='\n' || c== '\t') && !fin.eof()) readChar(c);
    if(fin.eof()){
        //cout << "Parse Error, missing opcode" <<endl;
        linenum = lastKnowCharPosition[0];
        offset = lastKnowCharPosition[1] + 1;
        printParseError(NUM_EXPECTED);
    }
    while ((c!=' ' && c!='\n' && c!= '\t') && !fin.eof()){
        if(c<'0' || c>'9'){
            //cout << "Parse Error, Invalid opcode" <<endl;
            printParseError(NUM_EXPECTED);
        }else if(instructionLength >= 4){
            cout << "Invalid opcode, length is more than 4 digits" <<endl;
        }
        instruction = instruction*10 + (c-'0');
        instructionLength++;
        readChar(c);
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
    linenum = 1;
    offset = 0;
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
    while ((c==' ' || c=='\n' || c== '\t') && !fin.eof()) readChar(c);
    while ((c!=' ' && c!='\n' && c!= '\t') && !fin.eof()){
        noOfDefinitions = noOfDefinitions*10 + (c-'0');
        readChar(c);
    }
    for(int i=0;i<noOfDefinitions;i++)
    parseSingleDefinitionPass2();
}

void parseSingleDefinitionPass2(){
    char c = ' ';
    while ((c==' ' || c=='\n' || c== '\t') && !fin.eof()) readChar(c);
    while ((c!=' ' && c!='\n' && c!= '\t') && !fin.eof()) readChar(c);
    while ((c==' ' || c=='\n' || c== '\t') && !fin.eof()) readChar(c);
    while ((c!=' ' && c!='\n' && c!= '\t') && !fin.eof()) readChar(c);
}

vector<string> parseUseListPass2(){
    int noOfTokens = 0;
    int token = 0;
    vector<string> tokens;
    char c = ' ';
    while ((c==' ' || c=='\n' || c== '\t') && !fin.eof()) readChar(c);
    while ((c!=' ' && c!='\n' && c!= '\t') && !fin.eof()){
        noOfTokens = noOfTokens*10 + (c-'0');
        readChar(c);
    }
    for(int i=0;i<noOfTokens;i++)
    parseTokenPass2(tokens);
    
    return tokens;
}

void parseTokenPass2(vector<string>& tokens){
    string token = "";
    char c =' ';
    while ((c==' ' || c=='\n' || c== '\t') && !fin.eof()) readChar(c);
    while ((c!=' ' && c!='\n' && c!= '\t') && !fin.eof()){
        token = token + c;
        readChar(c);
    }
    if(LOGS_ENABLED)
        cout<<token<<endl;
    tokens.insert(tokens.end(),token);
}

void parseProgramTextPass2(vector<string>& tokens){
    int noOfInstructions = 0;
    char c = ' ';
    while ((c==' ' || c=='\n' || c== '\t') && !fin.eof()) readChar(c);
    while ((c!=' ' && c!='\n' && c!= '\t') && !fin.eof()){
        noOfInstructions = noOfInstructions*10 + (c-'0');
        readChar(c);
    }
    for(int i=0;i<noOfInstructions;i++)
        parseInstructionPass2(tokens);
}

void parseInstructionPass2(vector<string>& tokens){
    string instructionType = "";
    int instruction = 0;
    char c = ' ';
    while ((c==' ' || c=='\n' || c== '\t') && !fin.eof()) readChar(c);
    while ((c!=' ' && c!='\n' && c!= '\t') && !fin.eof()){
        instructionType = instructionType + c;
        readChar(c);
    }
    while ((c==' ' || c=='\n' || c== '\t') && !fin.eof()) readChar(c);
    while ((c!=' ' && c!='\n' && c!= '\t') && !fin.eof()){
        instruction = instruction*10 + (c-'0');
        readChar(c);
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

void printParseError(int errcode) {
    static string errstr[] = {
        "NUM_EXPECTED",              // Number expect
        "SYM_EXPECTED",              // Symbol Expected
        "ADDR_EXPECTED",             // Addressing Expected which is A/E/I/R
        "SYM_TOO_LONG",              // Symbol Name is too long
        "TO_MANY_DEF_IN_MODULE",     // > 16
        "TO_MANY_USE_IN_MODULE",     // > 16
        "TO_MANY_INSTR",             // total num_instr exceeds memory size (512)
    };
    cout << "Parse Error line " + to_string(linenum) + " offset " + to_string(offset) + ": " <<errstr[errcode]<<endl;
    exit(0);
}

void readChar(char &c){
    fin.get(c);
    if(c=='\n'){
        linenum++;
        offset=0;
    }else{
        offset++;
        if(c!=' ' && c!='\t'){
            lastKnowCharPosition[0] = linenum;
            lastKnowCharPosition[1] = offset;
        }
    }
}