#ifndef PRIORITY_QUEUE_H
#define PRIORITY_QUEUE_H

#include <iostream>
#include "../maxHeap.h"

template <class T>
class PriorityQueue 
{
    maxHeap<T> PQ;
    int _size;
public:
    PriorityQueue(); // You know guys...
    void push(int data); // insert element into queue
    void pop(); // remove highest key value element
    T front(); // return the element with highest key value
    bool empty() {return _size == 0;} // tell whether queue is empty or not
    int size() {return _size;} // tell the size of the queue
    ~PriorityQueue() {delete PQ;} // do whatever is necessary...  
};

#endif
