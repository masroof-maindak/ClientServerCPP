#ifndef PRIORITY_QUEUE_H
#define PRIORITY_QUEUE_H

#include <iostream>
#include "../maxHeap.h"

template <class T>
class PriorityQueue 
{
    maxHeap<T> PQ;
public:
    PriorityQueue() {}; // You know guys...

    void push(int data) {PQ.insert(data);}; // insert element into queue
    void pop() {PQ.erase(front());} // remove highest key value element

    T front() {return PQ.getMax();} // return the element with highest key value
    bool empty () {return PQ.empty();} // tell whether queue is empty or not
    int size () {return PQ.size();} // tell the size of the queue
    ~PriorityQueue() {} // do whatever is necessary...  
};

#endif
