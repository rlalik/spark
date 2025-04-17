#pragma once

#include <concepts>
#include <limits>
#include <print>
#include <stdexcept>
#include <type_traits>

#include <fmt/core.h>

namespace spark::types
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
        throw std::out_of_range(fmt::format("Value {} exceeds int max-limit {}", val, std::numeric_limits<int>::max()));
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

}  // namespace spark::types
