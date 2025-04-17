// @(#)lib/base:$Id$
// Author: Rafal Lalik  18/11/2017

/*************************************************************************
 * Copyright (C) 2017-2018, Rafał Lalik.                                 *
 * All rights reserved.                                                  *
 *                                                                       *
 * For the licensing terms see $SPARKSYS/LICENSE.                         *
 * For the list of contributors see $SPARKSYS/README/CREDITS.             *
 *************************************************************************/

#pragma once

#include "spark/spark_export.hpp"

#include "spark/core/category.hpp"
#include "spark/core/category_manager.hpp"
#include "spark/core/data_source.hpp"
#include "spark/core/root_file_header.hpp"
#include "spark/core/task_manager.hpp"
#include "spark/core/types.hpp"
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

class SEvent;

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
    // spark(std::type_identity<ECategories>, std::string file_name, std::string tree_name)
    template<typename ECategories>
    sparksys(std::in_place_type_t<ECategories> _unused_type, std::string file_name, std::string tree_name)
        : output_file_name {std::move(file_name)}
        , output_tree_name {tree_name}
        , input_tree_name {std::move(tree_name)}
        , categories_hash {utils::get_type_hash<ECategories>()}
        , task_mgr {this}
    {
        file_header.serialize<ECategories>();
        // cinfovec.reserve(magic_enum::enum_count<ECategories>());
    }

public:
    template<typename ECategories>
    static constexpr auto create(std::string file_name, std::string tree_name) -> sparksys
    {
        return sparksys(std::in_place_type_t<ECategories> {}, file_name, std::move(tree_name));
    }

    sparksys(sparksys const&) = delete;
    sparksys(sparksys&&) = delete;

    auto operator=(sparksys const&) -> sparksys& = delete;
    auto operator=(sparksys&&) -> sparksys& = delete;

    ~sparksys() = default;

    // methods

    /*******************************************************/
    /********************** Detectors **********************/

    template<typename T, typename... Args>
    auto make_detector(Args... args) -> T*
    {
        auto det = new T(this, args...);
        detectors.push_back(std::shared_ptr<detector>(det));

        return det;
    }

    /**
     * Print info about the categories.
     */
    auto print() const -> void
    {
        output_file->cd();

        category_mgr.print();
    }

    /**
     * Clear all categories.
     */
    auto clear() -> void { category_mgr.clear(); }

    /**
     * Creates a new file and an empty root tree with output categories.
     * \param with_tree create output tree
     * \return true if success
     */
    auto book(bool with_tree = true) -> bool
    {
        // Create file and open it
        output_file = std::make_unique<TFile>(output_file_name.c_str(), "RECREATE");

        if (!with_tree) {
            return true;
        }

        if (!output_file->IsOpen()) {
            spdlog::critical("[Error] in spark: could not open file {}", output_file_name);
            return false;
        }

        // Create tree
        output_tree = std::make_unique<TTree>(output_tree_name.c_str(), output_tree_name.c_str());

        return true;
    }

    /**
     * Writes the tree to file and close it.
     *
     * \return success
     */
    auto save() -> bool
    {
        if (output_file) {
            output_file->cd();

            file_header.Write("FileHeader");

            if (output_tree) {
                output_tree->Write();  // Writing the tree to the file
            }

            // if (event) event->Write("Event"); FIXME

            // output_file->Close();  // and closing the tree (and the file) FIXME do I need this? causes crash
            return true;
        }

        return false;
    }

    /**
     * Fills the current event into the tree.
     *
     * \return status of Fill() method
     */
    auto fill() -> int
    {
        init_branches();

        category_mgr.fill();

        if (!output_tree) {
            return -1;
        }

        // fill tree
        auto status = output_tree->Fill();

        return status;
    }

    /**
     * Opens a file and loads the root tree.
     *
     * \return success
     */
    auto open() -> bool
    {
        input_tree = std::make_unique<TChain>(input_tree_name.c_str());

        if (input_tree == nullptr) {
            spdlog::critical("[Error] in spark: cannot open ROOT file {}", input_tree_name);
            return false;
        }

        for (auto& source : input_sources) {
            // spdlog::info("Add file %{}", inputFiles[i]);
            // input_tree->Add(inputFiles[i].c_str());

            if (!source->open()) {
                spdlog::critical("Could not open source {}", 0);  // TODO add some feedback info
                abort();
            }

            file_header.Write("FileHeader");
        }

        input_tree->SetBranchStatus("*");

        no_entries = input_tree->GetEntriesFast();

        return true;
    }

    /**
     * Init all systems and process the data in the loop.
     * \param entries number to entries to loop over
     * \param show_progress_bar display progress bar
     */
    auto process_data(uint64_t entries, bool /*show_progress_bar*/ = true) -> void;

    /**
     * Reads entry of the current tree.
     * The categories are filled with the data saved in tree entry at index \p i.
     * \param i event number
     */
    auto get_entry(Long64_t idx) -> void
    {
        if (!input_tree) {
            spdlog::warn("[Warning] in spark: no input tree is opened. Cannot get any entry.");
            return;
        }

        if (idx < no_entries) {
            current_entry = idx;
            input_tree->GetEntry(idx);
        }
    }

    /**
     * Returns number of entries in the current tree.
     * \return number of entries
     */
    auto get_entries() -> Long64_t
    {
        if (!input_tree) {
            spdlog::warn("[Warning] in spark: no input tree is opened. Cannot get any entry.");
            return -1;
        }
        no_entries = input_tree->GetEntries();
        return no_entries;
    }

    /**
     * Register category.
     *
     * Every category must be registered first before using it (reading or writing). The detector related categories
     * should be registered inside detector::setup_categories() method.
     *
     * \param cat category ID
     * \param name category name
     * \param dim address dimension
     * \param sizes dimension sizes
     * \param simulation simulation run
     * \return success
     */
    template<typename ECategories>
    auto register_category(ECategories cat,
                           const std::string& name,
                           std::initializer_list<size_t> sizes,
                           bool simulation) -> bool
    {
        return category_mgr.reg<ECategories>(cat, name, sizes, simulation);
    }

    /**
     * Build category based on its ID. Category must be first registered.
     *
     * \param cat category ID
     * \param persistent set category persistent
     * \return pointer to category object
     */
    template<typename T, typename ECategories>
    auto build_category(ECategories cat, bool persistent = true) -> category*
    {
        if (!output_tree) {
            throw std::runtime_error("Output tree not available");
            return nullptr;
        }

        return category_mgr.build<T>(cat, persistent);
    }

    /**
     * Get the category.
     *
     * If category is not open, opens the category from input tree.
     *
     * \param cat category ID
     * \param persistent set category persistent
     * \return pointer to category object
     */
    template<class ECategories>
    auto get_category(ECategories cat, bool persistent = true) -> category*
    {
        return category_mgr.get<ECategories>(cat, persistent);
    }

    /**
     * Setup all categories from registered detectors.
     */
    auto setup_categories() -> void;

    /// Set output file name
    /// \param file file name
    auto set_output_filename(const std::string& file) -> void { output_file_name = file; }

    /// Add source to the list of sources
    /// \param data source object
    auto add_source(data_source* data) -> void { input_sources.push_back(data); }

    /// Get entry number
    /// \return entry number
    auto get_entry_number() const -> Long64_t { return current_entry; }

    /// Get input tree
    /// \return input tree
    auto get_tree() const -> TTree* { return input_tree.get(); }

    /// Set input tree
    /// \param t TTree tree object
    // void set_tree(TTree* t) { input_tree = t; } FIXME

    /// Returns current event.
    /// \return SEvent structure
    auto get_current_event() const -> SEvent* { return event; }

    /**
     * Set event TODO what was that?
     * \param e event
     */
    auto set_event(SEvent* evt) -> void
    {
        //     if (event) delete event;
        event = evt;
    }

    /*******************************************************/
    /*********************** Control ***********************/

    auto init_system() -> void;

    /*******************************************************/
    /************************ Tasks ************************/

    auto setup_tasks() -> void;

    auto setup_containers() -> void;

    /*******************************************************/
    /********************** Unpackers **********************/

    template<typename U, typename... Args>
    auto make_unpacker(Args... args) -> std::shared_ptr<spark::unpacker>
    {
        return std::shared_ptr<spark::unpacker>(new U(this, args...));
    }

    /*******************************************************/
    /*********************** Sources ***********************/

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

private:
    /**
     * Init all branches in the output tree.
     *
     * Uses registered categories info to init branches. If the category is
     * persistent, will be written to output tree. The persistent input category
     * will be rewritten to output tree.
     */
    SPARK_NO_EXPORT void init_branches()
    {
        if (branches_set) {
            return;
        }

        for (auto& cinfo : category_mgr.cinfovec) {
            if (!cinfo.second.persistent) {
                continue;
            }

            output_tree->Branch(cinfo.second.ptr->get_name(), cinfo.second.ptr.get(), 16000, 99);
        }

        branches_set = true;
    }

    /*******************************************************/
    /******************* Files and trees *******************/
    root_file_header file_header;                  ///< Root File Header

    std::unique_ptr<TFile> output_file {nullptr};  ///< Pointer to output file
    std::string output_file_name;                  ///< Output file name

    std::unique_ptr<TTree> output_tree {nullptr};  ///< Pointer to output tree
    std::string output_tree_name;                  ///< Output tree name

    std::unique_ptr<TTree> input_tree {nullptr};   ///< Pointer to input tree
    std::string input_tree_name;                   ///< Input tree name

    /*******************************************************/
    /*********************** Sources ***********************/
    std::vector<data_source*> input_sources;  ///< Input file name

    /*******************************************************/
    /********************** Detectors **********************/
    std::vector<std::shared_ptr<detector>> detectors;

    /*******************************************************/
    /********************* Categories **********************/
    std::size_t categories_hash;  ///< Category hash
    category_manager category_mgr;

    /*******************************************************/
    /************************ Tasks ************************/
    task_manager task_mgr;

    /*******************************************************/
    /*********************** Events ************************/
    int64_t no_entries {-1};     ///< Number of input entries
    int64_t current_entry {-1};  ///< Current input entry number

    SEvent* event {nullptr};     ///< Event info structure

    /*******************************************************/
    /******************** State control ********************/
    bool branches_set {false};          ///< Has branches set
    bool disable_assertations {false};  ///< disable runtime assertions
};

}  // namespace spark
