/*************************************************************************
 * Copyright (C) 2025, Rafał Lalik <rafal.lalik@uj.edu.pl>               *
 * All rights reserved.                                                  *
 *                                                                       *
 * For the licensing terms see LICENSE file.                             *
 * For the list of contributors see README.md file.                      *
 *************************************************************************/

#pragma once

#include "spark/spark_export.hpp"

#include "spark/core/types.hpp"
#include "spark/parameters/container.hpp"
#include "spark/utils/data_scanner.hpp"

#include <concepts>
#include <functional>
#include <map>
#include <stdexcept>
#include <string>
#include <string_view>
#include <vector>

#include <TNamed.h>
#include <TObject.h>

#include <fmt/format.h>
#include <fmt/ranges.h>

namespace spark
{

struct SPARK_EXPORT lookup_container : public container
{
    using container::container;
};

/**
 * The lookup table maps physical front-end board address and channels to
 * abstract addressing used in the framework. The mapping contains then two
 * parts:
 * * mapped addresses (2 components)
 * * mapping (variable number of values).
 *
 * The default implementation provides three mapping addresses: module, layer,
 * channel. The respective entry in the lookup table looks following:
 *
 *     address channel        module layer channel
 *
 * where:
 *  * `address` - a hex value of board address (in the unpacker it is equivalent
 *    to subevent id.
 *  * `channel` - physical channel on the board
 *  * `module` - abstract module value
 *  * `layer` - abstract layer value
 *  * `channel` - abstract channel value
 *
 * Respective detectors can use this scheme or provide it's own
 * mapping. The virtual mapping for each detector/system is an individual
 * issue, but at least one dimension of mapping must exists.
 *
 * While parsing the lookup table, the parser creates object of SLookupBoard for
 * each unique address appearing in the table, which fit the address range
 * defined by #a_min, #a_max.
 * See documentation of existing detectors in the framework for details.
 */
template<typename Address, typename Row>
//    requires(spark::concepts::IsTabularPair<Address, Row>)  // FIXME crashes browser
class SPARK_EXPORT lookup_table : public lookup_container
{
protected:
    size_t a_min {};                 ///< lower boundary of address range
    size_t a_max {};                 ///< upper boundary of address range
    size_t channels {};              ///< maximal number of channels

    std::map<Address, Row> records;  ///< array of boards in a given range

    /**
     * Initialize tabular table from container named #container for given address
     * range of \p addr_min, \p addr_max. Defines that each board has no more channeel
     * than specified in #channels.
     *
     * \param name container name
     */
    lookup_table(std::string_view name, size_t addr_min, size_t addr_max, size_t channels)
        : lookup_container(TString(name), TString(name))
        , a_min(addr_min)
        , a_max(addr_max)
        , channels(channels)
    {
    }

public:
    static auto build(std::string_view name,
                      size_t addr_min,
                      size_t addr_max,
                      size_t channels,
                      std::string_view _fmt_addr,
                      std::string_view _fmt_row) -> lookup_table<Address, Row>
    {
        auto ret = lookup_table {name, addr_min, addr_max, channels};
        ret.fmt_addr = _fmt_addr;
        ret.fmt_row = _fmt_row;
        return ret;
    }

    // auto lu_table = lookup_table(name, addr_min, addr_max, channels);
    // lu_table.make_channel_template = [&]() -> lookup_channel { return {cells...}; };
    // return lu_table;

    lookup_table() = default;

    /// Do not allow for copying the tabular table
    lookup_table(const lookup_table&) = default;
    lookup_table(lookup_table&&) = default;

    auto operator=(const lookup_table&) -> lookup_table& = delete;
    auto operator=(lookup_table&&) -> lookup_table& = delete;

    // destructor
    ~lookup_table() override = default;

    /**
     * Return tabular channel object based on the board address and channel
     * number. Internally calls SLookupBoard::getChannel(). See its documentation
     * for further information.
     *
     * \param addr board address
     * \param chan channel number
     * \return channel object or nullptr if boad or channel doesn't exists.
     */
    auto get(Address addr) -> Row&
    {
        if (std::get<0>(addr) > a_max or std::get<0>(addr) < a_min) {
            throw lookup_address_out_of_range(fmt::format(
                "Address {} exceeds range of {}-{} for lookup table {}", addr, a_min, a_max, TNamed::GetName()));
        }

        if (std::get<1>(addr) >= channels or std::get<0>(addr) < 0) {
            throw lookup_channel_out_of_range(fmt::format(
                "Address {} exceeds channel range of {}-{} for lookup table {}", addr, 0, channels, TNamed::GetName()));
        }

        return records.at(addr);
    }

    auto at(Address addr) -> Row& { return records.at(addr); }

    auto insert(Address addr, Row row) -> void { records.emplace(addr, row); }

    auto name() -> std::string_view { return TNamed::GetName(); }

    /**
     * Prints all existing boards and its channel tabular tables
     */
    auto print() const -> void override
    {
        for (const auto& [key, row] : records) {
            fmt::print("{} : {}\n", key, row);
        }
    }

protected:
    /**
     * Initialize tabular from the ASCII view. The container must exists otherwise exception is thrown.
     *
     * \param view ascii container view
     */
    auto from_view(container::ascii_snapshot& view) -> bool override
    {
        for (const auto& line : view) {
            records.emplace(scanner(GetName(), line, fmt_addr, fmt_row));
        }

        return true;
    }

    /**
     * Export tabular table to the container. The function is called by the
     * SDatabase class. The container must be registered in the Parameters
     * Manager. Otherwise an exception is thrown.
     *
     * \param sc container object
     */
    auto to_container(container::ascii_snapshot& view) const -> void override
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
        //     //         tabular_channel* chan = boards[idx]->get_channel(c);
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

    friend class parameters_ascii_source;

private:
    utils::scan::tuple_pair_scanner<Address, Row> scanner;
    std::string fmt_addr;  //!
    std::string fmt_row;   //!

    ClassDefOverride(lookup_table, 1)
};

}  // namespace spark
