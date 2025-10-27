/*************************************************************************
 * Copyright (C) 2025, Rafał Lalik <rafal.lalik@uj.edu.pl>               *
 * All rights reserved.                                                  *
 *                                                                       *
 * For the licensing terms see LICENSE file.                             *
 * For the list of contributors see README.md file.                      *
 *************************************************************************/

#pragma once

// #include "spark/data_struct/SCategory.hpp"

#include "spark/external/magic_enum.hpp"
#include "spark/utils/conversions.hpp"

#include <map>
#include <print>

#include <Rtypes.h>
#include <TObject.h>
#include <TString.h>

#include <alpaca/alpaca.h>
#include <spdlog/spdlog.h>

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

        // for (const auto& entry : entries) {
        //     std::print("{} = {}\n", entry.second, magic_enum::enum_integer(entry.first));
        // }
        // std::print("serialized {} bytes -> {}\n", bytes_written, serialized_categories);
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
