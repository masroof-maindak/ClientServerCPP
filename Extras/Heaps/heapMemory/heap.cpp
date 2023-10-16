#include <iostream>
#include <vector>
#include "heap.h"

using namespace std;

Heap::Heap(){
    freeSegments.push_back(make_pair(HEAP_SIZE, arr));
    freeMemory.insert(make_pair(HEAP_SIZE, arr));
}

void* Heap::ITUN (size_t _size) {   

    //loop through free info
    for(int i = 0; i < freeSegments.size(); i++) {

        //if size of incoming memory is smaller than or equal to size of free segment + 4 bytes to store size
        if(freeSegments[i].first >= _size + 4) {

            //store that address pointer
            int* freeAddress = (int*)freeSegments[i].second;

            // cout << "DEBUG - Memory allocated at address: " << freeAddress << endl;
            //write size to char arr
            freeAddress[0] = _size;

            //reduce the size of that free (free till now) segment in the structure
            shortenVector(i, _size);

            //return pointer to segment
            // cout << "DEBUG - Memory Returned at address: " << freeAddress + 1 << endl;
            return freeAddress + 1;
            //+ 1 to account for the byte used to hold the size of that segment
        }

    }

    throw invalid_argument("Not enough memory available.");
    return nullptr;
}

void Heap::shortenVector(int index, size_t _size) {
    std::pair<int, char*> temp;

    // if size - _size == 0, delete that 'block' from the vector of free segments.
    if(_size + 1 == freeSegments[index].first){
        freeSegments.erase(freeSegments.begin() + index);
    }

    //if the size of the free segment is greater than the size of the incoming memory
    //reduce it, and add the new free segment to the structure
    else {

        freeSegments[index].second += (_size + 4); //move pointer (size + 4) bytes forward
        freeSegments[index].first -= (_size + 4); //reduce size of free segment by (size + 4) bytes
    }
}

void Heap::ITUD(void* ptr){
    if(ptr == nullptr){
        throw invalid_argument("Cannot delete null pointer.");
    }

    //make that area available for over-writing again.
    // cout << "\nDEBUG - Memory deallocated at address: " << ptr << endl;
    lengthenVector(ptr);
}

void Heap::lengthenVector(void* ptr) {
    int* size;
    size = static_cast<int*>(ptr) - 1;
    size_t umm = *size;
    freeSegments.push_back(make_pair(umm + 4, static_cast<char*>(ptr)));
    freeMemory.insert(make_pair(umm + 4, static_cast<char*>(ptr)));
}

int Heap::getFreeSize() {   
    int size = 0;
    for(int i = 0; i < freeSegments.size(); i++){
        size += freeSegments[i].first;
    }
    return size;
}
