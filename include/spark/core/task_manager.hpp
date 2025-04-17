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

#include "spark/core/spark_dep.hpp"
#include "spark/core/task.hpp"
#include "spark/utils/demangler.hpp"

#include <map>

#include <fmt/ranges.h>
#include <spdlog/spdlog.h>

namespace spark
{

class sparksys;

class SPARK_EXPORT task_manager : public spark_dep
{
private:
    std::map<size_t, std::unique_ptr<task>> unique_tasks;
    std::map<size_t, std::vector<task*>> tasks_queue;

public:
    using spark_dep::spark_dep;

    template<class Task, class... Deps, class... Args>
        requires(std::is_base_of_v<task, Deps> && ...)
    auto add_task(Args... args) -> void
    {
        std::array<std::string, sizeof...(Deps)> dep_names {utils::cpp_demangle(typeid(Deps).name()).get()...};

        try {
            std::array<task*, sizeof...(Deps)> deps {
                unique_tasks.at(std::hash<std::string> {}(typeid(Deps).name())).get()...};

            spdlog::debug("Task '{}' has dependencies: {}",
                          utils::cpp_demangle(typeid(Task).name()).get(),
                          fmt::join(dep_names, ","));

            auto new_task = std::make_unique<Task>(spark(), args...);
            // new_task->setup<Task>(); FIXME
            new_task->name = typeid(Task).name();
            new_task->own_hash = std::hash<std::string> {}(typeid(Task).name());

            for (auto dep_task : deps) {
                dep_task->has_children = true;
                new_task->deps.push_back(dep_task->own_hash);
            }

            new_task->debug_info();

            unique_tasks.emplace(new_task->own_hash, std::move(new_task));
        } catch (std::out_of_range& e) {
            spdlog::critical("Some of the '{}' task dependencies: {} are not found",
                             utils::cpp_demangle(typeid(Task).name()).get(),
                             fmt::join(dep_names, ","));
            throw e;
        }
    }

    /**
     * Make tasks execution order queue.
     *
     * First, find the end-tasks - those without children - and then goes up the tree to enumerate each preceding task.
     */
    auto build_queue() -> void;

    auto print_queue() -> void;

    /**
     * Call init() of each registered task.
     */
    auto init_tasks() -> void;

    /**
     * Call execute() of each registered task.
     */
    auto execute_tasks() -> void;

    /**
     * Call deinit() of each registered task.
     */
    auto deinit_tasks() -> void;
};

}  // namespace spark
