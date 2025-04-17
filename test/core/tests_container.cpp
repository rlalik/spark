/*************************************************************************
 * Copyright (C) 2025, Rafał Lalik <rafal.lalik@uj.edu.pl>               *
 * All rights reserved.                                                  *
 *                                                                       *
 * For the licensing terms see LICENSE file.                             *
 * For the list of contributors see README.md file.                      *
 *************************************************************************/

#include <gtest/gtest.h>

#include "test_container.hpp"

TEST(TestContainer, SetupDefault)
{
    auto cont = test_container_empty();

    // ASSERT_EQ(cont.lines.size(), 0);
}

TEST(TestContainer, ViolatileContainer)
{
    struct A
    {
    };

    struct B
    {
    };

    auto vcont = spark::violatile_container(typeid(A),
                                            "NoName",
                                            [](std::string_view, spark::parameters_source*, size_t) -> spark::container*
                                            { return nullptr; });

    ASSERT_TRUE(vcont.validate<A>());
    ASSERT_FALSE(vcont.validate<B>());

    ASSERT_NO_THROW(vcont.wrapper<A>());
    ASSERT_THROW(vcont.wrapper<B>(), spark::container_registration_error);
}
