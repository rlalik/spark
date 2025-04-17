/*************************************************************************
 * Copyright (C) 2025, Rafał Lalik <rafal.lalik@uj.edu.pl>               *
 * All rights reserved.                                                  *
 *                                                                       *
 * For the licensing terms see LICENSE file.                             *
 * For the list of contributors see README.md file.                      *
 *************************************************************************/

#include "spark/core/reader_tree.hpp"

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

namespace spark
{

/**
 * Reads entry of the current tree.
 * The categories are filled with the data saved in tree entry at index \p i.
 * \param i event number
 */
// auto reader_tuple::get_entry(Long64_t idx) -> void
// {
//     // if (!input_tree) { FIXME
//     //     spdlog::warn("[Warning] in spark: no input tree is opened. Cannot get any entry.");
//     //     return;
//     // }
//
//     if (idx < no_entries) {
//         current_entry = idx;
//         input_tree->GetEntry(idx);
//     }
// }

/**
 * Set categories to be read from the input files.
 *
 * \todo categories selection doesn't work yet
 * \param categories list of categories to be read.
 */
// void reader_tuple::set_input(std::initializer_list<SCategory::Cat> categories)
// {
//     if (chain->GetListOfFiles()->GetEntries() == 0) {
//         abort();
//     }
//
//     current_event = 0;
//     chain->GetEntry(current_event);
//
//     current_tree = chain->GetTree();
//
//     TFile* f = chain->GetCurrentFile();
//     f->GetObject("FileHeader", file_header);
//
//     if (!file_header) {
//         std::cerr << "File header does not exists!" << std::endl;
//         abort();
//     }
//
//     for (auto& cn : file_header->catName) {
//         printf("Read category %s\n", cn.second.Data());
//
//         TBranch* br = current_tree->GetBranch(Form("%s", cn.second.Data()));  // FIXME add .
//         if (!br) {
//             //         Error("setInput()","Branch not found : %s !",Form("%s.",catname.Data()));
//             //         return kFALSE;
//         } else {
//             int pos = SiFi::getCategoryIndex(cn.first);
//             chain->SetBranchAddress(Form("%s", cn.second.Data()), &(this->categories[pos]));
//             chain->SetBranchStatus(Form("%s", cn.second.Data()), 1);
//             chain->SetBranchStatus(Form("%s", cn.second.Data()), 1);
//
//             sifi()->getCurrentEvent()->addCategory(cn.first, this->categories[pos]);
//         }
//     }
// }

}  // namespace spark
