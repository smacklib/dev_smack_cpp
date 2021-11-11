#include <array>
#include <algorithm>
#include <cstdlib>
#include <string_view>
#include <type_traits>
#include <utility>
#include <vector>

#include "absl/strings/str_split.h"

// std::swap is constexpr in c++20
template<class T>
constexpr void swap_(T& l, T& r)
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
                swap_(to_sort[++m], to_sort[i]);

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
                swap_( to_sort[ i ], to_sort[ i + gap ] );
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

#if 0
constexpr std::array unsorted{5,7,3,4,313,1,8,2,9,0,6,10,-121};
#else
using namespace std::literals::string_view_literals;
constexpr std::array unsorted{"blue"sv,"yellow"sv,"black"sv,"white"sv,"cyan"sv,"pink"sv,"magenta"sv,"grey"sv,"cobalt"sv,"white"sv};
#endif
constexpr auto sorted = sort(unsorted);

template<typename R, typename T, typename Func>
static auto map_array(T& tpl, Func func) {
    constexpr auto i =
        std::make_index_sequence<std::tuple_size_v<T>>{};

    return map_tuple_<R>(tpl, func, i);
}

template <typename A>
struct cx_map {
    //using pair_t_ = std::pair<K, V>;
    //using Es_t_ = std::common_type_t<Es ...>;
    //static_assert(std::is_same_v<pair_t_, int>, "Pass pair<K,V>.");

    A elements_;

    using element_type = typename A::value_type;
    using idx_type = typename A::size_type;
    using key_type = typename element_type::first_type;
    using val_type = typename element_type::second_type;

    cx_map(const A& a) :
        elements_{ sort(a) }
    {
    }

    //template <typename T>
    //struct Comp
    //{
    //    bool operator() (const T& s, int i) const { return s.number < i; }
    //    bool operator() (int i, const S& s) const { return i < s.number; }
    //};


    idx_type find(key_type key)
    {
        const auto p2 = std::equal_range(vec.begin(), vec.end(), 2, Comp{});
        std::equal_range(elements_.begin(), elements_.end(), key);
    }

    //V get(idx_type e)
    //{

    //}
    // std__binary_search
};
 

#include <iostream>
int main()
{
    std::vector<std::string> v1 = absl::StrSplit("a, b, c", ", ");
    std::cout << "vsize=" << v1.size() << std::endl;
    std::pair<std::string, std::string> p = absl::StrSplit("a,b,c", ',');
    std::cout << "p1=" << p.first << std::endl;
    std::cout << "p2=" << p.second << std::endl;

    std::array xarray{
        std::make_pair("Febrary"sv, 2),
        std::make_pair("September"sv, 9),
        std::make_pair("January"sv, 1),
        std::make_pair("August"sv, 8) 
    };

    cx_map m{ xarray };

    //using p = std::pair<std::string_view, int>;

    //p jan = std::make_pair("January"sv, 1);

    //cx_map<std::string_view, int> xmap{ jan };

    //    std::make_pair("January"sv, 1),
    //        std::make_pair("Febrary"sv, 2),
    //        std::make_pair("September"sv, 9),
    //        std::make_pair("August"sv, 8) 
    //};

    std::cout << "unsorted: ";
    for(auto const& e : unsorted) 
      std::cout << e << ", ";
    std::cout << '\n';

    std::cout << "sorted: ";
    for(auto const& e : sorted) 
      std::cout << e << ", ";
    std::cout << '\n';
}
