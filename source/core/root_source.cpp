/*************************************************************************
 * Copyright (C) 2025, Rafał Lalik <rafal.lalik@uj.edu.pl>               *
 * All rights reserved.                                                  *
 *                                                                       *
 * For the licensing terms see LICENSE file.                             *
 * For the list of contributors see README.md file.                      *
 *************************************************************************/

#include "spark/core/root_source.hpp"

#include <memory>
#include <string>

#include <TChain.h>

namespace spark
{
root_source::root_source(const std::string& tree_name)
{
    chain = std::make_unique<TChain>(tree_name.c_str());
}

auto root_source::open() -> bool
{
    return true;
}

auto root_source::close() -> bool
{
    return true;
}

auto root_source::read_current_event() -> bool
{
    return true;
}

auto root_source::add_input(const std::string& filename) -> void
{
    chain->Add(filename.c_str());
}

}  // namespace spark
