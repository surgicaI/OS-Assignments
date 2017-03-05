#ifndef SIMULATION_H_INCLUDED
#define SIMULATION_H_INCLUDED

#include <iostream>
#include <unistd.h>
#include <stdio.h>
#include <fstream>
#include <queue>
#include <vector>
#include <iomanip>
#include "util.h"
#include "scheduler.h"

using namespace std;

//Method declarations
void initRandomVals(string);
int getrand(int);
void initSimulation(string);
void createProcess(int,int,int,int);
void runSimulation();
void printStatistics();

#endif
