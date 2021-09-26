#include <array>
#include <utility>
#include <cstdlib>

// std::swap constexpr in c++20
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

    using size_type = size_t;

    constexpr size_type size() const {
        return N;
    }

    T arr[N];
};

template <typename T, size_t N>
using A = std::array<T,N>;

template <typename T, size_t N>
constexpr void quick_sort_impl(A<T, N> &to_sort, size_t left, size_t right)
{
    if (left < right)
    {
        size_t m = left;

        for (size_t i = left + 1; i<right; i++)
            if (to_sort[i]<to_sort[left])
                swap(to_sort[++m], to_sort[i]);

        swap(to_sort[left], to_sort[m]);

        quick_sort_impl(to_sort, left, m);
        quick_sort_impl(to_sort, m + 1, right);
    }
}

template <typename T, size_t N>
constexpr void comb_sort_impl(A<T, N> &to_sort, size_t left, size_t right)
{
    using size_type = typename cx_array<T,N>::size_type;
    size_type gap = to_sort.size ( );
    bool swapped = false;
    while ( ( gap > size_type { 1 } ) or swapped ) {
        if ( gap > size_type { 1 } ) {
            gap = static_cast<size_type> ( gap / 1.247330950103979 );
        }
        swapped = false;
        for ( size_type i = size_type { 0 }; gap + i < static_cast<size_type> ( to_sort.size() ); ++i ) {
            if ( to_sort[ i ] > to_sort[ i + gap ] ) {
                auto swap = to_sort[ i ];
                to_sort[ i ] = to_sort[ i + gap ];
                to_sort[ i + gap ] = swap;
                swapped = true;
            }
        }
    }
}

template <typename T, size_t N>
constexpr A<T, N> sort(A<T, N> cx_array)
{
    auto sorted = cx_array;
    comb_sort_impl(sorted, 0, N);
    return sorted;
}

constexpr A unsorted{5,7,3,4,313,1,8,2,9,0,6,10};
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
