/*************************************************************************
 * Copyright (C) 2025, Rafał Lalik <rafal.lalik@uj.edu.pl>               *
 * All rights reserved.                                                  *
 *                                                                       *
 * For the licensing terms see LICENSE file.                             *
 * For the list of contributors see README.md file.                      *
 *************************************************************************/

#include <gtest/gtest.h>

#include <spark/core/types.hpp>

#include <limits>
#include <stdexcept>

using spark::types::to_int;

TEST(TestTypes, ToInt)
{
    ASSERT_EQ(to_int(0u), 0);
    ASSERT_EQ(to_int(10u), 10);
    ASSERT_EQ(to_int(static_cast<unsigned int>(std::numeric_limits<int>::max())), std::numeric_limits<int>::max());
    // std::numeric_limits<int>::max() == 2147483647
    ASSERT_THROW(to_int(2147483647u + 1u), std::out_of_range);
}

TEST(TestTypes, DimT2Int) {}
