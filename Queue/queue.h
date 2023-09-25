#ifndef QUEUE_H
#define QUEUE_H

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
    arr[_back++] = data;
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



template <class T>
class CircularQueue : public Queue
{
public:
    void push_back(T data){
        bool backIsBehind = false;

        int size = 0;
        if(_back < _front) {
            size = _back + (capacity - _front);
            backIsBehind = true;
        } else { //back is ahead of front
            size = _back - _front;
        }

        if(size == capacity - 1) {
            
            int prevCap = capacity;
            capacity *= 2;
            T* temp = new T[capacity];

            if(backIsBehind){
                for(int i = _front; i < prevCap; i++) {
                    temp[i] = arr[i];
                }
                for(int i = 0; i < _back; i++) {
                    temp[i + prevCap] = arr[i];
                }   
            } else {
                for(int i = _front; i < _back; i++) {
                    temp[i] = arr[i];
                }
            }
        }
        
        //back needs to always be 1 index ahead of front so that if _front hits capacity
        //it can loop back to 0
        _back = (_back + 1) % capacity;
        arr[_back] = data;
    }

    void pop_front(){
        if(_front == _back) {
            cout << "Queue is empty\n";
            return;
        } else if (_front == capacity - 1) {
            _front = 0;
            return;
        } //else
        front++;
    }

    void print(){
        if(_front == _back) {
            cout << "Queue is empty\n";
            return;
        } else if(_back >= _front) {
            for(int i = _front; i < _back; i++) {
                cout << arr[i] << " ";
            }
        } else {
            for(int i = _front; i < capacity; i++) {
                cout << arr[i] << " ";
            }
            for(int i = 0; i < _back; i++) {
                cout << arr[i] << " ";
            }
        }
    }

};

template <class T>
class Deque : public Queue
{
public:
    void push_front(T data) {
        if(_front == 0) {
            cout << "Front is at first index, can't insert further.\n";
            return;
        } //else
        arr[_front--;] = data;
    }

    void pop_back() {
        if(_back == _front) {
            cout << "Back is at first index, can't pop further.\n";
            return;
        } //else
        back--;
    }
};

template <class T>
class PriorityQueue : public Queue
{    
public:
    void push_back(T data) {
        //filthy bruteforce
        //too tired to do anything else right now to be honest
        if(size() == capacity) {
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

        arr[_back++] = data;

        if(_back == 1) return; //if only one element, no need to sort

        int insertionIndex = back - 1;
        while(arr[insertionIndex] > arr[insertionIndex - 1] and insertionIndex > _front) {
            T temp = arr[insertionIndex];
            arr[insertionIndex] = arr[insertionIndex - 1];
            arr[insertionIndex - 1] = temp;
            insertionIndex--;
        }
    }
};

#endif /* QUEUE_H */
