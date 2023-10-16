#include <iostream>
#include "maxHeap.h"
#include "minHeap.h"
#include <utility>

#define cap 7

int main () {
    std::pair<int, char> pair1 (3, 'a');
    std::pair<int, char> pair2 (7, 'b');
    std::pair<int, char> pair3 (9, 'c');
    std::pair<int, char> pair4 (5, 'd');
    std::pair<int, char> pair5 (1, 'e');

    minHeap<std::pair<int, char>> pairArr;
    pairArr.insert(pair1);
    pairArr.insert(pair2);
    pairArr.insert(pair3);
    pairArr.insert(pair4);
    pairArr.insert(pair5);

    std::pair<int, char> ans = pairArr.getMin();
    std::cout << ans.first << ", " << ans.second << std::endl;

    pairArr.erase(std::pair<int,char>(1,'e'));

    ans = pairArr.getMin();
    std::cout << ans.first << ", " << ans.second << std::endl;

    return 0;
}
