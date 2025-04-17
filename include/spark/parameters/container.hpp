// @(#)lib/base:$Id$
// Author: Rafal Lalik  18/11/2017

/*************************************************************************
 * Copyright (C) 2017-2018, Rafał Lalik.                                 *
 * All rights reserved.                                                  *
 *                                                                       *
 * For the licensing terms see $SiFiSYS/LICENSE.                         *
 * For the list of contributors see $SiFiSYS/README/CREDITS.             *
 *************************************************************************/

#pragma once

#include "spark/spark_export.hpp"

#include <TNamed.h>

#include <ctime>
#include <string>
#include <string_view>
#include <variant>
#include <vector>

#include <fmt/core.h>

class TFile;

namespace spark
{

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

private:
    validity_range_t validity;  ///< run validity start,end id

    ClassDefOverride(container, 1)
};

}  // namespace spark
