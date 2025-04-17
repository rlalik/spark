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

#include "spark/parameters/parameters_source.hpp"

#include <algorithm>
#include <concepts>
#include <list>
#include <map>
#include <memory>
#include <string>
#include <string_view>

namespace spark
{

class SPar
{
};  // FIXME forward decl only

class SParContainer
{
};  // FIXME forward decl only

class SVirtualCalContainer
{
};  // FIXME forward decl only

class lookup_table;

class SPARK_EXPORT database
{
public:
    // constructors
    database() = default;

    database(database const&) = delete;
    database(database const&&) = delete;

    auto operator=(database const&) -> database& = delete;
    auto operator=(database const&&) -> database& = delete;

    ~database() = default;

    // methods
    /// Set parameters source
    /// \param source source file name
    auto add_source(parameters_source* source) -> void { sources.push_front(source); }

    /// Set parameters destination
    /// \param target destination file name
    auto set_target(parameters_source* target) -> void { this->target = target; }

    /**
     * Write all containers to destination file. Internally it creates a list of
     * containers and calls writeContainers();
     */
    auto write_destination() -> void;

    /**
     * Write selected containers to the destination file.
     *
     * \param names vector of container names
     */
    auto write_containers(const std::vector<std::string>& names) -> void;

    /**
     * Add new parameter container.
     *
     * \param name container name
     * \param par parameter container object
     * \return success
     */
    auto add_par_container(std::string_view name, std::unique_ptr<SPar>&& par) -> bool;

    /**
     * Get parameter container by name.
     *
     * \param name container name
     * \return pointer to container
     */
    auto get_par_container(std::string_view name) -> SPar*;

    /**
     * Add new lookup table container.
     * If container with given name was not registered, function throws.
     *
     * \param name container name
     * \param lu lookup table object
     * \return success
     */
    template<typename T>
        requires requires { std::is_base_of<lookup_table, T>(); }
    auto add_lookup_container(std::string_view /*name*/) -> bool
    {
        for (auto& src : sources) {
            if (src) {
                return true;
            }
        }

        // lut_containers.insert(std::make_pair(name, std::move(lu)));
        return false;
    }

    /**
     * Get lookup table by name.
     *
     * \param name container name
     * \return pointer to container
     */
    template<typename T, typename... Args>
        requires requires { std::is_base_of<lookup_table, T>(); }
    auto get_lookup_container(std::string_view /*name*/, Args... /*args*/) -> lookup_table*
    {
        return nullptr;
    }

    /**
     * Add new calibration container.
     * If container with given name was not registered, function throws.
     *
     * \param name container name
     * \param cal calibration object
     * \return success
     */
    auto add_cal_container(std::string_view name, std::unique_ptr<SVirtualCalContainer>&& cal) -> bool;

    /**
     * Get calibration container by name.
     * If container with given name was not registered, function throws.
     *
     * \param name container name
     * \return pointer to container
     */
    auto get_cal_container(std::string_view name) -> SVirtualCalContainer*;

    /// Set release value.
    /// \param rel release name
    auto set_release(std::string_view rel) -> void { release = rel; }

    /// Get release value.
    /// \return release value
    auto get_release() const -> std::string_view { return release; }

    auto print() const -> void;

    friend class SParAsciiSource;

private:
    std::list<parameters_source*> sources;                                        ///< Parameters source file
    parameters_source* target {nullptr};                                          ///< Parameters destination file

    std::map<std::string, parameters_source*> conts_sources;                      ///< Input sources
    std::map<std::string, std::unique_ptr<SParContainer>> par_containers;         ///< Par Containers
    std::map<std::string, std::unique_ptr<SPar>> par_parameters;                  ///< Parameters
    std::map<std::string, std::unique_ptr<SVirtualCalContainer>> cal_containers;  ///< Par Containers
    std::map<std::string, lookup_table*> lut_containers;                          ///< Lookup Containers

    std::string_view release;                                                     ///< stores parameters release name
};

}  // namespace spark
