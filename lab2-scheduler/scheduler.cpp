#include "scheduler.h"

using namespace std;

/*-------------------------------------------------------
Implementation of first come first serve scheduler
In this shceduler the datastructure used will be queue.
Because queue has implicit property of FCFS, it will serve
our purpose in this scheduler
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
    //setting priority this way to maintain consistency
    //among all the schedulers
    p->dynamic_priority = p->static_priority-1;
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
Similar to the previous scheduler but here we will use 
stack datastructure, because of its property of LCFS.
Just using stack for storing our processes it will perfectly
serve our purpose in this scheduler
---------------------------------------------------------*/
Process* LCFS::getEvent(){
    if(!isEmpty()){
        Process *process;
        process = ready_stack.top();
        ready_stack.pop();
        return process;
    }
    return NULL;
}
void LCFS::setEvent(Process* p){
    //setting priority this way to maintain consistency
    //among all the schedulers
    p->dynamic_priority = p->static_priority-1;
    ready_stack.push(p);
}
bool LCFS::isEmpty(){
    return ready_stack.empty();
}
string LCFS::getName(){
    return "LCFS";
}

/*-------------------------------------------------------
Implementation of shortest Job first scheduler
In this scheduler we will use priority queue datastructure.
We will use the comparator such that processes are ordered
in such a way that processes with shortest remaining time
is on the top. Also if two processes have same remaining time
then the process which arrived first to the scheduler will 
be given priority.
---------------------------------------------------------*/
Process* SJF::getEvent(){
    if(!isEmpty()){
        Process *process;
        process = ready_queue.top();
        ready_queue.pop();
        return process;
    }
    return NULL;
}
void SJF::setEvent(Process* p){
    //setting priority this way to maintain consistency
    //among all the schedulers
    p->dynamic_priority = p->static_priority-1;
    p->insert_order = insert_order++;
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
This is very much similar to FCFS scheduler.
Here also we are using queue as our datastructure.
Only difference is that we will give cpu to a process for
only quantum of time and then put it in ready state if its
cpu-burst is not complete
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
    //setting priority this way to maintain consistency
    //among all the schedulers
    p->dynamic_priority = p->static_priority-1;
    p->quantum = quantum;
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
There will be 2 datastructures in this scheduler, running and expired
In this scheduler we will use priority queue as our datastructure
and comparator will be such, so that processes are ordered
in such a way that process with highest priority is on the top.
If two processes have same priority the the process which 
became ready first will be scheduled. We will decrease the 
priority of a process by 1 every time its completed it running 
state. If a process has priority of -1 then it will be moved 
to expired queue. If running queue is empty we will switch 
running and expired queue. In this way we are promoting fairness
among processes.
---------------------------------------------------------*/
PRIO::PRIO(int n)
{
    quantum = n;
    insert_order = 0;
    active_queue = new priority_queue<Process*, vector<Process*>, PriorityComparator>();
    expired_queue = new priority_queue<Process*, vector<Process*>, PriorityComparator>();
}
Process* PRIO::getEvent(){
    if(active_queue->empty()){
        temp = active_queue;
        active_queue = expired_queue;
        expired_queue = temp;
    }
    if(!active_queue->empty()){
        Process *process;
        process = active_queue->top();
        active_queue->pop();
        return process;
    }
    return NULL;
}
void PRIO::setEvent(Process* p){
    p->quantum = quantum;
    p->dynamic_priority--;
    p->insert_order = insert_order++;
    if(p->dynamic_priority<0){
        p->dynamic_priority = p->static_priority-1;
        expired_queue->push(p);
    }else{
        active_queue->push(p);
    }
}
bool PRIO::isEmpty(){
    return active_queue->empty()&&expired_queue->empty();
}
string PRIO::getName(){
    return "PRIO " + to_string(quantum);
}

