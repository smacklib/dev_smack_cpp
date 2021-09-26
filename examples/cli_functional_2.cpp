#include <array>
#include <utility>
#include <cstdlib>

// std::swap is constexpr in c++20
template<class T>
constexpr void swap(T& l, T& r)
{
    T tmp = std::move(l);
    l = std::move(r);
    r = std::move(tmp);
}

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
constexpr void comb_sort_impl_2(A<T, N> &to_sort, size_t left, size_t right)
{
    using S = typename A<T,N>::size_type;

    auto gap = to_sort.size();
    auto shrink = 1.3;
    bool sorted = false;
    
    while ( ! sorted ) 
    {
        gap = static_cast<S> ( gap / shrink );

        if ( gap <= 1 ) {
            gap = 1;
            sorted = true;
        }

        for ( S i = 0 ; i + gap < to_sort.size() ; ++i )
        { 
            if ( to_sort[ i ] > to_sort[ i + gap ] ) {
                swap( to_sort[ i ], to_sort[ i + gap ] );
                sorted = false;
            }
        }
    }
}

template <typename T, size_t N>
constexpr A<T, N> sort(A<T, N> cx_array)
{
    auto sorted = cx_array;
    comb_sort_impl_2(sorted, 0, N);
    return sorted;
}

constexpr A unsorted{5,7,3,4,313,1,8,2,9,0,6,10,-121};
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
