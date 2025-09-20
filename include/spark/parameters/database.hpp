/*************************************************************************
 * Copyright (C) 2025, Rafał Lalik <rafal.lalik@uj.edu.pl>               *
 * All rights reserved.                                                  *
 *                                                                       *
 * For the licensing terms see LICENSE file.                             *
 * For the list of contributors see README.md file.                      *
 *************************************************************************/

#pragma once

#include "spark/spark_export.hpp"

#include "spark/core/data_source.hpp"
#include "spark/core/detector.hpp"
#include "spark/core/detector_manager.hpp"
#include "spark/core/exceptions.hpp"
#include "spark/parameters/parameters_source.hpp"

#include <algorithm>
#include <concepts>
#include <list>
#include <map>
#include <memory>
#include <print>
#include <string>
#include <string_view>

#include <spdlog/spdlog.h>

namespace spark
{

struct lookup_container;
struct tabular_container;

class SPARK_EXPORT database
{
public:
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

    /*
     * Register container with given name.
     *
     * First, it checks whether given name is already registered. If not, creates object of T with given args.
     * If yes, it checks whether the already registered object is of the same type as requested. It throws if check
     * fails.
     *
     * \param name container name
     * \param args extra arguments, see the T declaration for details.
     */
    template<typename T, typename... Args>
        requires requires { std::is_base_of<container, T>(); }
    auto register_container(std::string_view name, Args... args) -> bool
    {
        auto pos = all_containers.find(std::string(name));
        if (pos == all_containers.end()) {
            all_containers.emplace(
                name,
                violatile_container {
                    typeid(T),
                    name,
                    [](std::string_view name, spark::parameters_source* src, size_t run_id) -> spark::container*
                    { return src->get_container<T>(name, run_id); }});

            bool has_any_source = false;
            for (auto& source : sources) {
                has_any_source |= source->make_container_available<T>(name, args...);
            }
            if (!has_any_source) {
                throw spark::container_missing_in_source(name);
            }
        } else {
            return false;
        }

        return true;
    }

    /**
     * Get calibration container by name.
     * If container with given name was not registered, function throws.
     *
     * \param name container name
     * \return pointer to container
     */
    template<typename T, typename... Args>
        requires requires { std::is_base_of<tabular_container, T>(); }
    auto get_container(std::string_view name) -> container_wrapper<T>
    {
        try {
            return all_containers.at(std::string(name)).wrapper<T>();
        } catch (const std::out_of_range& e) {
            spdlog::critical("Container {:s} does not exists in the database. Available containers are:", name);
            std::ranges::for_each(all_containers, [&](auto& container) { std::print(" -> {}\n", container.first); });
            std::abort();
        }
    }

    template<typename... Args>
    auto setup_from_detector(Args... detectors) -> void
    {
        spdlog::info(" Setup containers from detector");
        (detectors->init_containers(*this), ...);
    }

    auto setup_from_detector(detector_manager& det_mgr) -> void
    {
        spdlog::info(" Setup containers using detector manager");
        std::for_each(det_mgr.begin(), det_mgr.end(), [&](auto& det) { det->setup_containers(*this); });
    }

    auto init_containers(size_t run_id = 0) -> void
    {
        for (auto& cont : all_containers) {
            for (auto& src : sources) {
                if (src) {
                    cont.second.update(src, run_id);
                }
            }
        }
    }

    /// Set release value.
    /// \param rel release name
    auto set_release(std::string_view rel) -> void { release = rel; }

    /// Get release value.
    /// \return release value
    auto get_release() const -> std::string_view { return release; }

    auto print() const -> void;

    friend class SParAsciiSource;

private:
    std::list<parameters_source*> sources;                      ///< Parameters source file
    parameters_source* target {nullptr};                        ///< Parameters destination file
    std::map<std::string, violatile_container> all_containers;  ///< all registered containers

    std::string_view release;                                   ///< stores parameters release name
};

}  // namespace spark
