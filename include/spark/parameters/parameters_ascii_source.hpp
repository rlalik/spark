/*
 * <one line to give the program's name and a brief idea of what it does.>
 * Copyright (C) 2017  <copyright holder> <email>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

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

#include "spark/core/exceptions.hpp"
#include "spark/parameters/parameters_source.hpp"

#include <fstream>
#include <map>
#include <memory>
#include <string>

#include <spdlog/spdlog.h>

namespace spark
{

struct container_dump;

/**
 * Stores content of the container read out from the disk.
 */
struct SPARK_EXPORT container_dump
{
    std::vector<std::string> lines;  ///< lines read out from the container
    bool updated {false};            ///< flag -- was container updated?

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

    template<typename T, typename... Args>
    auto build_container(std::string_view name, Args... args) -> void
    {
        try {
            if (!dynamic_cast<T*>(containers_objs.at(std::string(name)).get())) {
                throw spark::container_invalid_type(name);
            }
        } catch (const std::out_of_range&) {
            auto& the_view = containers_views.at(std::string(name));

            auto cont_ptr = new T(T::build(name, args...));
            auto cont = std::shared_ptr<T>(std::move(cont_ptr));
            if (cont->from_view(the_view)) {
                auto ret = cont.get();
                containers_objs.insert({std::string(name), std::move(cont)});
            } else {
                throw spark::container_parsing_error(name);
            }
        }
    }

    template<typename T>
    auto get_container(std::string_view name, long /*_unused_runid*/) -> T*
    {
        return dynamic_cast<T*>(containers_objs.at(std::string(name)).get());
    }

private:
    auto parse_source() -> bool;

    std::string source;                                                 ///< ascii file name
    std::map<std::string, container::ascii_snapshot> containers_views;  ///< Containers views
    std::map<std::string, std::shared_ptr<container>> containers_objs;  ///< Containers mirrors
};

}  // namespace spark
