#include <cassert>
#include <fstream>
#include <cstring>


template <typename T>
void merge(const T* first_array, int first_len, const T* second_array, int second_len, T* final_array)
{
    int i = 0;
    int j = 0;
    int k = 0;
    while ((i < first_len) || (j < second_len))
    {
        if ((i == first_len) || ((j < second_len) && (second_array[j] < first_array[i])))
            final_array[k] = second_array[j++];
        else
            final_array[k] = first_array[i++];
        ++k;
    }
    assert(k == (first_len + second_len));
}

template <typename T>
void merge_sort(T* array, int len)
{
    // end of recursion
    if (len <= 1)
        return;
    
    // split
    int first_len = len / 2;
    int second_len = len - first_len;
    
    // sort parts
    merge_sort(array, first_len);
    merge_sort(array + first_len, second_len);
    
    // merge parts
    T* additional_array = new T[len];
    merge(array, first_len, array + first_len, second_len, additional_array);
    std::memcpy(array, additional_array, sizeof(T) * len);
    delete[] additional_array;
}

// j >= i + k -> a[i] <= a[j]
template <typename T>
void k_ordered_sort(T* array, int len, int k)
{
    for (int i = 0; i < len; i += k)
    {
        int block_len = std::min(2 * k, len - i);
        merge_sort(array + i, block_len);
    }
}


int main() {
    std::ifstream in_stream;
    std::ofstream out_stream;
    in_stream.open ("input.txt");
    out_stream.open("output.txt");
    
    // read
    int n = 0;
    int k = 1;
    in_stream >> n >> k;
    int* array = new int[n];
    for (int i = 0; i < n; ++i)
    {
        in_stream >> array[i];
    }
    
    // sort
    k_ordered_sort(array, n, k);
    
    // write result
    for (int i = 0; i < n; ++i)
    {
        out_stream << array[i] << ' ';
    }
    
    // clean
    delete[] array;
    in_stream.close();
    out_stream.close();

    return 0;
}