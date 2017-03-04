#include <iostream>
#include <queue>
#include "util.h"
#include "scheduler.h"

using namespace std;

/*-------------------------------------------------------
Implementation of first come first serve scheduler
---------------------------------------------------------*/
Process* FCFS::getEvent(){
    if(!isEmpty()){
        Process *process;
        process = ready_queue.front();
        ready_queue.pop();
        return process;
    }
    return NULL;
}
void FCFS::setEvent(Process* p){
        ready_queue.push(p);
}
bool FCFS::isEmpty(){
    return ready_queue.empty();
}
string FCFS::getName(){
    return "FCFS";
}

/*-------------------------------------------------------
Implementation of last come first serve scheduler
---------------------------------------------------------*/
Process* LCFS::getEvent(){
    if(!isEmpty()){
        Process *process;
        process = ready_queue.front();
        ready_queue.pop();
        return process;
    }
    return NULL;
}
void LCFS::setEvent(Process* p){
        ready_queue.push(p);
}
bool LCFS::isEmpty(){
    return ready_queue.empty();
}
string LCFS::getName(){
    return "LCFS";
}

/*-------------------------------------------------------
Implementation of shortest Job first scheduler
---------------------------------------------------------*/
Process* SJF::getEvent(){
    if(!isEmpty()){
        Process *process;
        process = ready_queue.front();
        ready_queue.pop();
        return process;
    }
    return NULL;
}
void SJF::setEvent(Process* p){
        ready_queue.push(p);
}
bool SJF::isEmpty(){
    return ready_queue.empty();
}
string SJF::getName(){
    return "SJF";
}

/*-------------------------------------------------------
Implementation of Round Robin scheduler
---------------------------------------------------------*/
RR::RR(int n){
    quantum = n;
}
Process* RR::getEvent(){
    if(!isEmpty()){
        Process *process;
        process = ready_queue.front();
        ready_queue.pop();
        return process;
    }
    return NULL;
}
void RR::setEvent(Process* p){
    ready_queue.push(p);
}
bool RR::isEmpty(){
    return ready_queue.empty();
}
string RR::getName(){
    return "RR " + to_string(quantum);
}

/*-------------------------------------------------------
Implementation of priority based scheduler
---------------------------------------------------------*/
PRIO::PRIO(int n)
{
    quantum = n;
}
Process* PRIO::getEvent(){
    if(!isEmpty()){
        Process *process;
        process = ready_queue.front();
        ready_queue.pop();
        return process;
    }
    return NULL;
}
void PRIO::setEvent(Process* p){
    ready_queue.push(p);
}
bool PRIO::isEmpty(){
    return ready_queue.empty();
}
string PRIO::getName(){
    return "PRIO " + to_string(quantum);
}