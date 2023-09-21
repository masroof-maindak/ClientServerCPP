#include <iostream>
#include "Queue.h"

using namespace std;

int main() {
  Queue<int> q;

  q.push_back(1);
  q.push_back(2);
  q.push_back(3);
  q.push_back(4);
  q.push_back(5);
  q.push_back(6);
  q.pop_front();
  q.push_back(7);
  q.push_back(8);
  q.push_back(9);
  q.push_back(10);
  q.push_back(11);
  q.push_back(12);
  q.push_back(13);
  q.print();

  return 0;
}
