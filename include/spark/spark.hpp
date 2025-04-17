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
#include "spark/utils/conversions.hpp"

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

class sevent;

/**
 * \class sparksys
 * \ingroup core
 *
 * Access point to load, read and write data.
 *
 * The main class of the Spark Framework. The spark is responsible for
 * managing all data operations. It loads a root tree from specified file.
 */

// template<typename ECategories>
class SPARK_EXPORT sparksys /*: public spark_abstract_base*/
{
private:
public:
    // spark(std::type_identity<ECategories>, std::string file_name, std::string tree_name)
    template<typename ECategories>
    sparksys(std::in_place_type_t<ECategories> _unused_type)
        : categories_hash {utils::get_type_hash<ECategories>()}
        , task_mgr {&cat_mgr, &par_db}
    {
        file_header.serialize<ECategories>();
    }

public:
    template<typename ECategories>
    static auto create() -> sparksys
    {
        return sparksys(std::in_place_type_t<ECategories> {});
    }

    template<typename WriterTupleType>
    auto create_writer(std::string tree_name, std::string file_name, uint64_t run_id) -> WriterTupleType
    {
        init_writer_system(run_id);
        return WriterTupleType(this, tree_name, file_name);
    }

    template<typename ReaderTupleType>
    auto create_reader(std::string tree_name) -> ReaderTupleType
    {
        init_reader_system();
        return ReaderTupleType(this, tree_name);
    }

    sparksys(sparksys const&) = delete;
    sparksys(sparksys&&) = delete;

    auto operator=(sparksys const&) -> sparksys& = delete;
    auto operator=(sparksys&&) -> sparksys& = delete;

    ~sparksys() = default;

    // methods

    /*******************************************************/
    /********************** Detectors **********************/

    // /** FIXME
    //  * Clear all categories.
    //  */
    // auto clear() -> void { category_mgr.clear(); }

    /**
     * Opens a file and loads the root tree.
     *
     * \return success
     */
    auto open() -> bool
    {
        // rnt_reader = ROOT::RNTupleReader::Open(std::move(model), input_tree_name, output_file_name);

        // input_tree = std::make_unique<TChain>(input_tree_name.c_str()); FIXME
        //
        // if (input_tree == nullptr) {
        //     spdlog::critical("[Error] in spark: cannot open ROOT file {}", input_tree_name);
        //     return false;
        // }

        for (auto& source : input_sources) {
            // spdlog::info("Add file %{}", inputFiles[i]);
            // input_tree->Add(inputFiles[i].c_str());

            if (!source->open()) {
                spdlog::critical("Could not open source {}", 0);  // TODO add some feedback info
                abort();
            }

            file_header.Write("FileHeader");
        }

        // input_tree->SetBranchStatus("*");

        // no_entries = input_tree->GetEntriesFast();

        return true;
    }

    /**
     * Init all systems and process the data in the loop.
     * \param entries number to entries to loop over
     * \param show_progress_bar display progress bar
     */
    // auto process_data(uint64_t entries, bool /*show_progress_bar*/ = true) -> void;

    /// Set output file name
    /// \param file file name
    // auto set_output_filename(const std::string& file) -> void { output_file_name = file; }

    /// Add source to the list of sources
    /// \param data source object
    auto add_source(data_source* data) -> void { input_sources.push_back(data); }

    /// Get entry number
    /// \return entry number
    auto get_entry_number() const -> Long64_t { return current_entry; }

    /// Get input tree
    /// \return input tree
    // auto get_tree() const -> TTree* { return input_tree.get(); } FIXME

    /// Set input tree
    /// \param t TTree tree object
    // void set_tree(TTree* t) { input_tree = t; } FIXME

    /// Returns current event.
    /// \return SEvent structure
    auto get_current_event() const -> sevent* { return event; }

    /**
     * Set event TODO what was that?
     * \param e event
     */
    auto set_event(sevent* evt) -> void
    {
        //     if (event) delete event;
        event = evt;
    }

    /*******************************************************/
    /*********************** Control ***********************/

    auto init_reader_system() -> void;
    auto init_writer_system(uint64_t run_id) -> void;

    /*******************************************************/
    /*********************** Various ***********************/

    /// In case of fatal event, like missing parameter or container, framework immediately breaks
    /// execution. This safety switch can be disabled if required (automatic tests need it), however
    /// this action is discouraged. This function disables it.
    auto assertations_disable() -> void { disable_assertations = true; }

    /// Enabled runtime assertion. See disableAssertations() for details.
    auto assertations_enable() -> void { disable_assertations = false; }

    /// Return status of runtime assertion. See disableAssertations() for details.
    /// \return status of runtime assertions
    auto assertations_disabled() const -> bool { return disable_assertations; }

    auto system() -> detector_manager& { return det_mgr; }

    auto model() -> category_manager& { return cat_mgr; }

    auto pardb() -> database& { return par_db; }

    auto tasks() -> task_manager& { return task_mgr; }

    auto sources() -> std::vector<data_source*>& { return input_sources; }

private:
    /*******************************************************/
    /******************* Files and trees *******************/
    root_file_header file_header;  ///< Root File Header

    /*******************************************************/
    /*********************** Sources ***********************/
    std::vector<data_source*> input_sources;  ///< Input file name

    /*******************************************************/
    /********************* Categories **********************/
    std::size_t categories_hash;  ///< Category hash

    /*******************************************************/
    /*********************** Managers **********************/
    spark::detector_manager det_mgr {};
    spark::category_manager cat_mgr {};
    spark::task_manager task_mgr;
    spark::database par_db {};

    /*******************************************************/
    /*********************** Events ************************/
    int64_t no_entries {-1};     ///< Number of input entries
    int64_t current_entry {-1};  ///< Current input entry number

    sevent* event {nullptr};     ///< Event info structure

    /*******************************************************/
    /******************** State control ********************/
    bool branches_set {false};          ///< Has branches set
    bool disable_assertations {false};  ///< disable runtime assertions

    friend class spark_writer;
    friend class spark_reader;
};

}  // namespace spark
