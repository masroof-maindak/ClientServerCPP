#include <iostream>
#include <vector>
#include "heap.h"

int main() {
	Heap H;
	vector<int*> arrs;
	int arr_size = 5;

    std::cout << "Free size: " << H.getFreeSize() << std::endl;

    std::cout << "Inserting int 4 arrays of size " << arr_size << std::endl;
	arrs.push_back((int*)H.ITUN(arr_size * sizeof(int)));
	arrs.push_back((int*)H.ITUN(arr_size * sizeof(int)));
	arrs.push_back((int*)H.ITUN(arr_size * sizeof(int)));
	arrs.push_back((int*)H.ITUN(arr_size * sizeof(int)));

    std::cout << "Free size: " << H.getFreeSize() << std::endl;

	for (int i = 0; i < arr_size; i++) {
		arrs[0][i] = (i+1)*1;
		arrs[1][i] = (i+1)*5;
		arrs[2][i] = (i+1)*10;
		arrs[3][i] = (i+1)*15;
	}
	
	//print all arrays
	std::cout << "All currently existing arrays: " << std::endl;
	for(int i = 0; i < arrs.size(); i++){
		std::cout << "\t" << i << ": ";
		for(int j = 0; j < arr_size; j++){
			std::cout << arrs[i][j] << " ";
		}
		std::cout << std::endl;
	}

    std::cout << "Removing array #2" << std::endl;
    H.ITUD(arrs[1]);
    arrs.erase(arrs.begin() + 1);
    std::cout << "Free size: " << H.getFreeSize() << std::endl;

	std::cout << "All currently existing arrays: " << std::endl;
	for(int i = 0; i < arrs.size(); i++){
		std::cout << "\t" << i << ": ";
		for(int j = 0; j < arr_size; j++){
			std::cout << arrs[i][j] << " ";
		}
		std::cout << std::endl;
	}
    std::cout << "Free size: " << H.getFreeSize() << std::endl;
}
