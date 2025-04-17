// @(#)lib/base/util:$Id$
// Author: Rafal Lalik  18/11/2017

/*************************************************************************
 * Copyright (C) 2017-2018, Rafał Lalik.                                 *
 * All rights reserved.                                                  *
 *                                                                       *
 * For the licensing terms see $SiFiSYS/LICENSE.                         *
 * For the list of contributors see $SiFiSYS/README/CREDITS.             *
 *************************************************************************/

#pragma once

// #include "spark/data_struct/SCategory.hpp"

#include "spark/utils/conversions.hpp"
#include "spark/external/magic_enum.hpp"

#include <map>

#include <Rtypes.h>
#include <TObject.h>
#include <TString.h>

#include <alpaca/alpaca.h>
#include <spdlog/spdlog.h>

#include <fmt/core.h>
#include <fmt/ranges.h>

/**
 * \class root_file_header
\ingroup lib_core_util

A header object for the category

*/
namespace spark
{

struct root_file_header : public TObject
{
    template<typename ECategories>
    auto serialize() -> void
    {
        constexpr auto entries = magic_enum::enum_entries<ECategories>();
        auto bytes_written = alpaca::serialize(entries, serialized_categories);

        for (const auto& entry : entries) {
            fmt::print("{} = {}\n", entry.second, magic_enum::enum_integer(entry.first));
        }
        fmt::print("serialized {} bytes -> {}\n", bytes_written, serialized_categories);
    }

    template<typename ECategories>
    auto deserialize_and_verify() -> bool
    {
        // Deserialize
        std::error_code err_code;
        auto object = alpaca::deserialize<ECategories>(serialized_categories, err_code);
        if (!err_code) {
            // use object
            constexpr auto entries = magic_enum::enum_entries<ECategories>();
            spdlog::info("Deserialized object: {} = {}", object == entries);
            return object == entries;
        }

        return false;
    }

private:
    std::vector<uint8_t> serialized_categories;
};

}  // namespace spark
