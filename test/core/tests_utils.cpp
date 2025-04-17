/*************************************************************************
 * Copyright (C) 2025, Rafał Lalik <rafal.lalik@uj.edu.pl>               *
 * All rights reserved.                                                  *
 *                                                                       *
 * For the licensing terms see LICENSE file.                             *
 * For the list of contributors see README.md file.                      *
 *************************************************************************/

#include <gtest/gtest.h>

#include <spark/core/types.hpp>
#include <spark/utils/string_functions.hpp>

#include <cstddef>
#include <initializer_list>

#include <spdlog/spdlog.h>

using namespace std::literals::string_literals;

TEST(TestUtilsStringFunctions, Ltrim)
{
    ASSERT_EQ(spark::utils::ltrim_copy("abc"), "abc"s);
    ASSERT_EQ(spark::utils::ltrim_copy(" abc"), "abc"s);
    ASSERT_EQ(spark::utils::ltrim_copy("\tabc"), "abc"s);
    ASSERT_EQ(spark::utils::ltrim_copy(" \tabc"), "abc"s);
    ASSERT_EQ(spark::utils::ltrim_copy("\t abc"), "abc"s);
}

TEST(TestUtilsStringFunctions, Rtrim)
{
    ASSERT_EQ(spark::utils::rtrim_copy("abc"), "abc"s);
    ASSERT_EQ(spark::utils::rtrim_copy("abc "), "abc"s);
    ASSERT_EQ(spark::utils::rtrim_copy("abc\t"), "abc"s);
    ASSERT_EQ(spark::utils::rtrim_copy("abc \t"), "abc"s);
    ASSERT_EQ(spark::utils::rtrim_copy("abc\t "), "abc"s);
}

TEST(TestUtilsStringFunctions, Trim)
{
    ASSERT_EQ(spark::utils::trim_copy("abc"), "abc"s);
    ASSERT_EQ(spark::utils::trim_copy(" abc "), "abc"s);
    ASSERT_EQ(spark::utils::trim_copy(" abc\t"), "abc"s);
    ASSERT_EQ(spark::utils::trim_copy("\t abc \t"), "abc"s);
    ASSERT_EQ(spark::utils::trim_copy("\nabc\t "), "abc"s);
}

TEST(TestUtilsStringFunctions, StripComment)
{
    auto str = "# comment"s;
    spark::utils::strip_comments(str);
    ASSERT_EQ(str, ""s);

    str = " # comment"s;
    spark::utils::strip_comments(str);
    ASSERT_EQ(str, " "s);
}

TEST(TestUtils, ConvertToTuple)
{
    auto list1 = std::initializer_list<size_t> {0, 1, 2, 3};

    spdlog::warn("size is {}  type size is {}", sizeof(list1), sizeof(size_t*));

    // ASSERT_EQ(spark::coreutils::convert_to_tuple<int>({}),
    //           (std::tuple<> {}));

    // ASSERT_EQ(spark::coreutils::convert_to_tuple<size_t>({0}), (std::tuple<size_t> {0}));
    // ASSERT_EQ(spark::coreutils::convert_to_tuple<size_t>({0, 1}), (std::tuple<size_t, size_t> {0, 1}));
    // ASSERT_EQ(spark::coreutils::convert_to_tuple<size_t>({0, 1, 2}), (std::tuple<size_t, size_t, size_t> {0, 1, 2}));
}
