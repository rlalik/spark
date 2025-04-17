// @(#)lib/base:$Id$
// Author: Rafal Lalik  18/11/2017

/*************************************************************************
 * Copyright (C) 2017-2018, Rafał Lalik.                                 *
 * All rights reserved.                                                  *
 *                                                                       *
 * For the licensing terms see $SPARKSYS/LICENSE.                         *
 * For the list of contributors see $SiFiSYS/README/CREDITS.             *
 *************************************************************************/

#include "spark/spark.hpp"

#include "spark/core/detector.hpp"

#include <algorithm>
#include <cstdint>
#include <limits>
#include <string>
#include <vector>

#include <indicators/color.hpp>
#include <indicators/cursor_control.hpp>
#include <indicators/font_style.hpp>
#include <indicators/indeterminate_progress_bar.hpp>
#include <spdlog/spdlog.h>

namespace spark
{

auto sparksys::setup_containers() -> void
{
    std::for_each(detectors.begin(), detectors.end(), [](auto& det) { det->init_containers(); });
}

auto sparksys::setup_categories() -> void
{
    std::for_each(detectors.begin(), detectors.end(), [](auto& det) { det->init_categories(); });
}

auto sparksys::setup_tasks() -> void
{
    std::for_each(detectors.begin(), detectors.end(), [&](auto& det) { det->setup_tasks(task_mgr); });
}

auto sparksys::init_system() -> void
{
    spdlog::info(" Setup detectors containers");
    setup_containers();
    spdlog::info(" Setup detectors categories");
    setup_categories();

    spdlog::info(" Setup detectors tasks");
    setup_tasks();

    spdlog::info("  Build tasks queue");
    task_mgr.build_queue();
    task_mgr.print_queue();
    spdlog::info("  Init tasks in queue");
    task_mgr.init_tasks();
}

/**
 * Loop ever entries.
 * \param entries number to entries to loop over
 * \param show_progress_bar display progress bar
 */
auto sparksys::process_data(uint64_t entries, bool /*show_progress_bar*/) -> void
{
    spdlog::info("Init system");
    init_system();

    spdlog::info("Open sources");
    open();

    spdlog::info("Initialize branches");
    init_branches();

    auto pbar = indicators::IndeterminateProgressBar {
        indicators::option::BarWidth {80},
        // indicators::option::Start {"["},
        // indicators::option::Fill {"·"},
        // indicators::option::Lead {"<==>"},
        // indicators::option::End {"]"},
        indicators::option::PostfixText {"Processing events"},
        indicators::option::ForegroundColor {indicators::Color::yellow},
        indicators::option::FontStyles {std::vector<indicators::FontStyle> {indicators::FontStyle::bold}}};

    // Hide cursor
    indicators::show_console_cursor(/*show=*/false);

    // go over all events
    uint64_t event_count {0};
    const uint64_t max_event_count = entries == 0 ? std::numeric_limits<uint64_t>::max() : entries;

    spdlog::info("Processing {} events", entries == 0 ? "all possible" : std::to_string(max_event_count));
    // spdlog::info("Processing {} events", max_event_count);

    for (; event_count < max_event_count; ++event_count) {
        if ((event_count + 1) % 1000 == 0) {
            // Show iteration as postfix text
            // pbar.set_option(
            //     indicators::option::PostfixText {std::to_string(event_count) + "/" +
            //     std::to_string(n_events.value())});

            pbar.set_option(indicators::option::PostfixText {std::to_string(event_count + 1)});
            pbar.print_progress();
        }

        if ((event_count + 1) % 10000 == 0) {
            pbar.tick();
        }

        clear();
        bool flag = false;

        get_entry(event_count);  // TODO do we need this?

        for (auto& source : input_sources) {
            source->set_current_event(event_count);
            flag = source->read_current_event();
            if (!flag) {
                spdlog::info("Source could not read more evenys, finished at {}", event_count);
                // break;
            }
        }

        if (!flag) {
            spdlog::info("Flag not valid, finished at {}", event_count);
            // break;
        }

        task_mgr.execute_tasks();

        fill();
    }

    pbar.mark_as_completed();

    task_mgr.deinit_tasks();

    // Show cursor
    indicators::show_console_cursor(/*show=*/true);

    save();

    spdlog::info("*** spark finished after {} events", event_count);
}

}  // namespace spark
