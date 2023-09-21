template <class T>
class Queue
{
protected:
    
    T *arr;
    int capacity;    // Total size of your queue
    int _front;    // starting index of your queue
    int _back;    //  ending index of your queue
    
public:
    
    Queue(); // initialize your attributes
    int size(); // returns current size of your queue
    bool empty(); // tells whether the queue is empty or not
    T front(); // returns element at front of queue
    T back(); // returns element at back of queue
    void push_back(T data); // pushes the data at the back
    void pop_front(); // removes the data at front
    void print(); // print your queue from front to back
    ~Queue(); // deallocates memory
};
