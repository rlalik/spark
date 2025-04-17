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

/**
 * Stores content of the container read out from the disk.
 */
struct SPARK_EXPORT container_dump
{
    std::vector<std::string> lines;  ///< lines read out from the container
    bool updated {false};            ///< flag -- was container updated?

    template<typename... Lines>
    container_dump(Lines... lines)
        : lines(lines...)
    {
    }

    /**
     * Print container content.
     *
     * \param name name to display
     */
    auto print(std::string name = std::string()) const -> void
    {
        fmt::println("Container Table [{}]", name);
        for (auto line : lines) {
            fmt::println("{}", line);
        }
    }
};

class SPARK_EXPORT parameters_ascii_source : public parameters_source
{
public:
    explicit parameters_ascii_source(const std::string& source);
    explicit parameters_ascii_source(std::string&& source);

    ~parameters_ascii_source() override = default;

    auto print() const -> void override;

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
    auto parse_source() -> bool;

    std::string source;                                                 ///< ascii file name
    std::map<std::string, container::ascii_snapshot> containers_views;  ///< Containers views
};

}  // namespace spark
