#ifndef SCHEDULER_H_INCLUDED
#define SCHEDULER_H_INCLUDED

#include <iostream>
#include <queue>
#include <stack>
#include "util.h"

using namespace std;

/*-------------------------------------------------------
parent class for all types of schedulers
---------------------------------------------------------*/
class Scheduler{
public:
    virtual Process* getEvent() = 0;
    virtual void setEvent(Process* p) = 0;
    virtual bool isEmpty() = 0;
    virtual string getName() = 0;
};

/*-------------------------------------------------------
class for FCFS scheduler
---------------------------------------------------------*/
class FCFS: public Scheduler{
private:
    queue<Process*> ready_queue;
public:
    Process* getEvent();
    void setEvent(Process* p);
    bool isEmpty();
    string getName();
};

/*-------------------------------------------------------
class for LCFS scheduler
---------------------------------------------------------*/
class LCFS: public Scheduler{
private:
    stack<Process*> ready_stack;
public:
    Process* getEvent();
    void setEvent(Process* p);
    bool isEmpty();
    string getName();
};

/*-------------------------------------------------------
class for SJF scheduler
---------------------------------------------------------*/
class SJF: public Scheduler{
private:
    priority_queue<Process*, vector<Process*>, ShortestJobFirstComparator> ready_queue;
    long int insert_order=0;
public:
    Process* getEvent();
    void setEvent(Process* p);
    bool isEmpty();
    string getName();
};

/*-------------------------------------------------------
class for RR scheduler
---------------------------------------------------------*/
class RR: public Scheduler{
private:
    queue<Process*> ready_queue;
    int quantum;
public:
    RR(int);
    Process* getEvent();
    void setEvent(Process* p);
    bool isEmpty();
    string getName();
};

/*-------------------------------------------------------
class for PRIO scheduler
---------------------------------------------------------*/
class PRIO: public Scheduler{
private:
    priority_queue<Process*, vector<Process*>, PriorityComparator> *active_queue;
    priority_queue<Process*, vector<Process*>, PriorityComparator> *expired_queue;
    priority_queue<Process*, vector<Process*>, PriorityComparator> *temp;
    int quantum;
    long int insert_order;
public:
    PRIO(int);
    Process* getEvent();
    void setEvent(Process* p);
    bool isEmpty();
    string getName();
};

#endif
