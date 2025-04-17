/*************************************************************************
 * Copyright (C) 2025, Rafał Lalik <rafal.lalik@uj.edu.pl>               *
 * All rights reserved.                                                  *
 *                                                                       *
 * For the licensing terms see LICENSE file.                             *
 * For the list of contributors see README.md file.                      *
 *************************************************************************/

#include "spark/core/task_manager.hpp"

#include "spark/utils/demangler.hpp"

#include <algorithm>
#include <cstddef>
#include <ranges>

#include <spdlog/spdlog.h>

/**
 * \class category
\ingroup lib_core

A Category abstract interface

*/

namespace spark
{

auto task_manager::build_queue() -> void
{
    auto has_no_children = [](const decltype(unique_tasks)::value_type& task) { return !task.second->has_children; };
    auto add_end_task = [](const decltype(unique_tasks)::value_type& task) { return task.second->own_hash; };

    auto end_tasks = unique_tasks | std::views::filter(has_no_children) | std::views::transform(add_end_task);

    size_t max_queue_value {0};
    std::ranges::for_each(end_tasks,
                          [&](size_t task_hash)
                          {
                              auto ret = unique_tasks.at(task_hash)->enumerate_task(0, unique_tasks);

                              max_queue_value = std::max(ret, max_queue_value);
                          });

    spdlog::debug("Max task queue value = {}", max_queue_value);

    std::ranges::for_each(unique_tasks,
                          [&](auto& task_pair)
                          {
                              task_pair.second->task_running_step_id =
                                  max_queue_value - task_pair.second->queue_builder_id;
                              tasks_queue[task_pair.second->task_running_step_id].push_back(task_pair.second.get());

                              spdlog::debug("Task {} <{}> queue id = {}  step = {}",
                                            task_pair.second->name,
                                            task_pair.second->own_hash,
                                            task_pair.second->queue_builder_id,
                                            task_pair.second->task_running_step_id);
                          });

    std::ranges::for_each(
        tasks_queue,
        [&](auto& queue_pair)
        {
            std::ranges::for_each(
                queue_pair.second,
                [&](auto& task)
                { spdlog::debug("Queue step {} -> task {} <{}>", queue_pair.first, task->name, task->own_hash); });
        });
}

auto task_manager::print_queue() -> void
{
    std::ranges::for_each(tasks_queue,
                          [&](auto& queue_pair)
                          {
                              spdlog::info("   Task queue step {}", queue_pair.first);
                              std::ranges::for_each(
                                  queue_pair.second,
                                  [&](auto& task)
                                  { spdlog::info("    {}", utils::cpp_demangle(task->name.c_str()).get()); });
                          });
}

auto task_manager::init_tasks() -> void
{
    std::ranges::for_each(tasks_queue,
                          [&](auto& queue_pair)
                          {
                              std::ranges::for_each(queue_pair.second,
                                                    [&](auto& task)
                                                    {
                                                        spdlog::info(" :: Init {}",
                                                                     utils::cpp_demangle(task->name.c_str()).get());
                                                        task->init();
                                                    });
                          });
}

auto task_manager::execute_tasks() -> void
{
    std::ranges::for_each(
        tasks_queue,
        [&](auto& queue_pair)
        { std::for_each(queue_pair.second.begin(), queue_pair.second.end(), [&](auto& task) { task->execute(); }); });
}

auto task_manager::deinit_tasks() -> void
{
    std::ranges::for_each(
        tasks_queue,
        [&](auto& queue_pair)
        { std::for_each(queue_pair.second.begin(), queue_pair.second.end(), [&](auto& task) { task->deinit(); }); });
}

auto task_manager::init_unpackers() -> void
{
    std::ranges::for_each(unpackers, [&](auto& unpacker) { unpacker->init(); });
}

}  // namespace spark
