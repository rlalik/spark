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

// #include "spark/core/category.hpp"
// #include "spark/core/data_source.hpp"
// #include "spark/core/root_file_header.hpp"
// #include "spark/core/task_manager.hpp"
// #include "spark/core/types.hpp"
// #include "spark/core/utils.hpp"
//
// #include <TChain.h>
// #include <TClass.h>
// #include <TFile.h>
// #include <TTree.h>
//
// #include <algorithm>
// #include <cstddef>
#include <map>
#include <memory>

#include <spdlog/spdlog.h>

namespace spark
{

class sparksys;

/**
 * \class category_manager
 * \ingroup lib_base
 *
 * Access point to load, read and write data.
 *
 * The main class of the Spark Framework. The spark is responsible for
 * managing all data operations. It loads a root tree from specified file.
 */

class category_manager
{
    std::map<uint8_t, std::string> cat_name;   ///< category type

    struct category_info                       ///< Category info
    {
        bool registered {false};               ///< Category is registered
        bool persistent {false};               ///< Category is persistent
        uint16_t cat {0};                      ///< Category ID
        std::string name;                      ///< Category name
        bool simulation {false};               ///< Simulation run
        std::initializer_list<size_t> sizes;   ///< Dimensions sizes
        std::unique_ptr<category> ptr;         ///< Pointer to category object
    };

    std::map<size_t, category_info> cinfovec;  ///< Category info array

    std::map<uint16_t, category*> categories;  ///< Map of categories

public:
    /**
     * Register category.
     *
     * Every category must be registered first before using it (reading or writing).
     * The detector related categories should be registered inside
     * MDetector::initCategories() method.
     *
     * \param cat category ID
     * \param name category name
     * \param dim address dimension
     * \param sizes dimension sizes
     * \param simulation simulation run
     * \return success
     */
    template<typename ECategories>
    auto reg(ECategories cat, const std::string& name, std::initializer_list<size_t> sizes, bool simulation) -> bool
    {
        auto pos = get_category_index(cat);

        if (cinfovec[pos].registered == true) {
            return true;
        }

        auto& cinfo = cinfovec[pos];
        cinfo.registered = true;
        cinfo.cat = pos;
        cinfo.name = name;
        cinfo.simulation = simulation;
        cinfo.sizes = sizes;
        return true;
    }

    /**
     * Build category based on its ID. Category must be first registered.
     *
     * \param cat category ID
     * \param persistent set category persistent
     * \return pointer to category object
     */
    template<typename T, typename ECategories>
    auto build(ECategories cat, bool persistent = true) -> category*
    {
        auto pos = get_category_index(cat);
        try {
            return categories.at(pos);
        } catch (...) {
            spdlog::info("{}:{}", __PRETTY_FUNCTION__, __LINE__);
        }

        auto& cinfo = cinfovec[pos];
        if (!cinfo.registered) {
            throw std::out_of_range(fmt::format("Category with id {} not registered", pos));
        }

        cinfo.persistent = persistent;
        auto cat_ptr = std::make_unique<category>(TClass::GetClass<T>(), cinfo.sizes, cinfo.simulation);

        if (!cat_ptr) {
            throw std::runtime_error(fmt::format("Category {} could not be created", cinfo.name));
        }

        cinfo.ptr = std::move(cat_ptr);
        categories[pos] = cinfo.ptr.get();
        cat_name[pos] = cinfo.name;
        return cinfo.ptr.get();
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
    template<class T>
    auto get(T cat, bool /*persistent*/ = true) -> category*
    {
        auto pos = get_category_index(cat);
        const auto& cinfo = cinfovec[pos];
        if (!cinfo.registered) {
            return nullptr;
        }
        if (cinfo.ptr) {
            return cinfo.ptr.get();
        }
        // TODO do we need to open category if not exists?
        //     category * c = 0;//openCategory(cat, persistent);
        //     if (c)
        //         return c;

        return nullptr;
    }

    /**
     * Clear all categories.
     */
    auto clear() -> void
    {
        for (auto& element : categories) {
            element.second->clear();
        }
    }

    /**
     * Fills the current event into the tree.
     *
     * \return status of Fill() method
     */
    auto fill() -> void
    {
        // clear the current event data
        for (auto& element : categories) {
            element.second->compress();
        }
    }

    /**
     * Print info about the categories.
     */
    auto print() const -> void
    {
        spdlog::info("There are {} categories in the output tree\n", categories.size());
        for (const auto& element : categories) {
            element.second->print();
        }
    }

private:
    /// Maps category kind and simulation flag into index.
    /// \param cat category kind
    /// \return linearised index of the category
    template<typename T>
    static constexpr auto get_category_index(T cat) -> std::underlying_type_t<T>
    {
        return static_cast<typename std::underlying_type<T>::type>(cat);
    }

    friend sparksys;
};

}  // namespace spark
