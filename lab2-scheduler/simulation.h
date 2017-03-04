#ifndef SIMULATION_H_INCLUDED
#define SIMULATION_H_INCLUDED

#include <iostream>
#include <unistd.h>
#include <stdio.h>
#include <fstream>
#include <queue>
#include <vector>
#include "util.h"
#include "scheduler.h"

using namespace std;

//Method declarations
void init_random_vals(string);
void init_simulation(string);
void createProcess(int,int,int,int);

#endif
