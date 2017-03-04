#ifndef UTIL_H_INCLUDED
#define UTIL_H_INCLUDED

#include <iostream>
using namespace std;

class Process{
public:
    int id;
    int AT;
    int TC;
    int CB;
    int IO;
    string State;
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
};

class ProcessComparator{
private:
    bool reverse;
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
#endif