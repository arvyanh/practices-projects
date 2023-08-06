#include <memory>

template <typename V> class CbIterator {
public:
    V *ptr;
    V *buf_start;
    V *buf_end;
    CbIterator(V *ptr, V *buf_start, V *buf_end) :
        ptr(ptr),
        buf_start(buf_start),
        buf_end(buf_end) {}

    CbIterator &operator=(const CbIterator &other) {
        ptr       = other.ptr;
        buf_start = other.buf_start;
        buf_end   = other.buf_end;
        return *this;
    }
    CbIterator &operator++() {
        if (ptr == buf_end) {
            ptr = buf_start;
        } else {
            ++ptr;
        }
        return *this;
    }
    V &operator[](int i) { return *(ptr + i); }
    V &operator->() { return *ptr; }
    V *operator*() { return ptr; }
};

template <class T> class CircularBuffer {
private:
    //---------------------------------------------------------------
    // Circular_Buffer - Private Member Variables
    //---------------------------------------------------------------

    std::unique_ptr<T[]> buffer; // using a smart pointer is safer (and we
                                 // don't have to implement a destructor)
    size_t head = 0;
    size_t tail = 0;
    int    max_size;
    T      empty_item; // we will use this to clear data
public:
    //---------------------------------------------------------------
    // Circular_Buffer - Public Methods
    //---------------------------------------------------------------

    // explicit size required, no default constructor
    //  Create a new Circular_Buffer.
    CircularBuffer<T>(int max_size) :
        buffer(std::unique_ptr<T[]>(new T[max_size])),
        max_size(max_size){};

    auto operator[](int index) -> decltype(buffer[index]) {
        int idx = (head + index) % max_size;
        return buffer[idx];
    }

    // Add an item to this circular buffer.
    bool enqueue(T item) {

        // if buffer is full, throw an error
        if (is_full()) return false;

        // insert item at back of buffer
        buffer[tail] = item;

        // increment tail
        tail = (tail + 1) % max_size;

        return true;
    }

    // Remove an item from this circular buffer and return it.
    T dequeue() {

        // if buffer is empty, return empty item
        if (is_empty()) return T();

        // get item at head
        T item = buffer[head];

        // set item at head to be empty
        T empty;
        buffer[head] = empty_item;

        // move head foward
        head = (head + 1) % max_size;

        // return item
        return item;
    }

    // Return the item at the front of this circular buffer.
    T front() { return buffer[head]; }

    // Return true if this circular buffer is empty, and false otherwise.
    bool is_empty() { return head == tail; }

    // Return true if this circular buffer is full, and false otherwise.
    bool is_full() { return tail == (head - 1) % max_size; }

    CbIterator<T> begin() {
        return CbIterator<T>(
            buffer.get() + head, buffer.get(), buffer.get() + max_size);
    }

    CbIterator<T> end() {
        return CbIterator<T>(
            buffer.get() + tail, buffer.get(), buffer.get() + max_size);
    }

    // Return the size of this circular buffer.
    size_t size() {
        if (tail >= head) return tail - head;
        return max_size - head - tail;
    }
};
