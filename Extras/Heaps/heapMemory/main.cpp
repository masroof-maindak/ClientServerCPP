#include <iostream>
#include <vector>
#include "heap.h"
using namespace std;

int main() {
	Heap H;
	vector<int*> arrs;
	int arr_size = 5;

	arrs.push_back((int*)H.ITUN(arr_size * sizeof(int)));
	arrs.push_back((int*)H.ITUN(arr_size * sizeof(int)));
	arrs.push_back((int*)H.ITUN(arr_size * sizeof(int)));

	for (int i = 0; i < arr_size; i++) {
		arrs[0][i] = (i+1)*1;
		arrs[1][i] = (i+1)*10;
		arrs[2][i] = (i+1)*100;
	}
	
	//print all arrays
	cout << "All currently existing arrays: " << endl;
	for(int i = 0; i < arrs.size(); i++){
		cout << "\t" << i << ": ";
		for(int j = 0; j < arr_size; j++){
			cout << arrs[i][j] << " ";
		}
		cout << endl;
	}

	cout << "Free space: " << H.getFreeSize() << endl;
}