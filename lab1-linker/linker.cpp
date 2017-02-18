#include <iostream>
#include <map>
#include <fstream>
#include <vector>
#include <iomanip>
#include <utility>
#include <unordered_set>
using namespace std;

const bool LOGS_ENABLED = false;

//Classes definition
class SymbolTable{
    map<string, int> symbols;
    unordered_set<string> symbolsDefinedMultipleTimes;
    map<string, int> symbolsMap2; //To get info which symbols were defined and not used

    //returns false if symbol is already present and true otherwise
    public:
    bool addSymbol(string symbol, int value, int module){
        if(symbols.find(symbol) != symbols.end()){
            if(LOGS_ENABLED)
                cout<<"Symbol already present in symbol table"<<endl;
            symbolsDefinedMultipleTimes.insert(symbol);
            return false;
        }
        symbols[symbol] = value;
        symbolsMap2[symbol] = module;
        return true;
    }

    pair<int,bool> getSymbolGlobalAddress(string symbol){
        map<string,int>::const_iterator it = symbols.find(symbol);
        if(it != symbols.end()){
            symbolsMap2.erase(symbol);
            return make_pair(it->second,true);
        }
        if(LOGS_ENABLED)
            cout<<"Symbol not present in symbol table"<<endl;
        return make_pair(0,false);
    }

    //prints the symbol table in formatting, given in requirements
    void print(){
        cout<<"Symbol Table"<<endl;
        map<string,int>::iterator it;
        for (it = symbols.begin(); it != symbols.end(); ++it ){
            cout << it->first << "=" << it->second;
            if(symbolsDefinedMultipleTimes.find(it->first) != symbolsDefinedMultipleTimes.end())
                cout << " Error: This variable is multiple times defined; first value used" << endl;
            else
                cout << endl;
        }
        cout <<endl;
    }

    void printWarningForUnusedSymbols(){
        map<string,int>::iterator it;
        if(!symbolsMap2.empty()){
            cout << endl;
        }
        for (it = symbolsMap2.begin(); it != symbolsMap2.end(); ++it ){
            cout<< "Warning: Module "<<it->second<<": "<<it->first<<" was defined but never used" <<endl;
        }
    }

    void printWarningForTooBigSymbolValues(vector<int> &moduleSizeVector, vector<int> &moduleOffsetVector){
        map<string,int>::iterator it;
        string symbol = "";
        int relativeValue = 0;
        int moduleNumber = 0;
        int moduleSize = 0;
        for (it = symbols.begin(); it != symbols.end(); ++it ){
            symbol = it->first;
            moduleNumber = symbolsMap2[symbol];
            relativeValue = it->second - moduleOffsetVector[moduleNumber-1];
            moduleSize = moduleSizeVector[moduleNumber-1];
            if(relativeValue>=moduleSize){
                cout << "Warning: Module "<< moduleNumber <<": "<<symbol<<" too big ";
                cout << relativeValue << " (max="<< moduleSize-1 <<") assume zero relative" <<endl;
                symbols[symbol] = moduleOffsetVector[moduleNumber-1];
            }
        }
    }
};
class MemoryMap{
    vector<pair<int,string> > memoryMap;
    vector<map<string, int> > tokenMapVector;
    public:
        void addInstructionCode(int instructionCode,string errorMessage){
            pair<int,string> instructionCodePair = make_pair(instructionCode,errorMessage);
            memoryMap.insert(memoryMap.end(),instructionCodePair);
        }
        void addTokens(vector<string> tokens, int module){
            map<string, int> tokenMap;
            for (int i=0; i<tokens.size(); ++i)
                tokenMap[tokens[i]] = module+1;
            tokenMapVector.insert(tokenMapVector.end(),tokenMap);
        }
        void removeUsedToken(string token, int module){
            tokenMapVector.back().erase(token);
        }
        void print(int totalInstructions, vector<int> &moduleOffsetsVector){
            int currentInstructionNumber = 1;
            int moduleNumber = 1;
            cout<<"Memory Map"<<endl;
            for (vector<pair<int,string> >::iterator it=memoryMap.begin();it!= memoryMap.end();++it){
                cout << setfill('0') << setw(3) << currentInstructionNumber-1 << ": "<< setfill('0') << setw(4) << (*it).first << (*it).second <<endl;
                if(currentInstructionNumber==totalInstructions || (moduleNumber<moduleOffsetsVector.size() && currentInstructionNumber==moduleOffsetsVector[moduleNumber])){
                    for (map<string,int>::iterator iter = tokenMapVector[moduleNumber-1].begin(); iter != tokenMapVector[moduleNumber-1].end(); ++iter ){
                        cout << "Warning: Module "<<iter->second<<": "<<iter->first<<" appeared in the uselist but was not actually used"<<endl;
                    }
                    moduleNumber++;
                }
                currentInstructionNumber++;
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
void updateModuleSize();

//variables
int numberOfModules = 0;
int numberOfInstructionsParsed = 0;
int currentModule = 0;
vector<int> moduleOffsets;
vector<int> moduleSize;
SymbolTable mSymbolTable;
MemoryMap mMemoryMap;
ifstream fin;
int linenum = 1;
int offset =1;
const int SIZE_OF_MACHINE = 512;

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
       //Priting warnings if symbol value is greater than size of module
       mSymbolTable.printWarningForTooBigSymbolValues(moduleSize,moduleOffsets);
       //Printing Symbol Table
       mSymbolTable.print();
       //Second Pass
       pass_2(fileName);
       //Printing Memory Map
       mMemoryMap.print(numberOfInstructionsParsed,moduleOffsets);
       //Printing warnings for symbols which were defined but never used
       mSymbolTable.printWarningForUnusedSymbols();
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
    currentModule = 0;
    while(!fin.eof()){
        //parsing inpput file
        currentModule++;
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
    int definitionsStartOffset = offset;
    int definitionsStartLine = linenum;
    while ((c!=' ' && c!='\n' && c!= '\t') && !fin.eof()){
        if(c<'0' || c>'9'){
            //cout << "Parse Error, unexpected symbol encountered" <<endl;
            offset = definitionsStartOffset;
            linenum = definitionsStartLine;
            printParseError(NUM_EXPECTED);
        }
        noOfDefinitions = noOfDefinitions*10 + (c-'0');
        readChar(c);
    }
    if(fin.eof() && noOfDefinitions==0){
        //cout << "Parse Error,  use_list expected" <<endl;
        printParseError(NUM_EXPECTED);
    }
    if(noOfDefinitions>16){
        offset = definitionsStartOffset;
        linenum = definitionsStartLine;
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
        printParseError(NUM_EXPECTED);
    }
    value += numberOfInstructionsParsed;
    mSymbolTable.addSymbol(symbol,value,currentModule);
    if(LOGS_ENABLED){
        cout<<"Symbol:"<<symbol<<" value:"<<value<<endl;
    }
}

void parseUseList(){
    int noOfTokens = 0;
    char c = ' ';
    while ((c==' ' || c=='\n' || c== '\t') && !fin.eof()) readChar(c);
    int useStartOffset = offset;
    int useStartLine = linenum;
    while ((c!=' ' && c!='\n' && c!= '\t') && !fin.eof()){
        if(c<'0' || c>'9'){
            //cout << "Parse Error, unexpected symbol encountered" <<endl;
            offset = useStartOffset;
            linenum = useStartLine;
            printParseError(NUM_EXPECTED);
        }
        noOfTokens = noOfTokens*10 + (c-'0');
        readChar(c);
    }
    if(LOGS_ENABLED)
        cout<<"Number of tokens:"<< noOfTokens <<endl;
    if(fin.eof()){
        //cout << "Parse Error, missing token" <<endl;
        printParseError(SYM_EXPECTED);
    }
    if(noOfTokens>16){
        offset = useStartOffset;
        linenum = useStartLine;
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
        printParseError(NUM_EXPECTED);
    }
    int instructionsStartOffset = offset;
    int instructionsStartLine = linenum;
    while ((c!=' ' && c!='\n' && c!= '\t') && !fin.eof()){
        if(c<'0' || c>'9'){
            //cout << "Parse Error, unexpected symbol encountered" <<endl;
            printParseError(NUM_EXPECTED);
        }
        noOfInstructions = noOfInstructions*10 + (c-'0');
        readChar(c);
    }
    numberOfInstructionsParsed += noOfInstructions;
    if(numberOfInstructionsParsed>SIZE_OF_MACHINE){
        offset = instructionsStartOffset;
        linenum = instructionsStartLine;
        printParseError(TO_MANY_INSTR);
    }
    for(int i=0;i<noOfInstructions;i++)
        parseInstruction();
    if(LOGS_ENABLED)
        cout<<"Number of instructions:"<< noOfInstructions <<endl;
    updateModuleSize();
}

void parseInstruction(){
    string instructionType = "";
    int instruction = 0;
    int instructionLength = 0;
    char c = ' ';
    while ((c==' ' || c=='\n' || c== '\t') && !fin.eof()) readChar(c);
    if(fin.eof()){
        //cout << "Parse Error, missing instruction Type" <<endl;
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
        printParseError(NUM_EXPECTED);
    }
    while ((c!=' ' && c!='\n' && c!= '\t') && !fin.eof()){
        if(c<'0' || c>'9'){
            //cout << "Parse Error, Invalid opcode" <<endl;
            printParseError(NUM_EXPECTED);
        }
        instruction = instruction*10 + (c-'0');
        readChar(c);
    }
    if(LOGS_ENABLED)
        cout << instructionType << " " << instruction <<endl;
}

void updateModuleCount(){
    numberOfModules++;
    moduleOffsets.insert(moduleOffsets.end(),numberOfInstructionsParsed);
    if(LOGS_ENABLED)
        cout<<"Module:"<<numberOfModules<<" has offset of:"<<numberOfInstructionsParsed<<endl;     
}

void updateModuleSize(){
    int size = numberOfInstructionsParsed - moduleOffsets.back() ;
    moduleSize.insert(moduleSize.end(),size);
}

void pass_2(string filename){
    fin.open(filename);
    linenum = 1;
    offset = 0;
    currentModule = 0;
    while(!fin.eof()){
        //parsing inpput file second time
        parseDefinitionsPass2();
        vector<string> tokens = parseUseListPass2();
        mMemoryMap.addTokens(tokens,currentModule);
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
    string errorMessage = "";
    if(instructionType=="R"){
        if(instruction>9999){
            instruction = 9999;
            errorMessage = " Error: Illegal opcode; treated as 9999";
        }else{
            int relativeAddress = instruction%1000;
            if(relativeAddress>moduleSize[currentModule]){
                instruction = (instruction/1000)*1000;
                errorMessage = " Error: Relative address exceeds module size; zero used";
            }
            instruction += moduleOffsets[currentModule];
        }
    }else if(instructionType=="E"){
        int tokenIndex = instruction%1000;
        if(tokenIndex>=tokens.size()){
            errorMessage = " Error: External address exceeds length of uselist; treated as immediate";
        }else{
            pair<int,bool> opcodePair = mSymbolTable.getSymbolGlobalAddress(tokens[tokenIndex]);
            instruction = ((instruction/1000)*1000) + opcodePair.first;
            mMemoryMap.removeUsedToken(tokens[tokenIndex],currentModule);
            if(!opcodePair.second){
                errorMessage = " Error: " + tokens[tokenIndex] + " is not defined; zero used";
            }
        }
    }else if(instructionType=="A"){
        int absoluteAddress = instruction%1000;
        if(absoluteAddress>SIZE_OF_MACHINE){
            instruction = (instruction/1000)*1000;
            errorMessage = " Error: Absolute address exceeds machine size; zero used";
        }
    }else if(instructionType=="I"){
        if(instruction>9999){
            instruction = 9999;
            errorMessage = " Error: Illegal immediate value; treated as 9999";
        }
    }
    mMemoryMap.addInstructionCode(instruction,errorMessage);
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
    }
}