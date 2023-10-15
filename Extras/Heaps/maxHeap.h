#ifndef MAXHEAP_H
#define MAXHEAP_H

#include <iostream>
#include <utility>

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

template<class T>
inline void maxHeap<T>::print() {
    std::cout << "Size: " << _size << std::endl;
    std::cout << "Capa: " << capacity << std::endl;

    for(int i = 0; i < _size; i++)
        std::cout << arr[i] << " ";
    std::cout << std::endl;
}

//generate heap from array w/ heapify
template <class T>
inline maxHeap<T>::maxHeap(T* _arr, int _capacity) {
    _size = _capacity;
    capacity = _capacity;
    arr = new T[capacity];

    for(int i = 0; i < _capacity; i++)
        arr[i] = _arr[i];

    //heapifyUp from size to size/2 OR
    //heapifyDown from size/2 to 0
    heapify(_arr, _capacity);
}

template<class T>
inline void maxHeap<T>::heapify(T* _arr, int _capacity) {    
    int secondLastLevel = getParent(_capacity);

    /*
    for(int i = secondLastLevel; i > -1; i--)
        heapifyDown(i);
    */
    
    //cap - 1 because size/index discrepancy
    for(int i = _capacity - 1; i >= secondLastLevel; i--)
        heapifyUp(i);
}

//auxiliary heapifys
template<class T>
inline void maxHeap<T>::heapifyUp(int node) {
    //larger node should be at the top
    int pNode = getParent(node);

    if(pNode == -1) {
        return;
    }

    //heapifyUp has a while because assume we swap up multiple times
    //and a value that's higher than us is on top after swapping
    //e.g try heapifying up 11 to 10 and then 1.
    while (arr[node] > arr[pNode]) {
        //if child is larger than its parent, then swap up, and keep swapping
        //up till the child (current) is no longer greater than its parent
        
        // std::cout << "Swapped " << arr[node] << " and " << arr[pNode] << std::endl; 
        std::swap(arr[node], arr[pNode]);
        heapifyUp(pNode);
    }
}

template<class T>
inline void maxHeap<T>::heapifyDown(int node) {
    //smaller node should be at the bottom
    int lNode = getLeftChild(node);
    int rNode = getRightChild(node);
    int maxValNode = node;

    if (lNode != -1 and arr[lNode] > arr[maxValNode])
        maxValNode = lNode;

    if (rNode != -1 and arr[rNode] > arr[maxValNode])
        maxValNode = rNode;

    //heapifyDown based because leaves don't have any children
    if (maxValNode != node) {
        //if one of the children is larger than the parent (current), then swap
        //them and keep swapping down till no child is larger than the parent
        
        // std::cout << "Swapped " << arr[node] << " and " << arr[maxValNode] << std::endl; 
        std::swap(arr[maxValNode], arr[node]);
        heapifyDown(maxValNode);
    }
}

//access relatives
template<class T>
inline int maxHeap<T>::getLeftChild(int parent) {
    int ans = (parent * 2) + 1;
    return (ans > _size) ? -1 : ans;
}

template<class T>
inline int maxHeap<T>::getRightChild(int parent) {
    int ans = (parent * 2) + 2;
    return (ans > _size - 1) ? -1 : ans;
}

template <class T>
inline int maxHeap<T>::getParent(int child) {
    int ans = (child - 1) / 2;
    return (ans < 0) ? -1 : ans;
}

//default constructor
template<class T>
inline maxHeap<T>::maxHeap() : _size(0), capacity(0), arr(nullptr) {}

#endif
