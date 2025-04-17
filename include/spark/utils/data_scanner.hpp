/*************************************************************************
 * Copyright (C) 2025, Rafał Lalik <rafal.lalik@uj.edu.pl>               *
 * All rights reserved.                                                  *
 *                                                                       *
 * For the licensing terms see LICENSE file.                             *
 * For the list of contributors see README.md file.                      *
 *************************************************************************/

#pragma once

#include <string_view>

#include <scn/scan.h>

namespace spark::utils::scan
{

template<typename... Ts>
constexpr auto generate_fmt_string()
{
    constexpr auto internal_width = 5u;  // width of "{:_} "
    constexpr auto count = sizeof...(Ts);
    constexpr auto size = count == 0 ? 1 : count * internal_width;
    std::array<char, size> result;

    std::size_t idx = 0;
    for (std::size_t i = 0; i < count; ++i) {
        result[idx++] = '{';
        result[idx++] = '}';
        if (i < count - 1) {
            result[idx++] = ' ';
        } else {
            result[idx++] = '\0';
        }
    }
    return result;
}

template<typename...>
struct tuple_pair_scanner
{
};

template<typename... T1, template<typename...> class U, typename... T2, template<typename...> class V>
struct tuple_pair_scanner<U<T1...>, V<T2...>>
{
    // auto operator()(auto input) -> std::pair<std::tuple<T1...>, std::tuple<T2...>>
    // {
    //     auto res_addr = scn::scan<T1...>(input, scn::runtime_format(generate_fmt_string<T1...>().data()));
    //     auto res_row = scn::scan<T2...>(res_addr->range(), scn::runtime_format(generate_fmt_string<T2...>().data()));
    //     return {res_addr->values(), res_row->values()};
    // }

    auto operator()(std::string_view name, auto input, std::string_view fmt_addr, std::string_view fmt_row)
        -> std::pair<std::tuple<T1...>, std::tuple<T2...>>
    {
        auto res_addr = scn::scan<T1...>(input, scn::runtime_format(fmt_addr));
        auto res_row = scn::scan<T2...>(res_addr->range(), scn::runtime_format(fmt_row));

        if (res_row->end() - res_row->begin()) {
            throw container_parsing_error(name);
        }
        return {res_addr->values(), res_row->values()};
    }
};
}  // namespace spark::utils::scan
