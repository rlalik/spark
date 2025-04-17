/*************************************************************************
 * Copyright (C) 2025, Rafał Lalik <rafal.lalik@uj.edu.pl>               *
 * All rights reserved.                                                  *
 *                                                                       *
 * For the licensing terms see LICENSE file.                             *
 * For the list of contributors see README.md file.                      *
 *************************************************************************/

#include "spark/core/writer_tree.hpp"

#include "spark/spark_config.hpp"
#include "spark/spark_export.hpp"

#include "spark/core/category.hpp"
#include "spark/core/category_manager.hpp"
#include "spark/core/data_source.hpp"
#include "spark/core/root_file_header.hpp"
#include "spark/core/spark_dep.hpp"
#include "spark/core/task_manager.hpp"
#include "spark/core/types.hpp"
#include "spark/parameters/database.hpp"
#include "spark/spark.hpp"
#include "spark/utils/conversions.hpp"

#include <TChain.h>
#include <TClass.h>
#include <TFile.h>
#include <TTree.h>

#include <algorithm>
#include <cstddef>
#include <map>
#include <string>
#include <utility>
#include <vector>

#include <spdlog/spdlog.h>

#include <indicators/color.hpp>
#include <indicators/cursor_control.hpp>
#include <indicators/font_style.hpp>
#include <indicators/indeterminate_progress_bar.hpp>

namespace spark::writer
{

tree::tree(sparksys* sprk, std::string_view tree_name, std::string_view file_name)
    : spark_dep(sprk)
    , output_file_name {std::move(file_name)}
    , output_tree_name {std::move(tree_name)}
{
    spdlog::info("..:: Init Tree writer with tree={} file={}", tree_name, file_name);

    // Create file and open it FIXME remove all below?
    output_file = std::make_unique<TFile>(output_file_name.c_str(), "RECREATE");

    if (!output_file->IsOpen()) {
        spdlog::critical("[Error] in spark: could not open file {}", output_file_name);
    }

    // Create tree
    output_tree = std::make_unique<TTree>(output_tree_name.c_str(), output_tree_name.c_str());

    spark()->model().build_from_model(
        [&](category_info& cinfo)
        {
            if (!cinfo.persistent) {
                spdlog::info("    -> Skipping category {:s} as not persistent.", cinfo.name);
                return;
            }

            if (!cinfo.ptr) {
                spdlog::info("    -> Skipping category {:s} as not built.", cinfo.name);
                return;
            }

            spdlog::info("    -> Add branch {:s} pointing at {:p}.", cinfo.name, static_cast<void*>(&cinfo.ptr));

            output_tree->Branch(cinfo.ptr->get_name(), &cinfo.ptr, 16000, 99);

            return;
        });
}

auto tree::process_data(uint64_t entries, bool /*show_progress_bar*/) -> void
{
    spdlog::info("Initialize model");
    // init_branches(); FIXME

    spark()->open();

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

        model().clear();

        bool flag = false;

        // get_entry(event_count);  // TODO do we need this? FIXME

        for (auto& source : spark()->sources()) {
            source->set_current_event(event_count);
            flag = source->read_current_event();
            if (!flag) {
                spdlog::info("Source could not read more events, finished at {}", event_count);
                // break;
            }
        }

        if (!flag) {
            spdlog::info("Flag not valid, finished at {}", event_count);
            // break;
        }

        tasks().execute_tasks();

        model().compress();
        output_tree->Fill();
    }

    pbar.mark_as_completed();

    tasks().deinit_tasks();

    // Show cursor
    indicators::show_console_cursor(/*show=*/true);

    output_file->cd();
    output_tree->Write();

    spdlog::info("*** spark finished after {} events", event_count);
}

}  // namespace spark::writer
