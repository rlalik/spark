/*************************************************************************
 * Copyright (C) 2025, Rafał Lalik <rafal.lalik@uj.edu.pl>               *
 * All rights reserved.                                                  *
 *                                                                       *
 * For the licensing terms see LICENSE file.                             *
 * For the list of contributors see README.md file.                      *
 *************************************************************************/

#pragma once

#include "spark/spark_export.hpp"

#include "spark/core/detector.hpp"
#include "spark/core/detector_manager.hpp"
#include "spark/core/task.hpp"
#include "spark/core/unpacker.hpp"
#include "spark/utils/demangler.hpp"

#include <map>

#include <fmt/ranges.h>
#include <spdlog/spdlog.h>

namespace spark
{

class sparksys;
class category_manager;
class database;

class SPARK_EXPORT task_manager
{
public:
    task_manager(category_manager* mgr, database* db)
        : cat_mgr {mgr}
        , db_mgr {db}
    {
    }

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
                          dep_names);

            auto new_task = std::make_unique<Task>(cat_mgr, db_mgr, args...);
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
                             dep_names);
            throw e;
        }
    }

    template<typename... Args>
    auto setup_from_detector(Args... detectors) -> void
    {
        spdlog::info(" Setup tasks from detector");
        (detectors->setup_tasks(*this), ...);
    }

    auto setup_from_detector(detector_manager& det_mgr) -> void
    {
        spdlog::info(" Setup tasks using detector manager");
        std::for_each(det_mgr.begin(), det_mgr.end(), [&](auto& det) { det->setup_tasks(*this); });
    }

    template<typename U, typename... Args>
    auto make_unpacker(Args... args) -> std::shared_ptr<spark::unpacker>
    {
        auto unp = std::make_shared<U>(model(), db(), args...);
        unpackers.push_back(unp);
        return unp;
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

    auto init_unpackers() -> void;

    auto model() -> category_manager* { return cat_mgr; }

    auto db() -> database* { return db_mgr; }

private:
    category_manager* cat_mgr {nullptr};
    database* db_mgr {nullptr};

    std::map<size_t, std::unique_ptr<task>> unique_tasks;
    std::map<size_t, std::vector<task*>> tasks_queue;
    std::vector<std::shared_ptr<spark::unpacker>> unpackers;

    friend struct fmt::formatter<spark::task_manager>;
};

}  // namespace spark

template<>
struct fmt::formatter<spark::task_manager> : fmt::formatter<std::string>
{
    static auto format(const spark::task_manager& task_mgr, format_context& ctx) -> format_context::iterator
    {
        fmt::format_to(ctx.out(), "Registered unpackers:\n");
        std::for_each(task_mgr.unpackers.begin(),
                      task_mgr.unpackers.end(),
                      [&](auto& unpacker) { fmt::format_to(ctx.out(), "-> {}\n", *unpacker); });
        return ctx.out();
    }
};
