// @(#)lib/base:$Id$
// Author: Rafal Lalik  18/11/2017

/*************************************************************************
 * Copyright (C) 2017-2018, Rafał Lalik.                                 *
 * All rights reserved.                                                  *
 *                                                                       *
 * For the licensing terms see $SiFiSYS/LICENSE.                         *
 * For the list of contributors see $SiFiSYS/README/CREDITS.             *
 *************************************************************************/

#pragma once

#include "spark/spark_export.hpp"

#include "spark/parameters/container.hpp"
#include "spark/parameters/database.hpp"

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

namespace spark
{

class container;

using lookup_channel_out_of_range = std::out_of_range;
using lookup_address_out_of_range = std::out_of_range;

struct lookup_cell : public TObject
{
private:
public:
    lookup_cell() = default;

    // lookup_cell(std::initializer_list<int> values)
    // : min {values[0]}
    // , max {values[1]}
    // {
    // }

    lookup_cell(int min_limit, int max_linit)
        : min {min_limit}
        , max {max_linit}
    {
    }

    lookup_cell(const lookup_cell&) = default;
    lookup_cell(lookup_cell&&) = default;

    auto operator=(const lookup_cell&) -> lookup_cell& = default;
    auto operator=(lookup_cell&&) -> lookup_cell& = default;

    ~lookup_cell() override = default;

    int val {0};
    int min {0};
    int max {0};

    ClassDefOverride(lookup_cell, 1)
};

template<typename T>
concept is_lookup_cell = std::same_as<T, lookup_cell> || std::convertible_to<T, lookup_cell>;

/**
 * Describes basic mapped address structure in the framework. Consist of three
 * address components:
 */
struct SPARK_EXPORT lookup_channel : public TObject
{
    lookup_channel() = default;

    template<is_lookup_cell... Cells>
    lookup_channel(Cells&&... args)
        : addr {lookup_cell {args}...}
    {
    }

    lookup_channel(const lookup_channel&) = default;
    lookup_channel(lookup_channel&&) = delete;

    auto operator=(const lookup_channel&) -> lookup_channel& = delete;
    auto operator=(lookup_channel&&) -> lookup_channel& = default;

    ~lookup_channel() override = default;

    std::vector<lookup_cell> addr;

    /**
     * Prints the parameters to the mapping part of the entry. When overriding
     * this class, the subclass must address all values in the channel. The output
     * format should be consistent with the read format parsing.
     *
     * \param newline puts newline on the end
     * \param prefix a text which should be displayed before the content of the
     * channel params. If prefix is empty, then
     */
    virtual auto print(bool newline = true, const char* prefix = nullptr) const -> void;

    ClassDefOverride(lookup_channel, 1)
};

/**
 * Each address in the lookup table has corresponding object of SLookupAddress.
 * The object is quite generic thus do not need to be subclass in the detectors.
 * Two functions setChannel() and getChannel() provide actual functionality
 * required by the user.
 */
class SPARK_EXPORT lookup_address : public TObject
{
private:
    size_t addr {};                          ///< board address
    size_t nchan {};                         ///< number of channels
    std::map<int, lookup_channel> channels;  ///< array of channels

    friend class TCollectionProxyInfo;
    template<class T1, class T2>
    friend struct std::pair;

public:
    lookup_address() = default;  // FIXME find way to make it private
    /**
     * Constructor
     *
     * \param addr,n_channels address and channels number
     */
    lookup_address(size_t address, size_t n_channels);

    lookup_address(const lookup_address&) = default;
    lookup_address(lookup_address&&) = default;

    auto operator=(const lookup_address&) -> lookup_address& = default;
    auto operator=(lookup_address&&) -> lookup_address& = default;

    ~lookup_address() override = default;

    /**
     * Get the channel object for given channel number. If needed, creates default object. If channel number is outside
     * the allowed range, the executions aborts.
     *
     * \param channel channel number
     * \return channel mapping object
     */
    auto get_channel_or_create(size_t channel, std::function<lookup_channel()>& channel_builder) -> lookup_channel&
    {
        if (channel >= nchan) {
            throw lookup_channel_out_of_range(
                fmt::format("Channel {} exceeds size of {} channels for lookup table of {}", channel, nchan, addr));
        }
        try {
            return channels.at(channel);
        } catch (const std::out_of_range& e) {
            channels[channel] = channel_builder();
        }

        return channels.at(channel);
    }

    virtual auto print() const -> void;

    friend class lookup_table;

    ClassDefOverride(lookup_address, 1)
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
class SPARK_EXPORT lookup_table : public container
{
protected:
    std::string container_name;               ///< container name
    size_t a_min {};                          ///< lower boundary of address range
    size_t a_max {};                          ///< upper boundary of address range
    size_t channels {};                       ///< maximal number of channels
    bool is_init {};                          ///< set if container was initialized

    std::map<size_t, lookup_address> boards;  ///< array of boards in a given range

    /**
     * Initialize lookup table from container named #container for given address
     * range of \p addr_min, \p addr_max. Defines that each board has no more channeel
     * than specified in #channels.
     *
     * \param name container name
     * \param addr_min,addr_max lower and upper range of boards addresses
     * \param channels maximal number of channels in each board
     */

    lookup_table(std::string_view name, size_t addr_min, size_t addr_max, size_t channels);

public:
    template<is_lookup_cell... Cells>
    static auto build(std::string_view name, size_t addr_min, size_t addr_max, size_t channels, Cells&&... cells)
        -> lookup_table
    {
        auto lu_table = lookup_table(name, addr_min, addr_max, channels);
        lu_table.make_channel_template = [&]() -> lookup_channel { return {cells...}; };
        return lu_table;
    }

    lookup_table() = default;

    /// Do not allow for copying the lookup table
    lookup_table(const lookup_table&) = default;
    lookup_table(lookup_table&&) = default;

    auto operator=(const lookup_table&) -> lookup_table& = delete;
    auto operator=(lookup_table&&) -> lookup_table& = delete;

    // destructor
    ~lookup_table() override = default;

    auto get_address(size_t addr, size_t chan) -> lookup_channel&;

    auto name() -> std::string_view { return container_name; }

    auto print() const -> void override;

protected:
    auto from_view(container::ascii_snapshot& view) -> bool override;
    auto to_container(container::ascii_snapshot& view) const -> void override;

    friend class parameters_ascii_source;

private:
    /**
     * We need mechanism to provide default object of lookup_channel.
     *
     * \return default object of lookup_channel class
     */
    std::function<lookup_channel()> make_channel_template;

    ClassDefOverride(lookup_table, 1)
};

}  // namespace spark

template<>
struct fmt::formatter<spark::lookup_cell> : formatter<int>
{
    auto format(const spark::lookup_cell& cell, format_context& ctx) const -> format_context::iterator
    {
        return formatter<int>::format(cell.val, ctx);
    }
};
