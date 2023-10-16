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
    minHeap <pair<int, char*>> freeMemory;

    vector <pair<int, char*>> freeSegments;

    void shortenHeap(std::pair<int, char*>, size_t _size);
    void lengthenHeap(void*);
public:
    Heap();
    void* ITUN (size_t);
    void ITUD (void*);
    int getFreeSize();
};

#endif
