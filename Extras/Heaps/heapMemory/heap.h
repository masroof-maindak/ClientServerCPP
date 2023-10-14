#ifndef HEAP_H
#define HEAP_H

#define HEAP_SIZE 4000
#include <vector>

using namespace std;

class Heap {
   private:
	char arr[HEAP_SIZE];
	vector<pair<char*, size_t>> freeSegments;

	void shortenVector(int index, size_t _size);
	void lengthenVector(void*);

   public:
	Heap();
	void* ITUN(size_t);
	void ITUD(void*);
};

#endif	// HEAP_H
