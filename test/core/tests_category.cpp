/*************************************************************************
 * Copyright (C) 2025, Rafał Lalik <rafal.lalik@uj.edu.pl>               *
 * All rights reserved.                                                  *
 *                                                                       *
 * For the licensing terms see LICENSE file.                             *
 * For the list of contributors see README.md file.                      *
 *************************************************************************/

#include <gtest/gtest.h>

#include <spark/core/category.hpp>
#include <spark/core/types.hpp>

#include <limits>
#include <stdexcept>

using spark::types::LocatorContainer;
using spark::types::to_int;

static_assert(LocatorContainer<std::vector<size_t>>);
static_assert(LocatorContainer<std::initializer_list<size_t>>);

TEST(TestCategory, SetupDefault)
{
    auto hdr = spark::details::category_internals();

    ASSERT_EQ(hdr.name, "");
    ASSERT_EQ(hdr.simulation, false);
    ASSERT_EQ(hdr.dim, 0);
    ASSERT_EQ(hdr.sizes.size(), 0);
    ASSERT_EQ(hdr.sizes, std::vector<size_t> {});
    ASSERT_EQ(hdr.offsets.size(), 0);
    ASSERT_EQ(hdr.offsets, std::vector<size_t> {});
    ASSERT_EQ(hdr.data_size, 0);
    ASSERT_EQ(hdr.writable, false);
}

TEST(TestCategory, SetupSetup1Dim)
{
    auto hdr = spark::details::category_internals();
    spark::details::setup_header(hdr, "test_cat", {10}, true);

    ASSERT_EQ(hdr.name, "test_cat");
    ASSERT_EQ(hdr.simulation, true);
    ASSERT_EQ(hdr.dim, 1);
    ASSERT_EQ(hdr.sizes.size(), 1);
    ASSERT_EQ(hdr.sizes, std::vector<size_t>({10}));
    ASSERT_EQ(hdr.offsets.size(), 1);
    ASSERT_EQ(hdr.offsets, std::vector<size_t>({1}));
    ASSERT_EQ(hdr.data_size, 10);
    ASSERT_EQ(hdr.writable, false);

    ASSERT_EQ(hdr.loc2pos({0}), 0);
    ASSERT_EQ(hdr.loc2pos({5}), 5);
}

TEST(TestCategory, SetupSetup2Dim)
{
    auto hdr = spark::details::category_internals();
    spark::details::setup_header(hdr, "test_cat", {2, 5}, true);

    ASSERT_EQ(hdr.name, "test_cat");
    ASSERT_EQ(hdr.simulation, true);
    ASSERT_EQ(hdr.dim, 2);
    ASSERT_EQ(hdr.sizes.size(), 2);
    ASSERT_EQ(hdr.sizes, std::vector<size_t>({2, 5}));
    ASSERT_EQ(hdr.offsets.size(), 2);
    ASSERT_EQ(hdr.offsets, std::vector<size_t>({5, 1}));
    ASSERT_EQ(hdr.data_size, 10);
    ASSERT_EQ(hdr.writable, false);

    ASSERT_EQ(hdr.loc2pos({0, 0}), 0);
    ASSERT_EQ(hdr.loc2pos({0, 1}), 1);
    ASSERT_EQ(hdr.loc2pos({0, 2}), 2);
    ASSERT_EQ(hdr.loc2pos({0, 3}), 3);
    ASSERT_EQ(hdr.loc2pos({0, 4}), 4);
    ASSERT_EQ(hdr.loc2pos({1, 0}), 5);
    ASSERT_EQ(hdr.loc2pos({1, 1}), 6);
    ASSERT_EQ(hdr.loc2pos({1, 2}), 7);
    ASSERT_EQ(hdr.loc2pos({1, 3}), 8);
    ASSERT_EQ(hdr.loc2pos({1, 4}), 9);

    ASSERT_EQ(hdr.pos2loc(0), (std::vector<size_t> {0, 0}));
    ASSERT_EQ(hdr.pos2loc(1), (std::vector<size_t> {0, 1}));
    ASSERT_EQ(hdr.pos2loc(2), (std::vector<size_t> {0, 2}));
    ASSERT_EQ(hdr.pos2loc(3), (std::vector<size_t> {0, 3}));
    ASSERT_EQ(hdr.pos2loc(4), (std::vector<size_t> {0, 4}));
    ASSERT_EQ(hdr.pos2loc(5), (std::vector<size_t> {1, 0}));
    ASSERT_EQ(hdr.pos2loc(6), (std::vector<size_t> {1, 1}));
    ASSERT_EQ(hdr.pos2loc(7), (std::vector<size_t> {1, 2}));
    ASSERT_EQ(hdr.pos2loc(8), (std::vector<size_t> {1, 3}));
    ASSERT_EQ(hdr.pos2loc(9), (std::vector<size_t> {1, 4}));
}

TEST(TestCategory, SetupSetup3Dim)
{
    auto hdr = spark::details::category_internals();
    spark::details::setup_header(hdr, "test_cat", {2, 5, 4}, true);

    ASSERT_EQ(hdr.name, "test_cat");
    ASSERT_EQ(hdr.simulation, true);
    ASSERT_EQ(hdr.dim, 3);
    ASSERT_EQ(hdr.sizes.size(), 3);
    ASSERT_EQ(hdr.sizes, std::vector<size_t>({2, 5, 4}));
    ASSERT_EQ(hdr.offsets.size(), 3);
    ASSERT_EQ(hdr.offsets, std::vector<size_t>({20, 4, 1}));
    ASSERT_EQ(hdr.data_size, 40);
    ASSERT_EQ(hdr.writable, false);

    ASSERT_EQ(hdr.loc2pos({0, 0, 0}), 0);
    ASSERT_EQ(hdr.loc2pos({0, 0, 1}), 1);
    ASSERT_EQ(hdr.loc2pos({0, 0, 2}), 2);
    ASSERT_EQ(hdr.loc2pos({0, 0, 3}), 3);

    ASSERT_EQ(hdr.loc2pos({0, 1, 0}), 4);
    ASSERT_EQ(hdr.loc2pos({0, 1, 1}), 5);
    ASSERT_EQ(hdr.loc2pos({0, 1, 2}), 6);
    ASSERT_EQ(hdr.loc2pos({0, 1, 3}), 7);

    ASSERT_EQ(hdr.loc2pos({0, 2, 0}), 8);
    ASSERT_EQ(hdr.loc2pos({0, 2, 1}), 9);
    ASSERT_EQ(hdr.loc2pos({0, 2, 2}), 10);
    ASSERT_EQ(hdr.loc2pos({0, 2, 3}), 11);

    ASSERT_EQ(hdr.loc2pos({0, 3, 0}), 12);
    ASSERT_EQ(hdr.loc2pos({0, 3, 1}), 13);
    ASSERT_EQ(hdr.loc2pos({0, 3, 2}), 14);
    ASSERT_EQ(hdr.loc2pos({0, 3, 3}), 15);

    ASSERT_EQ(hdr.loc2pos({0, 4, 0}), 16);
    ASSERT_EQ(hdr.loc2pos({0, 4, 1}), 17);
    ASSERT_EQ(hdr.loc2pos({0, 4, 2}), 18);
    ASSERT_EQ(hdr.loc2pos({0, 4, 3}), 19);

    ASSERT_EQ(hdr.loc2pos({1, 0, 0}), 20);
    ASSERT_EQ(hdr.loc2pos({1, 0, 1}), 21);
    ASSERT_EQ(hdr.loc2pos({1, 0, 2}), 22);
    ASSERT_EQ(hdr.loc2pos({1, 0, 3}), 23);

    ASSERT_EQ(hdr.loc2pos({1, 1, 0}), 24);
    ASSERT_EQ(hdr.loc2pos({1, 1, 1}), 25);
    ASSERT_EQ(hdr.loc2pos({1, 1, 2}), 26);
    ASSERT_EQ(hdr.loc2pos({1, 1, 3}), 27);

    ASSERT_EQ(hdr.loc2pos({1, 2, 0}), 28);
    ASSERT_EQ(hdr.loc2pos({1, 2, 1}), 29);
    ASSERT_EQ(hdr.loc2pos({1, 2, 2}), 30);
    ASSERT_EQ(hdr.loc2pos({1, 2, 3}), 31);

    ASSERT_EQ(hdr.loc2pos({1, 3, 0}), 32);
    ASSERT_EQ(hdr.loc2pos({1, 3, 1}), 33);
    ASSERT_EQ(hdr.loc2pos({1, 3, 2}), 34);
    ASSERT_EQ(hdr.loc2pos({1, 3, 3}), 35);

    ASSERT_EQ(hdr.loc2pos({1, 4, 0}), 36);
    ASSERT_EQ(hdr.loc2pos({1, 4, 1}), 37);
    ASSERT_EQ(hdr.loc2pos({1, 4, 2}), 38);
    ASSERT_EQ(hdr.loc2pos({1, 4, 3}), 39);

    ASSERT_EQ(hdr.pos2loc(0), (std::vector<size_t> {0, 0, 0}));
    ASSERT_EQ(hdr.pos2loc(1), (std::vector<size_t> {0, 0, 1}));
    ASSERT_EQ(hdr.pos2loc(2), (std::vector<size_t> {0, 0, 2}));
    ASSERT_EQ(hdr.pos2loc(3), (std::vector<size_t> {0, 0, 3}));

    ASSERT_EQ(hdr.pos2loc(4), (std::vector<size_t> {0, 1, 0}));
    ASSERT_EQ(hdr.pos2loc(5), (std::vector<size_t> {0, 1, 1}));
    ASSERT_EQ(hdr.pos2loc(6), (std::vector<size_t> {0, 1, 2}));
    ASSERT_EQ(hdr.pos2loc(7), (std::vector<size_t> {0, 1, 3}));

    ASSERT_EQ(hdr.pos2loc(8), (std::vector<size_t> {0, 2, 0}));
    ASSERT_EQ(hdr.pos2loc(9), (std::vector<size_t> {0, 2, 1}));
    ASSERT_EQ(hdr.pos2loc(10), (std::vector<size_t> {0, 2, 2}));
    ASSERT_EQ(hdr.pos2loc(11), (std::vector<size_t> {0, 2, 3}));

    ASSERT_EQ(hdr.pos2loc(12), (std::vector<size_t> {0, 3, 0}));
    ASSERT_EQ(hdr.pos2loc(13), (std::vector<size_t> {0, 3, 1}));
    ASSERT_EQ(hdr.pos2loc(14), (std::vector<size_t> {0, 3, 2}));
    ASSERT_EQ(hdr.pos2loc(15), (std::vector<size_t> {0, 3, 3}));

    ASSERT_EQ(hdr.pos2loc(16), (std::vector<size_t> {0, 4, 0}));
    ASSERT_EQ(hdr.pos2loc(17), (std::vector<size_t> {0, 4, 1}));
    ASSERT_EQ(hdr.pos2loc(18), (std::vector<size_t> {0, 4, 2}));
    ASSERT_EQ(hdr.pos2loc(19), (std::vector<size_t> {0, 4, 3}));

    ASSERT_EQ(hdr.pos2loc(20), (std::vector<size_t> {1, 0, 0}));
    ASSERT_EQ(hdr.pos2loc(21), (std::vector<size_t> {1, 0, 1}));
    ASSERT_EQ(hdr.pos2loc(22), (std::vector<size_t> {1, 0, 2}));
    ASSERT_EQ(hdr.pos2loc(23), (std::vector<size_t> {1, 0, 3}));

    ASSERT_EQ(hdr.pos2loc(24), (std::vector<size_t> {1, 1, 0}));
    ASSERT_EQ(hdr.pos2loc(25), (std::vector<size_t> {1, 1, 1}));
    ASSERT_EQ(hdr.pos2loc(26), (std::vector<size_t> {1, 1, 2}));
    ASSERT_EQ(hdr.pos2loc(27), (std::vector<size_t> {1, 1, 3}));

    ASSERT_EQ(hdr.pos2loc(28), (std::vector<size_t> {1, 2, 0}));
    ASSERT_EQ(hdr.pos2loc(29), (std::vector<size_t> {1, 2, 1}));
    ASSERT_EQ(hdr.pos2loc(30), (std::vector<size_t> {1, 2, 2}));
    ASSERT_EQ(hdr.pos2loc(31), (std::vector<size_t> {1, 2, 3}));

    ASSERT_EQ(hdr.pos2loc(32), (std::vector<size_t> {1, 3, 0}));
    ASSERT_EQ(hdr.pos2loc(33), (std::vector<size_t> {1, 3, 1}));
    ASSERT_EQ(hdr.pos2loc(34), (std::vector<size_t> {1, 3, 2}));
    ASSERT_EQ(hdr.pos2loc(35), (std::vector<size_t> {1, 3, 3}));

    ASSERT_EQ(hdr.pos2loc(36), (std::vector<size_t> {1, 4, 0}));
    ASSERT_EQ(hdr.pos2loc(37), (std::vector<size_t> {1, 4, 1}));
    ASSERT_EQ(hdr.pos2loc(38), (std::vector<size_t> {1, 4, 2}));
    ASSERT_EQ(hdr.pos2loc(39), (std::vector<size_t> {1, 4, 3}));
}

TEST(TestCategory, RangesCompatible)
{
    // static_assert(std::ranges::contiguous_range<spark::category>);
    // static_assert(std::ranges::random_access_range<spark::category>);

    // enum class TestCategory
    // {
    //     Test = 1
    // };
    //
    // // FIXME iterates forever
    // struct TestObject : public TObject
    // {
    //     int a = 0;
    // };
    //
    // auto cat = spark::category(TClass::GetClass<TestObject>(), {10}, false);
    //
    // cat.get_next_slot<TestObject>()->a = 1;
    // cat.get_next_slot<TestObject>()->a = 2;
    // cat.get_next_slot<TestObject>()->a = 3;
    //
    // int counter {0};
    // for (auto c_obj : cat) {
    //     counter++;
    //
    //     spdlog::info("{} -> {}", (void*)c_obj, static_cast<TestObject*>(c_obj)->a);
    //
    //     if (counter == 20) break;
    // }
    //
    // ASSERT_EQ(counter, 3);
}
