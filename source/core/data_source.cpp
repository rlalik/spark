// @(#)lib/base/datasources:$Id$
// Author: Rafal Lalik  18/11/2017

/*************************************************************************
 * Copyright (C) 2017-2018, Rafał Lalik.                                 *
 * All rights reserved.                                                  *
 *                                                                       *
 * For the licensing terms see $SiFiSYS/LICENSE.                         *
 * For the list of contributors see $SiFiSYS/README/CREDITS.             *
 *************************************************************************/

#include "spark/core/data_source.hpp"

namespace spark
{
/**
 * Add an unpacker for a source
 *
 * \param unpacker the unpacker object
 * \param address addresses which the unpacker must handle
 */
// auto data_source::add_unpacker(const std::shared_ptr<unpacker>& unpacker, const std::vector<uint16_t>& address) ->
// void
// {
//     for (auto addr : address) {
//         spdlog::info("Add unpacker: {:#x}", addr);
//         if (unpackers[addr] != nullptr) {
//             spdlog::critical("Unpacker already exists at address {:#x}", addr);
//             std::abort();
//         }
//         unpackers[addr] = unpacker;
//     }
// }

}  // namespace spark
