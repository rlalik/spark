/*************************************************************************
 * Copyright (C) 2025, Rafał Lalik <rafal.lalik@uj.edu.pl>               *
 * All rights reserved.                                                  *
 *                                                                       *
 * For the licensing terms see LICENSE file.                             *
 * For the list of contributors see README.md file.                      *
 *************************************************************************/

#pragma once

#include <concepts>
#include <format>
#include <limits>
#include <print>
#include <stdexcept>
#include <type_traits>

namespace spark
{

namespace concepts
{

template<typename T>
concept TupleLike = requires { typename std::tuple_size<std::remove_cvref_t<T>>; }
    && requires { requires std::tuple_size<std::remove_cvref_t<T>>::value >= 0; };

template<typename A, typename B>
concept IsTabularPair = TupleLike<A> && TupleLike<B>;

}  // namespace concepts

namespace types
{
using dim_t = size_t;
using loc_t = size_t;

/******************************************/
/********** HW and address types **********/
/******************************************/

using hwaddr_t = uint64_t;
using vaddr_t = uint16_t;

template<typename T>
concept HWaddr = (std::is_same_v<T, hwaddr_t>);

template<typename T>
concept Vaddr = (std::is_same_v<T, vaddr_t>);

/******************************************/
/**************** Locators ****************/
/******************************************/

template<class ContainerType, typename T = size_t>
concept LocatorContainer = requires(ContainerType cont) {
    // requires std::regular<ContainerType>;
    requires std::same_as<typename ContainerType::value_type, T>;
};

/******************************************/
/************** Conversions ***************/
/******************************************/

constexpr auto to_int(std::unsigned_integral auto val) -> int
{
    if (val > std::numeric_limits<int>::max()) {
        throw std::out_of_range(std::format("Value {} exceeds int max-limit {}", val, std::numeric_limits<int>::max()));
    }
    return static_cast<int>(val);
}

constexpr auto dim_t2int(dim_t val) -> int
{
    return (val <= std::numeric_limits<int>::max()) ? static_cast<int>(val) : -1;
}

constexpr auto int2dim_t(int val) -> dim_t
{
    return (val < 0) ? __INT_MAX__ : static_cast<dim_t>(val);
}

constexpr auto size_t2int(size_t val) -> int
{
    return (val <= std::numeric_limits<int>::max()) ? static_cast<int>(val) : -1;
}

constexpr auto int2size_t(int val) -> size_t
{
    return (val < 0) ? __SIZE_MAX__ : static_cast<size_t>(val);
}
}  // namespace types
}  // namespace spark
