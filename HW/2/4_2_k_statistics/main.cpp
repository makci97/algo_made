#include <fstream>
#include <queue>
#include <vector>
#include <utility>

#include <iostream>

template <typename T>
T median_of_three(T a, T b, T c)
{
    if ((b < a && a < c) || (c < a && a < b))
        return a;
    else if ((a < b && b < c) || (c < b && b < a))
        return b;
    return c;
}

template <typename T>
void set_median_first(T* array, int n)
{
    if (n < 3)
        return;
    
    // find median value
    int med_idx = n / 2;
    T med_val = median_of_three(array[0], array[med_idx], array[n - 1]);
    
    // swap with first
    if (med_val == array[med_idx])
        std::swap(array[0], array[med_idx]);
    else if (med_val == array[n - 1])
        std::swap(array[0], array[n - 1]);
}

template <typename T>
int partition(T* array, int n)
{
    if (n <= 1)
        return 0;
    
    set_median_first(array, n);
    const T& anchor = array[0];
    int i = n - 1;
    int j = n - 1;
    
    while (j > 0)
    {
        // array[0] == anchor
        if (array[j] > anchor)
            std::swap(array[j--], array[i--]);
        else
            --j;
    }
    std::swap(array[i], array[0]);
    
    return i;
}

template <typename T>
T k_statistics(T* array, int n, int k)
{
    int l = 0;
    int r = n;
    while (l < r)
    {
        int i = l + partition(array + l, r - l);
        if (i == k)
            return array[k];
        else if (i > k)
            r = i;
        else
            l = i + 1;
    }
}

int main() {
    std::ifstream in_stream;
    std::ofstream out_stream;
    in_stream.open("input.txt");
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
    
    // write result
    out_stream << k_statistics(array, n, k);
    
    // clean
    delete[] array;
    in_stream.close();
    out_stream.close();
    
    return 0;
}