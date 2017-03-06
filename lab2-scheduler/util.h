#ifndef UTIL_H_INCLUDED
#define UTIL_H_INCLUDED

#include <iostream>
#include <utility>
using namespace std;

/*-------------------------------------------------------
Constants
---------------------------------------------------------*/
static bool verbose_output = false;
static const bool LOGS_ENABLED = false;
static const string STATE_CREATED = "CREATED";
static const string STATE_READY = "READY";
static const string STATE_RUNNING = "RUNNG";
static const string STATE_BLOCKED = "BLOCK";
static const string STATE_PREMPT = "PREMPT";

/*-------------------------------------------------------
Classes
---------------------------------------------------------*/
class Process{
public:
    int id;
    int AT;
    int TC;
    int CB;
    int IO;
    string state;
    string previous_state;
    int FT;
    int TT;
    int IT;
    int CW;
    int time_left;
    int event_time;
    int cpu_burst;
    int io_burst;
    int insert_order;
    int quantum;
    int static_priority;
    int dynamic_priority;
    int state_start_time;
};

class ProcessComparator{
public:
    bool operator() (Process* lhs, Process*rhs) const
    {
        if (lhs->event_time == rhs->event_time){
            return lhs->insert_order > rhs->insert_order;
        }else{
            return lhs->event_time > rhs->event_time;
        }
    }
};
class FinishedProcessesComparator{
public:
    bool operator() (Process* lhs, Process*rhs) const
    {
        return lhs->id > rhs->id;
    }
};
class IOTimePairComparator{
public:
    bool operator() (pair<int,int> t1,pair<int,int> t2) const
    {
        if (t1.first == t2.first){
            return t1.second > t2.second;
        }else{
            return t1.first > t2.first;
        }
    }
};
class ShortestJobFirstComparator{
public:
    bool operator() (Process* lhs, Process*rhs) const
    {
        if (lhs->time_left == rhs->time_left){
            return lhs->insert_order > rhs->insert_order;
        }else{
            return lhs->time_left > rhs->time_left;
        }
    }
};
class PriorityComparator{
public:
    bool operator() (Process* lhs, Process*rhs) const
    {
        if (lhs->dynamic_priority == rhs->dynamic_priority){
            return lhs->insert_order > rhs->insert_order;
        }else{
            return lhs->dynamic_priority < rhs->dynamic_priority;
        }
    }
};
#endif

