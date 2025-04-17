/*************************************************************************
 * Copyright (C) 2025, Rafał Lalik <rafal.lalik@uj.edu.pl>               *
 * All rights reserved.                                                  *
 *                                                                       *
 * For the licensing terms see LICENSE file.                             *
 * For the list of contributors see README.md file.                      *
 *************************************************************************/

#pragma once

#include "spark/core/types.hpp"

#include <concepts>
// #include <array>
// #include <initializer_list>
// #include <memory>
// #include <string>
// #include <tuple>
// #include <typeinfo>
#include <vector>
//
#include <algorithm>
// // #include <cmath>
// // #include <cstddef>
// #include <sstream>
// // #include <string>
// // #include <ctype.h>

#ifdef __GNUG__
#    include <cxxabi.h>
#endif

namespace spark::utils
{

template<typename T>
auto get_type_name()
{
    return typeid(T).name();
}

template<typename T>
auto get_type_hash() -> std::size_t
{
    return std::hash<std::string> {}(typeid(T).name());
}

template<typename T, typename Tin, typename Tout, std::size_t N>
auto convert_to_array(const std::array<std::pair<T, Tin>, N>& input) -> std::array<std::pair<T, Tout>, N>
{
    std::array<std::pair<T, Tout>, N> result {};
    for (std::size_t i = 0; i < N; ++i) {
        result[i] = {input[i].first, Tout(input[i].second)};
    }
    return result;
}

template<typename T, typename Tin, typename Tout, std::size_t N>
auto convert_to_vector(const std::array<std::pair<T, Tin>, N>& input) -> std::vector<std::pair<T, Tout>>
{
    std::vector<std::pair<T, Tout>> result(N);

    for (std::size_t i = 0; i < N; ++i) {
        result[i] = {input[i].first, Tout(input[i].second)};
    }
    return result;
}

// template<typename T, std::size_t... Is>
// auto convert_to_tuple_impl(std::initializer_list<T> ilst, std::index_sequence<Is...> _unused_idxseq)
// {
//     return std::tuple(*(ilst.begin() + Is)...);
// }
//
// template<typename T>
// auto convert_to_tuple(std::initializer_list<T> ilst)
// {
//     return convert_to_tuple_impl<T>(ilst, std::make_index_sequence<sizeof(ilst)/sizeof(T)> {});
// }

// template<typename T, std::size_t... Is, typename Cont = std::initializer_list<T>>
// auto convert_to_tuple_impl(const Cont& ilst, std::index_sequence<Is...> /*_unused_idxseq*/)
//     requires types::LocatorContainer<Cont>
// {
//     return std::tuple(*(ilst.begin() + Is)...);
// }
//
// template<typename T, typename Cont = std::initializer_list<T>>
// auto convert_to_tuple(const Cont& ilst)
//     requires types::LocatorContainer<Cont>
// {
//     return convert_to_tuple_impl<T>(ilst, std::make_index_sequence<sizeof(ilst) / sizeof(T)> {});
// }

template<typename T, std::size_t... Is, std::size_t N, typename Cont = std::vector<T>>
auto convert_to_tuple_impl(const Cont& ilst, std::index_sequence<Is...> /*_unused_idxseq*/)
    requires types::LocatorContainer<Cont>
{
    return std::tuple(*(ilst.begin() + Is)...);
}

template<typename T, std::size_t N, typename Cont = std::vector<T>>
auto convert_to_tuple(const Cont& ilst)
    requires types::LocatorContainer<Cont>
{
    return convert_to_tuple_impl<T>(ilst, std::make_index_sequence<sizeof(ilst) / sizeof(T)> {});
}

}  // namespace spark::utils
