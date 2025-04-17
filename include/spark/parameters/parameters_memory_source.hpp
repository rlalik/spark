/*************************************************************************
 * Copyright (C) 2025, Rafał Lalik <rafal.lalik@uj.edu.pl>               *
 * All rights reserved.                                                  *
 *                                                                       *
 * For the licensing terms see LICENSE file.                             *
 * For the list of contributors see README.md file.                      *
 *************************************************************************/

#pragma once

#include "spark/spark_export.hpp"

#include "spark/core/exceptions.hpp"
#include "spark/parameters/parameters_source.hpp"

#include <fstream>
#include <map>
#include <memory>
#include <string>

#include <spdlog/spdlog.h>

namespace spark
{

class parameters_memory_source : public parameters_source
{
public:
    ~parameters_memory_source() override = default;

    auto add_view(std::string_view name, container::ascii_snapshot lines) -> void
    {
        add_provided_container_name(name);
        containers_views.emplace(name, std::move(lines));
    }

    auto print() const -> void override { spdlog::info("=== Memory Source Info ==="); }

    virtual auto build_in_place(std::string_view name, container* cont) -> bool override
    {
        try {
            auto& the_view = containers_views.at(std::string(name));
            if (cont->from_view(the_view)) {
                return true;
            } else {
                throw spark::container_parsing_error(name);
            }
        } catch (const std::out_of_range&) {
            return false;
        }

        return false;
    }

private:
    std::map<std::string, container::ascii_snapshot> containers_views;
};

}  // namespace spark
