#ifndef MINHEAP_H 
#define MINHEAP_H

#include <iostream>
#include <utility>

template <class T>
class minHeap
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
    // compare children with parent and swap if disruption exists
    
    void heapifyDown(int node); // restore the heap property
    // compare parent with children and swap if disruption exists
    
public:
    minHeap(); // make an empty heap

    void heapify(T* _arr, int _capacity); // make a heap from given array
    minHeap(T* _arr, int _capacity); // initialize through heapify

    bool empty() {return (_size == 0);} // tells whether heap is empty or not
    int size() {return _size;} // return number of elements

    T getMin() {return arr[0];} // return the element with minimum key value
    T extractMin(); // return and pop the element with minumum key value

    void insert(T _key); // insert the data into heap based on key value - heapifyDown
    void erase(T _key); // delete the data from heap based on key value - heapifyUp

    void print(); // print the heap in whatever order you like
    // by order it means, either preorder, inorder, postorder
    // or in ascending/descending order
    
    static void sort(T* _arr, int _capacity);
    // sort the array in descending order using heap sort
    ~minHeap() {delete[] arr;} // deallocate the memory
};


//access relatives
template<class T>
inline int minHeap<T>::getLeftChild(int parent) {
    int ans = (parent * 2) + 1;
    return (ans > _size) ? -1 : ans;
}

template<class T>
inline int minHeap<T>::getRightChild(int parent) {
    int ans = (parent * 2) + 2;
    return (ans > _size) ? -1 : ans;
}

template <class T>
inline int minHeap<T>::getParent(int child) {
    int ans = (child - 1) / 2;
    return (ans < 0) ? -1 : ans;
}

//default constructor
template<class T>
inline minHeap<T>::minHeap() : _size(0), capacity(0), arr(nullptr) {}

#endif
