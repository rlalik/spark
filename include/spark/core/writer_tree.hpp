/*************************************************************************
 * Copyright (C) 2025, Rafał Lalik <rafal.lalik@uj.edu.pl>               *
 * All rights reserved.                                                  *
 *                                                                       *
 * For the licensing terms see LICENSE file.                             *
 * For the list of contributors see README.md file.                      *
 *************************************************************************/

#pragma once

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

namespace spark::writer
{

/**
 * \class sparksys
 * \ingroup core
 *
 * Access point to load, read and write data.
 *
 * The main class of the Spark Framework. The spark is responsible for
 * managing all data operations. It loads a root tree from specified file.
 */

// class SPARK_EXPORT spark_reader : public spark_dep
// {
// public:
//     spark_reader(sparksys* sprk, std::string_view tree_name)
//         : spark_dep(sprk)
// #ifdef USE_TTREE
//         , input_tree {std::make_unique<TChain>(std::string(tree_name).c_str())} {}
// #else
//         , rnt_reader {ROOT::RNTupleReader::Open(std::move(model), input_tree_name, input_file_name)}
//     {
//         no_entries = rnt_reader->GetNEntries();
//         rnt_reader->PrintInfo();
//     }
// #endif
//
//         /**
//          * Returns number of entries in the current tree.
//          * \return number of entries
//          */
//         auto get_entries() -> Long64_t
//     {
//         // if (!input_tree) { FIXME
//         //     spdlog::warn("[Warning] in spark: no input tree is opened. Cannot get any entry.");
//         //     return -1;
//         // }
//
// #ifdef USE_TTREE
// // no_entries = input_tree->GetEntries();
// #else
//         no_entries = rnt_reader->GetNEntries();
// #endif
//         return no_entries;
//     }
//
//     auto add_file(const char* file)
//     {
// #ifdef USE_TTREE
//         input_tree->AddFile(file);
// #else
//         DUPA;
// #endif
//     }
//
// #ifdef USE_TTREE
//     auto chain() -> TChain* { return input_tree.get(); }
// #else
//     DUPA;
// #endif
//
//     /**
//      * Reads entry of the current tree.
//      * The categories are filled with the data saved in tree entry at index \p i.
//      * \param i event number
//      */
//     auto get_entry(Long64_t idx) -> void
//     {
//         // if (!input_tree) { FIXME
//         //     spdlog::warn("[Warning] in spark: no input tree is opened. Cannot get any entry.");
//         //     return;
//         // }
//
//         if (idx < no_entries) {
//             current_entry = idx;
// #ifdef USE_TTREE
//             input_tree->GetEntry(idx);
// #else
//             rnt_reader->LoadEntry(idx);
// #endif
//         }
//     }
//
//     auto model() -> category_manager& { return spark()->model(); }
//
//     auto pardb() -> database& { return spark()->pardb(); }
//
//     auto tasks() -> task_manager& { return spark()->tasks(); }
//
// private:
// #ifdef USE_TTREE
//     // std::string input_tree_name;
//     std::unique_ptr<TChain> input_tree;
// #else
//     std::unique_ptr<ROOT::RNTupleReader> rnt_reader;
// #endif
//
//     int64_t no_entries {-1};     ///< Number of input entries
//     int64_t current_entry {-1};  ///< Current input entry number
// };
class SPARK_EXPORT tree : public spark_dep
{
public:
    /**
     * Creates a new file and an empty root tree with output categories.
     * \param with_tree create output tree
     * \return true if success
     */
    tree(sparksys* sprk, std::string_view tree_name, std::string_view file_name);

    /**
     * Fills the current event into the tree.
     *
     * \return status of Fill() method
     */
    auto fill() -> int
    {
        // spark()-> TODO see below
        // category_mgr.fill(); FIXME make it compress categories
        auto status = 0;

        return status;
    }

    /**
     * Loop ever entries.
     * \param entries number to entries to loop over
     * \param show_progress_bar display progress bar
     */
    auto process_data(uint64_t entries, bool /*show_progress_bar*/ = true) -> void;

    auto model() -> category_manager& { return spark()->model(); }

    auto pardb() -> database& { return spark()->pardb(); }

    auto tasks() -> task_manager& { return spark()->tasks(); }

private:
    std::unique_ptr<TFile> output_file {nullptr};  ///< Pointer to output file
    std::string output_file_name;                  ///< Output file name

    std::unique_ptr<TTree> output_tree {nullptr};  ///< Pointer to output tree
    std::string output_tree_name;

    std::map<uint16_t, category> cat_obj;          ///< Map of categories
};

}  // namespace spark::writer
