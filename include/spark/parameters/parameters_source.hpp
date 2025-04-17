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

#include "spark/parameters/container.hpp"

#include <map>
#include <string>
#include <string_view>

namespace spark
{

struct runs_view
{
    std::vector<std::shared_ptr<container>> containers;
};

class SPARK_EXPORT parameters_source
{
public:
    parameters_source() = default;

    parameters_source(const parameters_source&) = delete;
    parameters_source(parameters_source&&) = delete;

    auto operator=(const parameters_source&) -> parameters_source& = delete;
    auto operator=(parameters_source&&) -> parameters_source& = delete;

    virtual ~parameters_source() = default;

    /// Print containers stored in the source. Details about source must be print by subclass
    /// method.
    virtual auto print() const -> void = 0;

private:
    // virtual auto get_container_view(std::string_view name, long runid) -> file_view = 0;

    std::map<std::string, runs_view> cont_runs;
};

}  // namespace spark
