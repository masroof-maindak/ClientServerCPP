#ifndef MAXHEAP_H 
#define MAXHEAP_H

template <class T>
class minHeap
{
private:
    T* arr;
    int _size;
    int _capacity;
    
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
    T getMin(); // return the element with minimum key value
    T extractMin(); // return and pop the element with minumum key value
    void insert(T _key); // insert the data into heap based on key value
    void erase(T _key); // delete the data based on key value
    bool empty(); // tells whether heap is empty or not
    int size(); // return number of elements
    void print(); // print the heap in whatever order you like
    // by order it means, either preorder, inorder, postorder
    // or in ascending/descending order
    
    static void sort();
    ~minHeap(); // deallocate the memory
};


#endif