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
    vector <pair<size_t, char*>> freeSegments;
    minHeap <pair<int, char*>> freeMemory;

    void shortenVector(int index, size_t _size);
    void lengthenVector(void*);

public:
    Heap();
    int getFreeSize();
    void* ITUN(size_t);
    void ITUD(void*);
};

#endif