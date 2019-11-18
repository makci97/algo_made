#include <assert.h>
#include <fstream>


template<typename T>
class Deque
{
public:
    Deque() : _size(0), _capacity(1), _head(0), _tail(0)
    {
        _buffer = new T[_capacity];
    }
    
    bool empty();
    T pop_front();
    T pop_back();
    void push_front(T value);
    void push_back(T value);
    void realloc();

private:
    size_t _size;
    size_t _capacity;
    size_t _head;
    size_t _tail;
    
    T *_buffer;
};

template<typename T>
bool Deque<T>::empty()
{
    return _size == 0;
}

template<typename T>
T Deque<T>::pop_front()
{
    assert(!empty());
    T result = _buffer[_head];
    _head = (_head + 1) % _capacity;
    --_size;
    
    return result;
}

template<typename T>
T Deque<T>::pop_back()
{
    assert(!empty());
    _tail = (_tail - 1) % _capacity;
    T result = _buffer[_tail];
    --_size;

    return result;
}

template<typename T>
void Deque<T>::push_front(T value)
{
    if(_size == _capacity)
        realloc();

    // add new value
    _head = (_head - 1) % _capacity;
    _buffer[_head] = value;
    ++_size;
}

template<typename T>
void Deque<T>::push_back(T value)
{
    if(_size == _capacity)
        realloc();
    
    // add new value
    _buffer[_tail] = value;
    _tail = (_tail + 1) % _capacity;
    ++_size;
}

template<typename T>
void Deque<T>::realloc()
{
    // move data to new space
    T *new_buffer = new T[_capacity * 2];
    for (int i = 0; i < _size; ++i)
    {
        new_buffer[i] = _buffer[(_head + i) % _size];
    }
    
    // update vars state
    delete[] _buffer;
    _buffer = new_buffer;
    
    _capacity *= 2;
    _head = 0;
    _tail = _size;
}

int main()
{
    std::ifstream in_stream;
    std::ofstream out_stream;
    in_stream.open("input.txt");
    out_stream.open("output.txt");
    
    int n = 0;
    in_stream >> n;
    
    int command, value;
    Deque<int> container;
    bool is_correct = true;
    for (int i = 0; i < n; ++i)
    {
        in_stream >> command >> value;
        switch (command)
        {
            case 1:
                // push front
                container.push_front(value);
                break;
            case 2:
                // pop front
                if (container.empty())
                {
                    if (value != -1)
                        is_correct = false;
                }
                else
                {
                    is_correct &= (container.pop_front() == value);
                }
                break;
            case 3:
                // push back
                container.push_back(value);
                break;
            case 4:
                // pop back
                if (container.empty())
                {
                    if (value != -1)
                        is_correct = false;
                }
                else
                {
                    is_correct &= (container.pop_back() == value);
                }
                
                break;
            
            default:
                break;
        }
        if (!is_correct)
            break;
    }
    
    
    if (is_correct)
        out_stream << "YES";
    else
        out_stream << "NO";
    
    in_stream.close();
    out_stream.close();
    
    return 0;
}