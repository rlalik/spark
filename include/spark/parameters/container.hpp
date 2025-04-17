/*************************************************************************
 * Copyright (C) 2025, Rafał Lalik <rafal.lalik@uj.edu.pl>               *
 * All rights reserved.                                                  *
 *                                                                       *
 * For the licensing terms see LICENSE file.                             *
 * For the list of contributors see README.md file.                      *
 *************************************************************************/

#pragma once

#include "spark/spark_export.hpp"

#include "spark/core/exceptions.hpp"

#include <TNamed.h>

#include <ctime>
#include <functional>
#include <string>
#include <string_view>
#include <typeindex>
#include <variant>
#include <vector>

#include <fmt/core.h>
#include <spdlog/spdlog.h>

class TFile;

namespace spark
{

class parameters_source;

/**
 * Simple structure to hold data validty range for a single version of container.
 */
struct SPARK_EXPORT validity_range_t
{
private:
    validity_range_t() = default;

    friend class container;

public:
    std::time_t from {0};       ///< beginning of the range
    std::time_t to {0};         ///< end of the range
    std::time_t truncated {0};  ///< if truncate, stores the previous value

    /// Constructor
    /// \param f valid from time
    /// \param t valid to time
    validity_range_t(std::time_t time_from, std::time_t time_to)
        : from(time_from)
        , to(time_to)
    {
    }

    ~validity_range_t() = default;

    /// Compare ranges, the smaller one has lower start time. Ranges may overlap.
    /// \param r range to compare
    /// \return whether the object is smaller than compared
    constexpr auto operator<(const validity_range_t& rhs) const -> bool { return from < rhs.from; }

    /// Compare ranges, the smaller one has lower start time. Ranges may overlap.
    /// \param time time to compare
    /// \return whether the object is smaller than compared
    constexpr auto operator<(std::time_t time) const -> bool { return from < time; }

    /// Checks whether #time is within the range
    /// \param time time to compare
    /// \return whether the #time is within the range
    constexpr auto operator==(std::time_t time) const -> bool { return from <= time and time < to; }

    /// Check for overlap
    /// \param range tested range
    /// \return overlap test result
    constexpr auto check_overlap(const validity_range_t& range) const -> bool { return from < range.to; }

    /// Check for overlap
    /// \param range tested range
    auto truncate(const validity_range_t& range) -> void
    {
        if (check_overlap(range)) {
            truncated = to;
            to = range.from;
        }
    }

    ClassDef(validity_range_t, 1)
};

/// Compare ranges, the smaller one has lower start time. Ranges may overlap.
/// \param lhs left range to compare
/// \param rhs right range to compare
/// return whether the object is smaller than compared
constexpr auto operator<(const validity_range_t& lhs, const validity_range_t& rhs) -> bool
{
    return lhs.from < rhs.from;
}

/**
 * Template that contains basic calibation parameters. Consist of a N-parameters array:
 *  * #par,
 * which interpretation can be any, depends of the user.
 *
 * This structure must provide interface to read and write cal pars to a container. Each deriving class must also
 * provide proper interface.
 *  * read() - reads cal pars from container
 *  * write() - writes cal pars to the container
 *  * print() - print cal par values
 */

/**
 * Stores content of the container read out from the disk.
 */
class SPARK_EXPORT container : public TNamed
{
public:
    container() = default;

    container(std::string_view name, std::string_view title)
        : TNamed(TString(name), TString(title))
    {
    }

    container(const container&) = default;
    container(container&&) = default;

    auto operator=(const container&) -> container& = delete;
    auto operator=(container&&) -> container& = delete;

    ~container() override = default;

    /**
     * Print container content.
     *
     * \param name name to display
     */
    virtual auto print() const -> void {}

    using ascii_snapshot = std::vector<std::string>;

protected:
    virtual auto from_view(ascii_snapshot& view) -> bool = 0;
    virtual auto to_container(ascii_snapshot& view) const -> void = 0;

    friend class parameters_ascii_source;
    friend class parameters_memory_source;

private:
    validity_range_t validity;  ///< run validity start,end id

    ClassDefOverride(container, 1)
};

template<typename T>
class container_wrapper;

class violatile_container
{
    std::type_index type_idx;
    std::string name;
    container* ptr {nullptr};

    using UpdateFunction = std::function<container*(std::string_view name, parameters_source*, size_t run_id)>;
    UpdateFunction init_from_source_function;

public:
    explicit violatile_container(std::type_index&& type_idx, std::string_view name, UpdateFunction init_function)
        : type_idx {std::move(type_idx)}
        , name {name}
        , init_from_source_function {std::move(init_function)}
    {
    }

    violatile_container(const violatile_container&) = delete;
    violatile_container(violatile_container&&) = default;

    auto operator=(const violatile_container&) -> violatile_container& = delete;
    auto operator=(violatile_container&&) -> violatile_container& = delete;

    constexpr auto update(parameters_source* src, size_t run_id)
    {
        if (!ptr) {
            ptr = init_from_source_function(name, src, run_id);
            spdlog::info("    -> Container [{}] was updated", name);
        } else {
            spdlog::info("    -> Container [{}] was created", name);
        }
    }

    template<typename T>
    auto validate() -> bool
    {
        return std::type_index(typeid(T)) == type_idx;
    }

    template<typename T>
    constexpr auto wrapper() -> container_wrapper<T>
    {
        if (!validate<T>()) {
            throw container_registration_error("", type_idx.name(), typeid(T).name());
        }
        return this;
    }

    constexpr auto get() -> container* { return ptr; }
};

template<typename T>
class container_wrapper
{
    violatile_container* vcont {nullptr};

    constexpr container_wrapper(violatile_container* _ptr)
        : vcont {_ptr}
    {
    }

    friend class violatile_container;

public:
    container_wrapper() = default;

    constexpr auto operator->() -> T* { return dynamic_cast<T*>(vcont->get()); }
};

}  // namespace spark
