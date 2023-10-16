#include <iostream>
#include <vector>
#include "heap.h"

Heap::Heap(){
    freeSegments.push_back(make_pair(HEAP_SIZE, arr));
    freeMemory.insert(make_pair(HEAP_SIZE, arr));
}

void* Heap::ITUN (size_t _size) {
    //store elements in a vector till required 'size' found
    std::vector<pair<int,char*>> storage;
    std::pair<int, char*> element;
    int count = 0;

    //keep searching till the 'size' we find is not either equal to requirement or greater
    //                                              //or we exceed the bounds of the heap
    do {
        //get element at top
        element = freeMemory.extractMin();

        //store it for later
        storage.push_back(element);
        
        //increment count
        count++;
    } while (!(element.first >= _size + 4) and count < freeMemory.size());

    //now, 'element' contains the piece of memory we want to manipulate
    //at least, I hope so...
    
    //start off by removing it from the vector
    storage.pop_back();

    //put back the ones popped before it back into the heap
    for (int i = 0; i < storage.size(); i++)
        freeMemory.insert(storage[i]);

    //if we DID manage to find a sexy node we need:
    if (element.first >= _size + 4) {
        //then, work on that one node that is cool
        //start off by storing the memory it's pointing as an int
        int* freeAddress = (int*)element.second;

        //we stored it as an int so we can write the size into it
        freeAddress[0] = _size;

        //now, we modify the node to shorten/eliminate it
        shortenHeap(element, _size);

        //and finally, send back the pointer to where the segment starts from
        return freeAddress + 1; //+1 to account for the byte used to hold the size
    } 

    //if we didn't however find the sexy node
    //add the last popped node back anyway
    freeMemory.insert(element);

    //and return jack shit
    std::cerr << "Not enough memory available." << std::endl;
    return nullptr;
}

void Heap::shortenHeap(std::pair<int, char*> temp, size_t _size) {
    //if size requirement matches perfectly, just remove that shit permanently, i.e continue:
    if (_size + 1 == temp.first)
        return;

    //if it doesn't, that means that we have to reduce the size a bit and add it back to the heap
    temp.first -= (_size + 4);
    temp.second += (_size + 4);
    freeMemory.insert(temp);
}

void Heap::ITUD(void* ptr){
    if(ptr == nullptr){
        throw invalid_argument("Cannot delete null pointer.");
    }

    //make that area available for over-writing again.
    // cout << "\nDEBUG - Memory deallocated at address: " << ptr << endl;
    lengthenHeap(ptr);
}

void Heap::lengthenHeap(void* ptr) {
    //'size' points to the point behind ptr starts (i.e where we literally stored the size)
    //of the segment that is now going to follow
    int* size = static_cast<int*>(ptr) - 1;

    //dereference it to store it in an umm
    size_t umm = *size;
    
    //add to the heap
    freeMemory.insert(make_pair(umm + 4, static_cast<char*>(ptr)));
}

int Heap::getFreeSize() {
    int ans = 0;
    vector<pair<int, char*>> storage;

    //store segments
    while (!freeMemory.empty()) {
        std::pair<int, char*> temp = freeMemory.extractMin();
        ans += temp.first;
        storage.push_back(temp);
    }

    //add segments back
    for (int i = 0; i < storage.size(); i++)
        freeMemory.insert(storage[i]);
    storage.clear();
    return ans;
}
