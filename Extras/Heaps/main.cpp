#include <iostream>
#include "maxHeap.h"
#include "minHeap.h"

#define cap1 7

int main () {
    int* arr = new int[cap1]{5, 10, 3, 11, 8, 4, 1};
    minHeap<int> h1(arr, cap1);

    return 0;
}
