/*************************************************************************
 * Copyright (C) 2025, Rafał Lalik <rafal.lalik@uj.edu.pl>               *
 * All rights reserved.                                                  *
 *                                                                       *
 * For the licensing terms see LICENSE file.                             *
 * For the list of contributors see README.md file.                      *
 *************************************************************************/

#pragma once

#include "spark/spark_export.hpp"

#include "spark/core/data_source.hpp"

#include <memory>
#include <string>

#include <TChain.h>

namespace spark
{

/**
 * Read data from root file. needs better implementation when a such data show
 * up.
 * \todo Implement actually
 */
class SPARK_EXPORT root_source : public data_source
{
public:
    /**
     * Constructor
     * \param tree_name tree name to read from source
     */
    explicit root_source(const std::string& tree_name);

    auto open() -> bool override;
    auto close() -> bool override;
    auto read_current_event() -> bool override;

    /**
     * Set input for the source.
     *
     * \param filename input file name
     */
    auto add_input(const std::string& filename) -> void;

    /// Returns number of entries in the source
    /// \return number of entries
    auto get_entries() const -> int64_t { return chain->GetEntries(); }

private:
    std::unique_ptr<TChain> chain;  ///< TChain to read from the source
};

}  // namespace spark
