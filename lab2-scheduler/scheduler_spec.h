#ifndef SCHEDULER_SPEC_H_INCLUDED
#define SCHEDULER_SPEC_H_INCLUDED

#include <iostream>
#include <queue>
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
class for all FCFS scheduler
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
class for all LCFS scheduler
---------------------------------------------------------*/
class LCFS: public Scheduler{
private:
    queue<Process*> ready_queue;
public:
    Process* getEvent();
    void setEvent(Process* p);
    bool isEmpty();
    string getName();
};

/*-------------------------------------------------------
class for all SJF scheduler
---------------------------------------------------------*/
class SJF: public Scheduler{
private:
    queue<Process*> ready_queue;
public:
    Process* getEvent();
    void setEvent(Process* p);
    bool isEmpty();
    string getName();
};

/*-------------------------------------------------------
class for all RR scheduler
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
class for all PRIO scheduler
---------------------------------------------------------*/
class PRIO: public Scheduler{
private:
    queue<Process*> ready_queue;
    int quantum;
public:
    PRIO(int);
    Process* getEvent();
    void setEvent(Process* p);
    bool isEmpty();
    string getName();
};

#endif
