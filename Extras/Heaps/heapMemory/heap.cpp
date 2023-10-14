#include "heap.h"
#include <stddef.h>
#include <iomanip>
#include <iostream>
#include <vector>

using namespace std;

Heap::Heap() { freeSegments.push_back(make_pair(arr, HEAP_SIZE)); }

void* Heap::ITUN(size_t _size) {
	// loop through free info
	for (int i = 0; i < freeSegments.size(); i++) {
		// if size of incoming memory is smaller than or equal to size of free
		// segment + 4 bytes to store size
		if (freeSegments[i].second >= _size + 4) {
			// store that address pointer
			int* freeAddress = (int*)freeSegments[i].first;

			// write size to char arr
			freeAddress[0] = _size;
			// cout << freeAddress[0] << endl;

			// reduce the size of that free (free till now) segment in the
			// vector
			shortenVector(i, _size);

			// return pointer to segment
			return freeAddress + 4;
			//+ 4 to account for the byte used to hold the size of that segment
		}
	}
	// TODO: If no free segment is large enough, throw an exception.
	return nullptr;
}

// TODO: ACCOMODATE 4 BYTES FOR SIZE OF SEGMENT
void Heap::shortenVector(int index, size_t _size) {
	// if size - _size == 0, delete that 'block' from the vector of free
	// segments.
	if (_size + 4 == freeSegments[index].second) {
		freeSegments.erase(freeSegments.begin() + index);
	}

	// if the size of the free segment is greater than the size of the incoming
	// memory reduce it, and add the new free segment to the vector
	else {
		freeSegments[index].second -=
			(_size = 4);  // reduce size of free segment by (size + 4) bytes
		freeSegments[index].first +=
			(_size + 4);  // move pointer size + 4 bytes forward
	}
}

void Heap::ITUD(void* ptr) {
	// make that area available for over-writing again.
	lengthenVector(ptr);
}

void Heap::lengthenVector(void* ptr) {
	size_t size;
	size = *(size_t*)ptr - 4;
	freeSegments.push_back(make_pair(static_cast<char*>(ptr), size));
}
