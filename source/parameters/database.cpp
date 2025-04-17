/*************************************************************************
 * Copyright (C) 2025, Rafał Lalik <rafal.lalik@uj.edu.pl>               *
 * All rights reserved.                                                  *
 *                                                                       *
 * For the licensing terms see LICENSE file.                             *
 * For the list of contributors see README.md file.                      *
 *************************************************************************/

#include "spark/parameters/database.hpp"

// #include "SCalContainer.h"
// #include "SPar.h"
// #include "SParContainer.h"
// #include "SParSource.h"

#include <cstdlib>
#include <limits>
#include <memory>
#include <stdexcept>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

#include <fmt/format.h>
#include <spdlog/spdlog.h>

/**
 * \class SDatabase

\ingroup lib_base

Parameters Manager class. Stores and dumps all parameters used by the framework.

It's a singleton class of which object can be obtained using instance() method.

Parameters manager must be initializatied before
MDetectorManager::initParameterContainers() is called since it checks whether
the requested parameter containers exists.
*/

// for trim functions see
// https://stackoverflow.com/questions/216823/whats-the-best-way-to-trim-stdstring
namespace spark
{

void database::write_destination()
{
    //     std::vector<std::string> names; FIXME
    //     names.reserve(containers.size());
    //
    //     for (auto& p : parconts)
    //     {
    //         SParContainer* pc = par_containers[p.first];
    //         p.second->putParams(pc);
    //     }
    //
    //     std::transform(containers.begin(), containers.end(), std::back_inserter(names),
    //                    [](std::pair<std::string, SContainer*> const& e) { return e.first; });
    //
    //     writeContainers(names);
}

void database::write_containers(const std::vector<std::string>& /*names*/)
{
    //     for (const auto& pc : par_containers)    FIXME x3
    //         pc.second->toContainer();

    //     for (const auto& cc : tab_containers)
    //         cc.second->toContainer();

    //     for (const auto& lc : lut_containers)
    //         lc.second->toContainer();

    if (target == nullptr) {
        spdlog::critical("No target specified!");
        abort();
    }

    //     std::ofstream ofs(destination); FIXME
    //     if (ofs.is_open())
    //     {
    //         for (auto& c : names)
    //         {
    //             ofs << "[" << c << "]" << std::endl;
    //             for (const auto& l : containers.at(c)->lines)
    //                 ofs << l << std::endl;
    //         }
    //     }
    //     ofs.close();
}

/**
 * Print containers
 */
void database::print() const
{
    for (const auto& p : all_containers) {
        // p.second->print(); FIXME
    }
}

}  // namespace spark
