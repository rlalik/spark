/*************************************************************************
 * Copyright (C) 2025, Rafał Lalik <rafal.lalik@uj.edu.pl>               *
 * All rights reserved.                                                  *
 *                                                                       *
 * For the licensing terms see LICENSE file.                             *
 * For the list of contributors see README.md file.                      *
 *************************************************************************/

#pragma once

#include "spark/spark_export.hpp"

#include "spark/parameters/container.hpp"

#include <map>
#include <set>
#include <string>
#include <string_view>

namespace spark
{

struct runs_view
{
    std::vector<std::shared_ptr<container>> containers;
};

/**
 * \class SParSource
 *
 \ i*ngroup lib_base

 Parameters Manager class. Stores and dumps all parameters used by the framework.

 It's a singleton class of which object can be obtained using instance() method.

 Parameters manager must be initializatied before
 MDetectorManager::initParameterContainers() is called since it checks whether
 the requested parameter containers exists.
 */
class SPARK_EXPORT parameters_source
{
public:
    parameters_source() = default;

    parameters_source(const parameters_source&) = delete;
    parameters_source(parameters_source&&) = delete;

    auto operator=(const parameters_source&) -> parameters_source& = delete;
    auto operator=(parameters_source&&) -> parameters_source& = delete;

    virtual ~parameters_source() = default;

    virtual auto build_in_place(std::string_view /*name*/, container* /*cont*/) -> bool { return true; }

    /*
     * Build container name of type T with constructor arguments args.
     */
    template<typename T, typename... Args>
    auto make_container_available(std::string_view name, Args... args) -> bool
    {
        if (!provided_containers.contains(std::string(name))) {
            return false;
        }

        try {
            if (auto ret = dynamic_cast<T*>(containers_objs.at(std::string(name)).get()); ret) {
                return true;
            } else {
                throw spark::container_invalid_type(name);
            }
        } catch (const std::out_of_range&) {
            auto cont_ptr = new T(T::build(name, args...));
            auto cont = std::unique_ptr<T>(std::move(cont_ptr));

            build_in_place(name, cont.get());

            containers_objs.insert({std::string(name), std::move(cont)});
        }

        return true;
    }

    /**
     * Get plain container by name. Interpretation of the content must be done by appropriate class,
     * see SDatabase::getParContainer, SDatabase::getLookupContainer, SDatabase::getCalContainer for
     * examples.
     *
     * \param name container name
     * \param runid run id
     * \return pointer to the container
     */
    template<typename T>
    auto get_container(std::string_view name, long /*_unused_runid*/) -> T*
    {
        return dynamic_cast<T*>(containers_objs.at(std::string(name)).get());
    }

    /// Print containers stored in the source. Details about source must be print by subclass
    /// method.
    virtual auto print() const -> void = 0;

protected:
    auto add_provided_container_name(std::string_view name) { provided_containers.emplace(name); }

private:
    // virtual auto get_container_view(std::string_view name, long runid) -> file_view = 0;

    std::set<std::string> provided_containers;

    std::map<std::string, runs_view> cont_runs;

    std::map<std::string, std::unique_ptr<container>> containers_objs;  ///< Containers mirrors
};

}  // namespace spark
