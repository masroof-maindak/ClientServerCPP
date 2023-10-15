#include <iostream>
#include "maxHeap.h"
#include "minHeap.h"

#define cap 7

int main () {
    int* arr = new int[cap] {1, 10, 3, 11, 5, 6, 4};
    
    minHeap<int>::sort(arr, cap);

    std::cout << "Sorted Array: ";
    for (int i = 0; i < cap; i++) {
        std::cout << arr[i] << " ";
    }
    std::cout << std::endl;

    return 0;
}
