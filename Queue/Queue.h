#ifndef Queue_h
#define Queue_h

using namespace std;

template <class T>
class Queue
{
protected:
    T *arr;
    int capacity;       // Total size of your queue
    int _front;         // starting index of your queue
    int _back;          //  ending index of your queue
    
public:
    Queue();            // initialize your attributes
    int size();         // returns current size of your queue
    bool empty();       // tells whether the queue is empty or not
    T front();          // returns element at front of queue
    T back();           // returns element at back of queue
    void push_back(T);  // pushes the data at the back
    void pop_front();   // removes the data at front
    void print();       // print your queue from front to back
    ~Queue();           // deallocates memory
};

//push
template <class T>
inline void Queue<T>::push_back(T data)
{
    if(_back == capacity) {
        capacity *= 2;
        int width = size();

        T* temp = new T[capacity];
        for(int i = 0; i < width; i++) {
            temp[i] = arr[i + _front];
        }
        delete[] arr;
        arr = temp;

        _front = 0;
        _back = width;
    }
    arr[_back] = data;
    _back++;
}

//pop
template <class T>
inline void Queue<T>::pop_front()
{   
    if(_front == _back) {
        cout << "Queue is empty\n";
        return;
    }
    _front++;
}

//constructor
template<class T>
inline Queue<T>::Queue()
{
    capacity = 2;
    _front = 0;
    _back = 0;
    arr = new T[capacity];
}

//size
template <class T>
inline int Queue<T>::size()
{
    return _back - _front;
}

//empty check
template <class T>
inline bool Queue<T>::empty()
{
    return _front == _back;
}

//return front/first/oldest
template <class T>
inline T Queue<T>::front()
{
    return arr[_front];
}

//return back/last/newest
template <class T>
inline T Queue<T>::back()
{
    return arr[_back];
}

//print queue
template <class T>
inline void Queue<T>::print()
{
    for(int i = _front; i < _back; i++) {
        cout << arr[i] << " ";
    }
    cout << "\n";
}

//destructor
template <class T>
inline Queue<T>::~Queue()
{
    delete[] arr;
}

#endif /* Queue_h */
