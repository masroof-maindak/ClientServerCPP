#include <iostream>
#include "maxHeap.h"
#include "minHeap.h"

#define cap1 7

int main () {
    int* arr = new int[cap1] {1, 10, 3, 11, 5, 6, 4};

    for(int i = 0; i < cap1; i++) {
        std::cout << arr[i] << " ";
    }
    std::cout << std::endl << std::endl;

    maxHeap<int> h1(arr, cap1);
    h1.print();

    return 0;
}
