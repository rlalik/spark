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
#include "spark/core/unpacker.hpp"

#include <cstdint>
#include <map>
#include <memory>
#include <tuple>
#include <vector>

#include <spdlog/spdlog.h>

namespace spark
{

using types::hwaddr_t;
using types::vaddr_t;

// Main utility to generate sequence from A to B-1
template<vaddr_t A, vaddr_t B, typename = std::make_index_sequence<B - A>>
struct make_address_range;

template<vaddr_t A, vaddr_t B, std::size_t... Is>
struct make_address_range<A, B, std::index_sequence<Is...>>
{
    static_assert(A <= B, "A must be less than or equal to B");
    using type = std::integer_sequence<vaddr_t, (A + Is)...>;
};

/**
 * \class data_source
 * \ingroup lib_core_datasources
 * This source read events from hld file. For each readout subevent address,
 * a respective unpacker is called. If unpacker is missing, the program is
 * aborted. To ignore the address, nullptr object can be passed.
 */
class SPARK_EXPORT data_source
{
public:
    data_source() = default;

    data_source(const data_source&) = delete;
    data_source(data_source&&) = delete;

    auto operator=(const data_source&) -> data_source& = delete;
    auto operator=(data_source&&) -> data_source& = delete;

    virtual ~data_source() = default;

    /// Open the source
    /// \return success
    virtual auto open() -> bool = 0;
    /// Close the source
    /// \return success
    virtual auto close() -> bool = 0;
    /// Read evnt from source
    /// \return success
    virtual auto read_current_event() -> bool = 0;

    /// Set index of the current event
    /// \param i new index of the current event
    auto set_current_event(uint64_t event) -> void { current_event = event; }

    /// Get index of the current event
    /// \return current event index
    auto get_current_event() const -> uint64_t { return current_event; }

    /**
     * Add an unpacker for a source
     *
     * \param unpacker the unpacker object
     * \param address addresses which the unpacker must handle
     */
    template<typename... Vaddrs>
    auto add_unpacker(const std::shared_ptr<unpacker>& unpacker, Vaddrs... vaddrs) -> void
    {
        static_assert((std::is_convertible_v<Vaddrs, vaddr_t> && ...), "All addresses must be vaddr_t convertible");

        // for (auto addr : std::forward_as_tuple(vaddrs...)) {
        for (auto addr : {vaddrs...}) {
            spdlog::info("    -> Add unpacker: {:#x}", addr);
            if (unpackers[addr] != nullptr) {
                spdlog::critical("Unpacker already exists at address {:#x}", addr);
                std::abort();
            }
            unpackers[addr] = unpacker;
        }
    }

    auto count_unpackers() const -> size_t { return unpackers.size(); }

    auto get_unpacker(uint16_t addr) const -> unpacker*
    {
        if (unpackers.contains(addr)) {
            return unpackers.at(addr).get();
        }

        return nullptr;
    }

    /****************** Hardware managing ******************/

    /**
     * Map the Hardware Address to Virtual address.
     *
     * HW address is the phsyica address under which a board can be identified in real world.
     * Virtual Address is the address by which the board is recognized by the system.
     *
     * @param hwaddr HW address
     * @param vaddr assigned virtual address
     */
    constexpr auto register_hw_address(hwaddr_t hwaddr, vaddr_t vaddr) -> void
    {
        hw_address_map.insert_or_assign(hwaddr, vaddr);
    }

    /**
     * Returns the virtual address for hiven hw. Throws if address not registsred.
     *
     * @param hwaddr HW address
     * @return virtual address
     */
    constexpr auto get_vadrr(hwaddr_t hwaddr) -> vaddr_t { return hw_address_map.at(hwaddr); }

    /******************* Events handling *******************/
    /**
     * Get number of events in the source.
     *
     * Not every source allows easy lookup for events number, then the return value is none.
     *
     * @return no of events number of events in the source or {} if number cannot be determined
     */
    auto get_no_events() -> std::optional<uint64_t> { return no_of_events; }

protected:
    /**
     * Set number of events in the source. Not every source allows easy lookup for events number.
     *
     * @param no_events number of events in the source
     */
    auto set_no_events(uint64_t no_events) { no_of_events = no_events; }

private:
    /****************** Hardware managing ******************/
    std::map<hwaddr_t, vaddr_t> hw_address_map;               ///< Stores HW address and its mapping to virtual address
    std::map<uint16_t, std::shared_ptr<unpacker>> unpackers;  ///< store unpackers and its addresses
    uint64_t current_event {0};                               ///< current event index
    std::optional<uint64_t> no_of_events;
};

}  // namespace spark
