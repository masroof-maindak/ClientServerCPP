#include <iostream>
#include "priorityQueue.h"

int main () {
    PriorityQueue<int> Q;

    Q.push(20);
    Q.push(10);
    Q.push(30);
    Q.push(40);
    Q.pop();
    std::cout << "Front: " << Q.front() << std::endl;
    Q.push(35);
    std::cout << "Front: " << Q.front() << std::endl;

    return 0;
}
