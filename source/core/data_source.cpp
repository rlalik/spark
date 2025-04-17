/*************************************************************************
 * Copyright (C) 2025, Rafał Lalik <rafal.lalik@uj.edu.pl>               *
 * All rights reserved.                                                  *
 *                                                                       *
 * For the licensing terms see LICENSE file.                             *
 * For the list of contributors see README.md file.                      *
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
