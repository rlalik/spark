/*************************************************************************
 * Copyright (C) 2025, Rafał Lalik <rafal.lalik@uj.edu.pl>               *
 * All rights reserved.                                                  *
 *                                                                       *
 * For the licensing terms see LICENSE file.                             *
 * For the list of contributors see README.md file.                      *
 *************************************************************************/

#pragma once

#include "spark/spark_export.hpp"

#include <spdlog/spdlog.h>

namespace spark
{

class sparksys;
class category_manager;
class database;

class task
{
public:
    task(category_manager* catmgr, database* dbmgr)
        : cat_mgr {catmgr}
        , db_mgr {dbmgr}
    {
    }

    task(const task&) = delete;
    task(task&&) = delete;

    auto operator=(const task&) -> task& = delete;
    auto operator=(task&&) -> task& = delete;

    virtual ~task() = default;

    virtual auto init() -> bool { return true; }

    virtual auto execute() -> bool { return true; }

    virtual auto deinit() -> bool { return true; }

    auto model() -> category_manager* { return cat_mgr; }

    auto db() -> database* { return db_mgr; }

private:
    template<typename T>
    auto setup() -> void
    {
        // name = typeid(T).name();
        // own_hash = std::hash<std::string> {}(typeid(T).name());
    }

    auto debug_info() -> void
    {
        // spdlog::debug("Task {} <{}> deps are {}", coreutils::cpp_demangle(name.c_str()).get(), own_hash, deps);
    }

    auto enumerate_task(size_t current_step, auto& unique_tasks) -> size_t
    {
        queue_builder_id = current_step;

        size_t ret_value {queue_builder_id};

        for (auto& dep_hash : deps) {
            auto prev_task = unique_tasks.at(dep_hash).get();

            if (prev_task->queue_builder_id <= current_step) {
                auto ret = unique_tasks.at(dep_hash).get()->enumerate_task(current_step + 1, unique_tasks);

                ret_value = std::max(ret, ret_value);
            }
        }

        return ret_value;
    }

    category_manager* cat_mgr {nullptr};
    database* db_mgr {nullptr};

    std::string name;
    size_t own_hash {0};
    std::vector<size_t> deps;
    size_t queue_builder_id {0};
    size_t task_running_step_id {0};
    bool has_children {false};

    friend class task_manager;
};

}  // namespace spark
