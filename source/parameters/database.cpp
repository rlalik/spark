// @(#)lib/base:$Id$
// Author: Rafal Lalik  18/11/2017

/*************************************************************************
 * Copyright (C) 2017-2018, Rafał Lalik.                                 *
 * All rights reserved.                                                  *
 *                                                                       *
 * For the licensing terms see $SiFiSYS/LICENSE.                         *
 * For the list of contributors see $SiFiSYS/README/CREDITS.             *
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

    //     for (const auto& cc : cal_containers)
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
    for (const auto& p : par_containers) {
        // p.second->print(); FIXME
    }
}

auto database::add_par_container(std::string_view name, std::unique_ptr<SPar>&& par) -> bool
{
    auto it = par_parameters.find(std::string(name));
    if (it != par_parameters.end()) {
        return false;
    }

    par_parameters.insert(std::make_pair(name, std::move(par)));
    return true;
}

auto database::get_par_container(std::string_view name) -> SPar*
{
    auto it = par_parameters.find(std::string(name));
    if (it == par_parameters.end()) {
        auto msg = fmt::format("Parameter container [{}] not known!", name);
        spdlog::critical(msg);
        throw std::runtime_error(msg);
        // if (!sifi()->assertationsDisabled()) { FIXME
        //     exit(EXIT_FAILURE);
        // }
        return nullptr;
    }

    // TODO current id number
    const long runid = std::numeric_limits<long>::max();

    // Check if Container was initialized from source, and if not, do it.
    auto it2 = par_containers.find(std::string(name));
    if (it2 == par_containers.end()) {
        for (auto* s : sources) {
            // auto c = s->getContainer(name, runid); FIXME
            // if (c) {
            //     conts_sources.emplace(name, s);
            //     SParContainer* pc = new SParContainer(name);
            //     pc->fromContainer(c);
            //     par_containers.emplace(name, pc);
            //     it->second.get()->getParams(pc);
            //     return it->second.get();
            // }
        }

        spdlog::critical("Parameter container [{}] could not be initialized!", name);
        // if (!sifi()->assertationsDisabled()) { FIXME
        //     exit(EXIT_FAILURE);
        // }
        return nullptr;
    }

    return it->second.get();
}

/*
bool database::add_lookup_container(std::string_view name, std::unique_ptr<lookup_table>&& lu)
{
    auto it = lut_containers.find(std::string(name));
    if (it != lut_containers.end()) {
        return false;
    }

    lut_containers.insert(std::make_pair(name, std::move(lu)));
    return true;
}*/

// auto database::get_lookup_container(std::string_view name) -> lookup_table*
// {
//     // If lookup_table was not registered before, exit with failure.
//     auto it = lut_containers.find(std::string(name));
//     if (it == lut_containers.end()) {
//         spdlog::critical("Lookup table [{}] not known!", name);
//         // if (!sifi()->assertationsDisabled()) { FIXME
//         //     exit(EXIT_FAILURE);
//         // }
//         return nullptr;
//     }
//
//     // TODO current id number
//     long runid = 1e10;
//
//     auto lu = it->second.get();
//
//     // Check if Container was initialized from source, and if not, do it.
//     auto it2 = conts_sources.find(std::string(name));
//     if (it2 == conts_sources.end()) {
//         for (auto s : sources) {
//             // auto c = s->get_container<lookup_table>(name, runid); FIXME TODO
//             // if (c) {
//             //     conts_sources.emplace(name, s);
//             //     // lu->from_container(c); FIXME
//             //     return lu;
//             // }
//         }
//
//         spdlog::critical("Lookup table [{}] could not be initialized from sources!", name);
//         // if (!sifi()->assertationsDisabled()) { FIXME
//         //     exit(EXIT_FAILURE);
//         // }
//         return nullptr;
//     }
//
//     return lu;
// }

auto database::add_cal_container(std::string_view name, std::unique_ptr<SVirtualCalContainer>&& cal) -> bool
{
    auto it = cal_containers.find(std::string(name));
    if (it != cal_containers.end()) {
        return false;
    }

    cal_containers.insert(std::make_pair(name, std::move(cal)));

    return true;
}

auto database::get_cal_container(std::string_view name) -> SVirtualCalContainer*
{
    // If lookup_table was not registered before, exit with failure.
    auto it = cal_containers.find(std::string(name));
    if (it == cal_containers.end()) {
        spdlog::critical("Calibration container [{}] not known!", name);
        // if (!sifi()->assertationsDisabled()) { FIXME
        //     exit(EXIT_FAILURE);
        // }
        return nullptr;
    }

    // TODO current id number
    const long runid = 1e10;

    auto* cal = it->second.get();

    // Check if Container was initialized from source, and if not, do it.
    auto it2 = conts_sources.find(std::string(name));
    if (it2 == conts_sources.end()) {
        for (auto* s : sources) {
            // auto c = s->getContainer(name, runid); FIXME
            // if (c) {
            //     conts_sources.emplace(name, s);
            //     cal->fromContainer(c);
            //     return cal;
            // }
        }

        spdlog::critical("Calibration container [{}] could not be initialized from sources!", name);
        // if (!sifi()->assertationsDisabled()) { FIXME
        //     exit(EXIT_FAILURE);
        // }
        return nullptr;
    }

    return cal;
}

}  // namespace spark
