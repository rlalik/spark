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

#include "spark/core/detector.hpp"
#include "spark/core/detector_manager.hpp"

#include <format>
#include <map>
#include <memory>

#include <spdlog/spdlog.h>

namespace spark
{

struct category_info  ///< Category info
{
    category_info() = default;

    category_info(const category_info&) = delete;
    category_info(category_info&&) = delete;

    auto operator=(const category_info&) -> category_info& = delete;
    auto operator=(category_info&&) -> category_info& = delete;

    ~category_info() = default;

    bool registered {false};              ///< Category is registered
    bool persistent {false};              ///< Category is persistent
    uint16_t cat_id {0};                  ///< Category ID
    std::string name;                     ///< Category name
    bool simulation {false};              ///< Simulation run
    std::initializer_list<size_t> sizes;  ///< Dimensions sizes
    std::unique_ptr<category> obj;        ///< Category object
    category* ptr {nullptr};              ///< Pointer to category object
};
}  // namespace spark

template<>
struct std::formatter<spark::category_info> : std::formatter<std::string>
{
    auto format(const spark::category_info& cinfo, format_context& ctx) const -> format_context::iterator
    {
        return formatter<std::string>::format(cinfo.name, ctx);
    }
};

/**
 * \class category_manager
 * \ingroup lib_base
 *
 * Access point to load, read and write data.
 *
 * The main class of the Spark Framework. The spark is responsible for
 * managing all data operations. It loads a root tree from specified file.
 */

namespace spark
{

class sparksys;

class category_manager
{
public:
    category_manager() = default;

    /**
     * Get reference to the category_info object. Throws std::out_of_range if category not registered yet.
     *
     * \param cat category ID
     * \return reference to the category_info object
     */
    template<typename ECategories>
    auto get_category_info(ECategories cat) -> category_info&
    {
        auto pos = get_category_index(cat);
        return cinfovec[pos];
    }

    /**
     * Register category.
     *
     * Every category must be registered first before using it (reading or writing). Categories can be registsred
     * manually in the ctaegory manager or within the detector's detector::setup_categories() function.
     * The detector related categories should be registered inside detector::setup_categories().
     * The sizes value is a tuple of dimension sizes. The length of the tuple specifies the category dimension.
     *
     * \param cat category ID
     * \param name category name
     * \param sizes sizes of dimension
     * \param simulation simulation run
     * \return success
     */
    template<typename ECategories>
    auto register_category(ECategories cat,
                           const std::string& name,
                           std::initializer_list<size_t> sizes,
                           bool simulation) -> bool
    {
        auto pos = get_category_index(cat);

        try {
            return cinfovec.at(pos).registered == true;
        } catch (std::out_of_range&) {
            auto& cinfo = cinfovec[pos];
            cinfo.registered = true;
            cinfo.cat_id = pos;
            cinfo.name = name;
            cinfo.simulation = simulation;
            cinfo.sizes = sizes;
            spdlog::info(
                "    -> Category {} registered with sizes: {}  sim: {} in {}", name, sizes, simulation, (void*)this);
        }
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
    auto build_category(ECategories cat, bool persistent = true) -> category*
    {
        auto pos = get_category_index(cat);
        try {
            return categories.at(pos);
        } catch (...) {
        }

        auto& cinfo = cinfovec[pos];
        if (!cinfo.registered) {
            throw std::out_of_range(std::format("Category with id {} not registered", pos));
        }

        cinfo.persistent = persistent;
        cinfo.obj = std::make_unique<category>(TClass::GetClass<T>(), cinfo.sizes, cinfo.simulation);
        cinfo.ptr = cinfo.obj.get();
        categories[pos] = cinfo.ptr;
        cat_name[pos] = cinfo.name;

        return cinfo.ptr;
    }

    /**
     * Build category based on its ID. Category must be first registered.
     *
     * \param cat category ID
     * \param persistent set category persistent
     * \return pointer to category object
     */
    template<typename ECategories>
    auto set_category(ECategories cat, bool persistent = false) -> category_info&
    {
        auto pos = get_category_index(cat);

        auto& cinfo = cinfovec[pos];
        if (!cinfo.registered) {
            throw std::out_of_range(std::format("Category with id {} not registered", pos));
        }

        cinfo.persistent = persistent;
        cinfo.ptr = nullptr;
        categories[pos] = cinfo.ptr;
        cat_name[pos] = cinfo.name;

        return cinfo;
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
    auto get_category(T cat, bool /*persistent*/ = true) -> category*
    {
        auto pos = get_category_index(cat);
        const auto& cinfo = cinfovec[pos];
        if (!cinfo.registered) {
            return nullptr;
        }
        if (cinfo.ptr) {
            return cinfo.ptr;
        }
        // TODO do we need to open category if not exists?
        //     category * c = 0;//openCategory(cat, persistent);
        //     if (c)
        //         return c;

        return nullptr;
    }

    template<typename... Args>
    auto setup_from_detector(Args... detectors) -> void
    {
        spdlog::info(" Setup categories from detector");
        (detectors->setup_categories(*this), ...);
    }

    auto setup_from_detector(detector_manager& det_mgr) -> void
    {
        spdlog::info(" Setup categories using detector manager");
        std::for_each(det_mgr.begin(), det_mgr.end(), [&](auto& det) { det->setup_categories(*this); });
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
    auto compress() -> void
    {
        // clear the current event data
        for (auto& element : categories) {
            element.second->compress();
        }
    }

    /**
     * Print info about the categories.
     */
    auto print_registered() const -> void
    {
        spdlog::info("There are {} registered categories:", cinfovec.size());
        std::print("  -> ");
        for (const auto& element : cinfovec) {
            std::print("  {}", element.second);
        }
        std::print("\n");
    }

    /**
     * Print info about the categories.
     */
    auto print() const -> void
    {
        spdlog::info("There are {} categories in the output tree", categories.size());
        for (const auto& element : categories) {
            element.second->print();
        }
    }

    auto build_from_model(const std::function<void(category_info&)>& func) -> void
    {
        for (auto& [key, element] : cinfovec) {
            func(element);
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

    std::map<uint8_t, std::string> cat_name;   ///< category type
    std::map<size_t, category_info> cinfovec;  ///< Category info array
    std::map<uint16_t, category*> categories;  ///< Map of categories

    friend struct std::formatter<spark::category_info>;
};

}  // namespace spark
