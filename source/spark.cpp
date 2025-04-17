/*************************************************************************
 * Copyright (C) 2025, Rafał Lalik <rafal.lalik@uj.edu.pl>               *
 * All rights reserved.                                                  *
 *                                                                       *
 * For the licensing terms see LICENSE file.                             *
 * For the list of contributors see README.md file.                      *
 *************************************************************************/

#include "spark/spark.hpp"

#include "spark/core/detector.hpp"

#include <algorithm>
#include <cstdint>
#include <limits>
#include <string>
#include <vector>

#include <spdlog/spdlog.h>

namespace spark
{

auto sparksys::init_reader_system() -> void
{
    spdlog::info("..:: INIT SPARK READER SYSTEM ::..");

    spdlog::info(" Setup detectors categories");
    cat_mgr.setup_from_detector(det_mgr);
    model().print_registered();
}

auto sparksys::init_writer_system(uint64_t run_id) -> void
{
    spdlog::info("..:: INIT SPARK WRITER SYSTEM ::..");

    spdlog::info(" Setup detectors categories");
    cat_mgr.setup_from_detector(det_mgr);
    model().print_registered();

    spdlog::info(" Setup tasks");
    task_mgr.setup_from_detector(det_mgr);

    spdlog::info(" Setup detectors containers");
    pardb().setup_from_detector(det_mgr);
    pardb().init_containers(run_id);

    spdlog::info(" Setup detectors tasks");
    spdlog::info("  Init unpackers");
    tasks().init_unpackers();

    spdlog::info("  Build tasks queue");
    tasks().build_queue();
    tasks().print_queue();

    spdlog::info("  Init tasks in queue");
    tasks().init_tasks();

    // spdlog::info("Open sources");
    // open();
}

}  // namespace spark
