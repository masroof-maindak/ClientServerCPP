#include <iostream>
#include "heap.h"

using namespace std;

int main() {
	int arr_size = 5;
	Heap H;
	int* arr = (int*)H.ITUN(arr_size * sizeof(int));
	for (int i = 0; i < arr_size; i++) {
		*(arr + i + 1) = i;
	}
	cout << "nigga";

	return 0;
}
