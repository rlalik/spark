
// @(#)lib/base:$Id$
// Author: Rafal Lalik  18/11/2017

/*************************************************************************
 * Copyright (C) 2017-2018, Rafał Lalik.                                 *
 * All rights reserved.                                                  *
 *                                                                       *
 * For the licensing terms see $SiFiSYS/LICENSE.                         *
 * For the list of contributors see $SiFiSYS/README/CREDITS.             *
 *************************************************************************/

#include "spark/parameters/lookup.hpp"

#include "spark/core/exceptions.hpp"
#include "spark/parameters/container.hpp"

#include <cassert>
#include <cstdio>
#include <stdexcept>
#include <string_view>
#include <tuple>
#include <utility>

#include <TString.h>

#include <fmt/base.h>
#include <fmt/format.h>
#include <fmt/ranges.h>
#include <scn/scan.h>
#include <spdlog/spdlog.h>

/**
 * \class lookup
\ingroup lib_core

SLookup is an abstract class to hold container and geometry parameters.

It must be derivated and pure virtual members defined.

The parameters are parsed from text file in SLookupManager and stored in the
SLookupContainer. The getParam() method reads content of the SLookupContainer and
fills variables inside the SLookup object. The putParam method allows to update
parameters in the container and write to param file.

\sa SFibersCalibratorPar
\sa SFibersDigitizerPar
\sa SFibersGeomPar
*/

namespace spark
{

void lookup_channel::print(bool newline, const char* /*prefix*/) const
{
    fmt::print("{:5d}", fmt::join(addr, "  "));
    if (newline) {
        fmt::print("{:c}", '\n');
    }
}

lookup_address::lookup_address(size_t address, size_t n_channels)
    : addr(address)
    , nchan(n_channels)
{
}

/**
 * Prints all registered channels for given board.
 */
void lookup_address::print() const
{
    for (const auto& [key, val] : channels) {
        fmt::print(" 0x{:04x} {:3d} ", addr, key);
        val.print(/*newline=*/true);
    }
}

lookup_table::lookup_table(std::string_view name, size_t addr_min, size_t addr_max, size_t channels)
    : container(TString(name), TString(name))
    , container_name(name)
    , a_min(addr_min)
    , a_max(addr_max)
    , channels(channels)

{
}

/**
 * Initialize lookup table from the container. The container must exists
 * otherwise exception is thrown.
 *
 * \param container container object
 */
auto lookup_table::from_view(container::ascii_snapshot& view) -> bool
{
    for (const auto& line : view) {
        spdlog::info("Read line: {}", line);
        auto result = scn::scan<int, int>(line, "{:x} {}");
        auto [addr, chan] = result->values();

        auto& entry = get_address(addr, chan);

        const auto entry_size = entry.addr.size();
        size_t cnt {0};

        auto range = result->range();
        while (true) {
            auto result_sub = scn::scan<int>(range, "{}");
            if (!result_sub) {
                break;
            }

            range = result_sub->range();

            if (cnt > entry_size) {
                spdlog::critical("Size mismatch for {}", entry_size);
                throw "DUPA";
            }

            auto [vchan] = result_sub->values();

            spdlog::info("Access {}/{} = {}", cnt, entry_size, vchan);
            entry.addr[cnt].val = vchan;

            cnt++;
        }

        if (cnt < entry_size) {
            spdlog::critical("Read not enough values for channel length {}", entry_size);
            throw spark::container_parsing_error(GetName());
        }
    }

    is_init = true;

    return true;
}

/**
 * Export lookup table to the container. The function is called by the
 * SDatabase class. The container must be registered in the Parameters
 * Manager. Otherwise an exception is thrown.
 *
 * \param sc container object
 */
void lookup_table::to_container(container::ascii_snapshot& view) const
{
    // sc->lines.clear(); TODO
    //
    // const int len = 1024;
    // char buff[len];
    //
    // for (uint addr = a_min; addr < a_max; ++addr) {
    //     uint idx = addr - a_min;
    //     // if (boards[idx]) { FIXME
    //     //     for (uint c = 0; c < channels; ++c) {
    //     //         lookup_channel* chan = boards[idx]->get_channel(c);
    //     //         if (chan) {
    //     //             snprintf(buff, len, "  %#x  %3d\t\t", addr, c);
    //     //             std::string s;
    //     //             s.reserve(128);
    //     //             s += buff;
    //     //             chan->write(buff, len);
    //     //             s += buff;
    //     //             sc->lines.push_back(s);
    //     //         }
    //     //     }
    //     // }
    // }
}

/**
 * Return lookup channel object based on the board address and channel
 * number. Internally calls SLookupBoard::getChannel(). See its documentation
 * for further information.
 *
 * \param addr board address
 * \param chan channel number
 * \return channel object or nullptr if boad or channel doesn't exists.
 */
auto lookup_table::get_address(size_t addr, size_t chan) -> lookup_channel&
{
    if (addr > a_max or addr < a_min) {
        throw lookup_address_out_of_range(
            fmt::format("Address {} exceeds range of {}-{} for lookup table {}", addr, a_min, a_max, container_name));
    }

    try {
        return boards.at(addr).get_channel_or_create(chan, make_channel_template);
    } catch (const std::out_of_range& e) {
        boards.emplace(std::piecewise_construct, std::forward_as_tuple(addr), std::forward_as_tuple(addr, channels));
        return boards.at(addr).get_channel_or_create(chan, make_channel_template);
    };
}

/**
 * Prints all existing boards and its channel lookup tables
 */
auto lookup_table::print() const -> void
{
    fmt::print("[{:s}]\n", container_name.c_str());
    for (const auto& brd : boards) {
        brd.second.print();
    }
}

}  // namespace spark
