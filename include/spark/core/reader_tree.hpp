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
#include "spark/external/magic_enum.hpp"
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

namespace spark
{
class data_source;
class detector;

class sevent;
class sparksys;

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

namespace reader
{
class SPARK_EXPORT tree : public spark_dep
{
public:
    tree(sparksys* sprk, std::string_view tree_name)
        : spark_dep(sprk)
        , input_tree {std::make_unique<TChain>(std::string(tree_name).c_str())}
    {
    }

    /**
     * Returns number of entries in the current tree.
     * \return number of entries
     */
    auto get_entries() const -> Long64_t
    {
        // if (!input_tree) { FIXME
        //     spdlog::warn("[Warning] in spark: no input tree is opened. Cannot get any entry.");
        //     return -1;
        // }

        // no_entries = input_tree->GetEntries();

        if (no_entries == -1) {
            throw std::runtime_error("No input file");
        }

        return no_entries;
    }

    auto add_file(const char* file)
    {
        input_tree->AddFile(file);
        no_entries = input_tree->GetEntries();
    }

    auto chain() -> TChain* { return input_tree.get(); }

    /**
     * Reads entry of the current tree.
     * The categories are filled with the data saved in tree entry at index \p i.
     * \param i event number
     */
    auto get_entry(Long64_t idx) -> void
    {
        // if (!input_tree) { FIXME
        //     spdlog::warn("[Warning] in spark: no input tree is opened. Cannot get any entry.");
        //     return;
        // }

        if (idx < no_entries) {
            current_entry = idx;
            input_tree->GetEntry(idx);
        }
    }

    template<typename ECategories>
    auto set_input(std::initializer_list<ECategories> categories) -> void
    {
        if (input_tree->GetListOfFiles()->GetEntries() == 0) {
            abort();
        }

        // input_tree->GetEntry(0);

        // auto f = input_tree->GetCurrentFile();
        // f->GetObject("FileHeader", file_header);

        // if (!file_header) {
        //     spdlog::critical("File header does not exists!");
        //     abort();
        // }

        // for (auto& cn : file_header->catName) {

        for (auto cat : categories) {
            auto& cat_info = model().get_category_info(cat);
            spdlog::info("Read category {:s}", cat_info.name);

            TBranch* br = input_tree->GetBranch(Form("%s", cat_info.name.c_str()));  // FIXME add .
            if (!br) {
                //         Error("setInput()","Branch not found : %s !",Form("%s.",catname.Data()));
                //         return kFALSE;
            } else {
                auto& cat_info = model().set_category(cat);

                input_tree->SetBranchAddress(Form("%s", cat_info.name.c_str()), &cat_info.ptr);
                input_tree->SetBranchStatus(Form("%s", cat_info.name.c_str()), 1);
                input_tree->SetBranchStatus(Form("%s", cat_info.name.c_str()), 1);
            }
        }
    }

    auto model() -> category_manager& { return spark()->model(); }

    auto pardb() -> database& { return spark()->pardb(); }

    auto tasks() -> task_manager& { return spark()->tasks(); }

private:
    // std::string input_tree_name;
    std::unique_ptr<TChain> input_tree;

    int64_t no_entries {-1};                                ///< Number of input entries
    int64_t current_entry {-1};                             ///< Current input entry number

    std::map<uint16_t, std::unique_ptr<category>> cat_ptr;  ///< Map of categories
};

}  // namespace reader
}  // namespace spark
