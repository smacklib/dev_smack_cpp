#include <array>
#include <utility>
#include <cstdlib>

template<class T>
constexpr void swap(T& l, T& r)
{
    T tmp = std::move(l);
    l = std::move(r);
    r = std::move(tmp);
}

template <typename T, size_t N>
struct cx_array
{
    constexpr T& operator[](size_t i)
    {
        return arr[i];
    }

    constexpr const T& operator[](size_t i) const
    {
        return arr[i];
    }

    constexpr const T* begin() const
    {
        return arr;
    }
    constexpr const T* end() const
    {
        return arr + N;
    }

    T arr[N];
};

template <typename T, size_t N>
constexpr void quick_sort_impl(cx_array<T, N> &cx_array, size_t left, size_t right)
{
    if (left < right)
    {
        size_t m = left;

        for (size_t i = left + 1; i<right; i++)
            if (cx_array[i]<cx_array[left])
                swap(cx_array[++m], cx_array[i]);

        swap(cx_array[left], cx_array[m]);

        quick_sort_impl(cx_array, left, m);
        quick_sort_impl(cx_array, m + 1, right);
    }
}

template<typename cx_array>
constexpr void comb_sort_impl ( cx_array & cx_array_ ) noexcept {
    using size_type = typename cx_array::size_type;
    size_type gap = cx_array_.size ( );
    bool swapped = false;
    while ( ( gap > size_type { 1 } ) or swapped ) {
        if ( gap > size_type { 1 } ) {
            gap = static_cast<size_type> ( gap / 1.247330950103979 );
        }
        swapped = false;
        for ( size_type i = size_type { 0 }; gap + i < static_cast<size_type> ( cx_array_.size ( ) ); ++i ) {
            if ( cx_array_ [ i ] > cx_array_ [ i + gap ] ) {
                auto swap = cx_array_ [ i ];
                cx_array_ [ i ] = cx_array_ [ i + gap ];
                cx_array_ [ i + gap ] = swap;
                swapped = true;
            }
        }
    }
}

template <typename T, size_t N>
constexpr cx_array<T, N> sort(cx_array<T, N> cx_array)
{
    auto sorted = cx_array;
    quick_sort_impl(sorted, 0, N);
    return sorted;
}

constexpr cx_array<int, 11> unsorted{5,7,3,4,1,8,2,9,0,6,10}; // odd number of elements
constexpr auto sorted = sort(unsorted);

#include <iostream>
int main()
{
    std::cout << "unsorted: ";
    for(auto const& e : unsorted) 
      std::cout << e << ", ";
    std::cout << '\n';

    std::cout << "sorted: ";
    for(auto const& e : sorted) 
      std::cout << e << ", ";
    std::cout << '\n';
}
