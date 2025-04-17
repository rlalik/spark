/*************************************************************************
 * Copyright (C) 2025, Rafał Lalik <rafal.lalik@uj.edu.pl>               *
 * All rights reserved.                                                  *
 *                                                                       *
 * For the licensing terms see LICENSE file.                             *
 * For the list of contributors see README.md file.                      *
 *************************************************************************/

#include <gtest/gtest.h>

#include "test_container.hpp"

#include <spark/parameters/lookup.hpp>

#include <tuple>

#include <TFile.h>

// TEST(TestsLookup, LookupCell)
// {
//     auto lu_cell = spark::lookup_cell {1, 2};
//
//     ASSERT_EQ(lu_cell.val, 0);
//     ASSERT_EQ(lu_cell.min, 1);
//     ASSERT_EQ(lu_cell.max, 2);
//
//     static_assert(spark::is_lookup_cell<spark::lookup_cell>);
// }

// TEST(TestsLookup, LookupChannelDefault)
// {
//     auto lu_chan = spark::lookup_channel();
//     ASSERT_EQ(lu_chan.addr.size(), 0);
//
//     lu_chan.addr = {{0, 1}, {1, 2}};
//
//     ASSERT_EQ(lu_chan.addr.size(), 2);
//
//     ASSERT_EQ(lu_chan.addr[0].val, 0);
//     ASSERT_EQ(lu_chan.addr[0].min, 0);
//     ASSERT_EQ(lu_chan.addr[0].max, 1);
//
//     ASSERT_EQ(lu_chan.addr[1].val, 0);
//     ASSERT_EQ(lu_chan.addr[1].min, 1);
//     ASSERT_EQ(lu_chan.addr[1].max, 2);
// }

// TEST(TestsLookup, LookupChannelInitialized)
// {
//     auto lu_chan = spark::lookup_channel(spark::lookup_cell {0, 1},  // first value
//                                          spark::lookup_cell {2, 3}   // second value
//     );
//
//     ASSERT_EQ(lu_chan.addr.size(), 2);
//
//     ASSERT_EQ(lu_chan.addr[0].val, 0);
//     ASSERT_EQ(lu_chan.addr[0].min, 0);
//     ASSERT_EQ(lu_chan.addr[0].max, 1);
//
//     ASSERT_EQ(lu_chan.addr[1].val, 0);
//     ASSERT_EQ(lu_chan.addr[1].min, 2);
//     ASSERT_EQ(lu_chan.addr[1].max, 3);
// }

// TEST(TestsLookup, LookupAddress)
// {
//     std::function<spark::lookup_channel()> chan_gen = []() -> spark::lookup_channel
//     {
//         return spark::lookup_channel(spark::lookup_cell {0, 1},  // first value
//                                      spark::lookup_cell {2, 3}   // second value
//         );
//     };
//
//     auto lu_addr = spark::lookup_address(0, 16);
//
//     lu_addr.get_channel_or_create(0, chan_gen);
//     lu_addr.get_channel_or_create(10, chan_gen);
//     lu_addr.get_channel_or_create(15, chan_gen);
//     ASSERT_THROW(lu_addr.get_channel_or_create(16, chan_gen), std::out_of_range);
// }

TEST(TestsLookup, LookupTable)
{
    auto lu_table = Lookup1Lut::build("TestTable", 0, 10, 16, "{:x} {}", "{} {}");

    ASSERT_THROW(lu_table.get({100, 0}), spark::lookup_address_out_of_range);
    ASSERT_THROW(lu_table.get({100, 100}), spark::lookup_address_out_of_range);
    ASSERT_THROW(lu_table.get({0, 100}), spark::lookup_channel_out_of_range);
    // ASSERT_NO_THROW(lu_table.get({0, 0}));
    // ASSERT_NO_THROW(lu_table.get({10, 0}));
    // ASSERT_NO_THROW(lu_table.get({10, 15}));
    ASSERT_THROW(lu_table.get({10, 16}), spark::lookup_channel_out_of_range);

    ASSERT_THROW(lu_table.get({5, 0}), std::out_of_range);

    lu_table.insert({5, 0}, {13, 15});

    auto& lu_chan = lu_table.get({5, 0});

    ASSERT_EQ((std::tuple_size<std::remove_reference<decltype(lu_chan)>::type> {}), 2);

    ASSERT_EQ(lu_chan, (std::tuple<int, int> {13, 15}));

    // ASSERT_EQ(lu_chan.addr[0].val, 0);
    // ASSERT_EQ(lu_chan.addr[0].min, 0);
    // ASSERT_EQ(lu_chan.addr[0].max, 1);
    //
    // ASSERT_EQ(lu_chan.addr[1].val, 0);
    // ASSERT_EQ(lu_chan.addr[1].min, 2);
    // ASSERT_EQ(lu_chan.addr[1].max, 3);

    auto f = TFile::Open("TestsLookup_LookupTable.root", "RECREATE");
    lu_table.Write();
    f->Close();
}
