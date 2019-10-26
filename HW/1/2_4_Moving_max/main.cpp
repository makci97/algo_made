#include <fstream>
#include <queue>
#include <vector>
#include <utility>

#include <iostream>

// https://neerc.ifmo.ru/wiki/index.php?title=%D0%94%D0%B2%D0%BE%D0%B8%D1%87%D0%BD%D0%B0%D1%8F_%D0%BA%D1%83%D1%87%D0%B0
template <typename T>
class Heap
{
public:
    bool empty() const;
    T top() const;
    T pop();
    void push(const T& value);

private:
    void _sift_down(size_t idx);
    void _sift_up(size_t idx);
    
    std::vector<T> _data;
};

template <typename T>
bool Heap<T>::empty() const
{
    return _data.empty();
}

template <typename T>
T Heap<T>::top() const
{
    return _data.front();
}

template <typename T>
T Heap<T>::pop()
{
    T max = _data.front();
    _data[0] = _data.back();
    _data.pop_back();
    _sift_down(0);
    return max;
}

template <typename T>
void Heap<T>::push(const T& value)
{
    _data.push_back(value);
    _sift_up(_data.size() - 1);
}

template <typename T>
void Heap<T>::_sift_down(size_t idx)
{
    while (2 * idx + 1 < _data.size())
    {
        size_t left = 2 * idx + 1;
        size_t right = 2 * idx + 2;
        int j = left;
        if (right < _data.size() && _data[right] > _data[left])
            j = right;
        if (_data[idx] >= _data[j])
            break;
        swap(_data[idx], _data[j]);
        idx = j;
    }
}

template <typename T>
void Heap<T>::_sift_up(size_t idx)
{
    while (idx >= 1 && _data[idx] > _data[(idx - 1) / 2])
    {
        swap(_data[idx], _data[(idx - 1) / 2]);
        idx = (idx - 1) / 2;
    }
}

int main() {
    std::ifstream in_stream;
    std::ofstream out_stream;
    in_stream.open ("input.txt");
    out_stream.open("output.txt");
    
    // read
    int n = 0;
    in_stream >> n;
    std::vector<int> elems_queue;
    int elem;
    for (int i = 0; i < n; ++i)
    {
        in_stream >> elem;
        elems_queue.push_back(elem);
    }
    int k = 1;
    in_stream >> k;
    
    // find moving max
    Heap<std::pair<int, int> > heap;
    for (int i = 0; i < n; ++i)
    {
        heap.push(std::make_pair(elems_queue[i], i));
        if (i >= k - 1)
        {
            auto max_pair = heap.top();
            while (max_pair.second <= i - k)
            {
                heap.pop();
                max_pair = heap.top();
            }
            out_stream << max_pair.first << ' ';
        }
    }

    in_stream.close();
    out_stream.close();

    return 0;
}