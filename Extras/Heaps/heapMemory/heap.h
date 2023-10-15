#ifndef HEAP_H
#define HEAP_H

#include <vector>
#include "../minHeap.h"

#define HEAP_SIZE 100
using namespace std;

class Heap
{
private:
    char arr[HEAP_SIZE];
    vector <pair<int, char*>> freeSegments;
    minHeap <pair<int, char*>> freeMemory;

    void shortenVector(int index, size_t _size);
    void lengthenVector(void*);
public:
    Heap();
    void* ITUN (size_t);
    void ITUD (void*);
    int getFreeSize ();
};

#endif