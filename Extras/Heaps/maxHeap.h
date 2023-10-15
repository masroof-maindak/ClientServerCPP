#ifndef MAXHEAP_H
#define MAXHEAP_H

#include <iostream>

template <class T>
class maxHeap
{
private:
    T* arr;
    int _size;
    int capacity;
    
    int getLeftChild(int parent); // return left child of parent
    // return -1 if it doesn't exist
    
    int getRightChild(int parent); // return right child of parent
    // return -1 if it doesn't exist
    
    int getParent(int child); // return parent of child
    // return -1 if it doesn't exist
    
    void heapifyUp(int node); // restore the heap property
    // compare children with parent and std::swap if disruption exists
    
    void heapifyDown(int node); // restore the heap property
    // compare parent with children and std::swap if disruption exists
    
public:
    maxHeap(); // make an empty heap

    void heapify(T* _arr, int _capacity); // make a heap from given array
    maxHeap(T* _arr, int _capacity); // initialize through heapify

    T getMax() {return arr[0];} // return the element with maximum key value
    T extractMax(); // return and pop the element with maximum key value

    bool empty() {return _size == 0;} // tells whether heap is empty or not
    int size() {return _size;} // return number of elements

    void insert(T _key); // insert the data into heap based on key value
    void erase(T _key); // delete the data based on key value

    void print(); // print the heap in whatever order you like
    // by order it means, either preorder, inorder, postorder
    // or in ascending/descending order
    ~maxHeap() {delete[] arr;} // deallocate the memory
};

//access relatives
template<class T>
inline int maxHeap<T>::getLeftChild(int parent) {
    int ans = (parent*2) + 1;
    return (ans > _size) ? -1 : ans;
}

template<class T>
inline int maxHeap<T>::getRightChild(int parent) {
    int ans = (parent*2) + 2;
    return (ans > _size) ? -1 : ans;
}

template <class T>
inline int maxHeap<T>::getParent(int child) {
    int ans = (child - 1) * 2;
    return (ans < 0) ? -1 : ans;
}

//default constructor
template<class T>
inline maxHeap<T>::maxHeap() : _size(0), capacity(0), arr(nullptr) {}

#endif
