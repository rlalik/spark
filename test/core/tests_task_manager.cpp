/*************************************************************************
 * Copyright (C) 2025, Rafał Lalik <rafal.lalik@uj.edu.pl>               *
 * All rights reserved.                                                  *
 *                                                                       *
 * For the licensing terms see LICENSE file.                             *
 * For the list of contributors see README.md file.                      *
 *************************************************************************/

#include <gtest/gtest.h>

#include <spark/core/task_manager.hpp>

#include <limits>
#include <stdexcept>

class task1 : public spark::task
{
    using task::task;
};

class task2 : public spark::task
{
    using task::task;
};

class task3 : public spark::task
{
    using task::task;
};

class task4 : public spark::task
{
    using task::task;
};

class task5 : public spark::task
{
    using task::task;
};

class task6 : public spark::task
{
    using task::task;
};

class bad_task
{
};

TEST(TestTaskManager, AddingTasks)
{
    spdlog::set_level(spdlog::level::debug);

    auto tmgr = spark::task_manager(nullptr, nullptr);

    ASSERT_NO_THROW((tmgr.add_task<task1>()));
    ASSERT_THROW((tmgr.add_task<task1, task2>()), std::out_of_range);
    ASSERT_THROW((tmgr.add_task<task1, task2, task3>()), std::out_of_range);

    ASSERT_NO_THROW((tmgr.add_task<task2>()));
    ASSERT_NO_THROW((tmgr.add_task<task1, task2>()));

    spdlog::set_level(spdlog::level::info);
}

TEST(TestTaskManager, MakingRunQueue)
{
    spdlog::set_level(spdlog::level::debug);

    auto tmgr = spark::task_manager(nullptr, nullptr);

    tmgr.add_task<task1>();
    tmgr.add_task<task2, task1>();
    tmgr.add_task<task3, task2>();
    tmgr.add_task<task4, task3>();
    tmgr.add_task<task5, task3>();
    tmgr.add_task<task6, task3, task2, task4, task5>();

    tmgr.build_queue();

    spdlog::set_level(spdlog::level::info);
}
