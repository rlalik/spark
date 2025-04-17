#include <gtest/gtest.h>

#include <spark/parameters/lookup.hpp>

#include <TFile.h>

TEST(TestsLookup, LookupCell)
{
    auto lu_cell = spark::lookup_cell {1, 2};

    ASSERT_EQ(lu_cell.val, 0);
    ASSERT_EQ(lu_cell.min, 1);
    ASSERT_EQ(lu_cell.max, 2);

    static_assert(spark::is_lookup_cell<spark::lookup_cell>);
}

TEST(TestsLookup, LookupChannelDefault)
{
    auto lu_chan = spark::lookup_channel();
    ASSERT_EQ(lu_chan.addr.size(), 0);

    lu_chan.addr = {{0, 1}, {1, 2}};

    ASSERT_EQ(lu_chan.addr.size(), 2);

    ASSERT_EQ(lu_chan.addr[0].val, 0);
    ASSERT_EQ(lu_chan.addr[0].min, 0);
    ASSERT_EQ(lu_chan.addr[0].max, 1);

    ASSERT_EQ(lu_chan.addr[1].val, 0);
    ASSERT_EQ(lu_chan.addr[1].min, 1);
    ASSERT_EQ(lu_chan.addr[1].max, 2);
}

TEST(TestsLookup, LookupChannelInitialized)
{
    auto lu_chan = spark::lookup_channel(spark::lookup_cell {0, 1},  // first value
                                         spark::lookup_cell {2, 3}   // second value
    );

    ASSERT_EQ(lu_chan.addr.size(), 2);

    ASSERT_EQ(lu_chan.addr[0].val, 0);
    ASSERT_EQ(lu_chan.addr[0].min, 0);
    ASSERT_EQ(lu_chan.addr[0].max, 1);

    ASSERT_EQ(lu_chan.addr[1].val, 0);
    ASSERT_EQ(lu_chan.addr[1].min, 2);
    ASSERT_EQ(lu_chan.addr[1].max, 3);
}

TEST(TestsLookup, LookupAddress)
{
    std::function<spark::lookup_channel()> chan_gen = []() -> spark::lookup_channel
    {
        return spark::lookup_channel(spark::lookup_cell {0, 1},  // first value
                                     spark::lookup_cell {2, 3}   // second value
        );
    };

    auto lu_addr = spark::lookup_address(0, 16);

    lu_addr.get_channel_or_create(0, chan_gen);
    lu_addr.get_channel_or_create(10, chan_gen);
    lu_addr.get_channel_or_create(15, chan_gen);
    ASSERT_THROW(lu_addr.get_channel_or_create(16, chan_gen), std::out_of_range);
}

TEST(TestsLookup, LookupTable)
{
    auto lu_table = spark::lookup_table::build("TestTable",
                                               0,
                                               10,
                                               16,
                                               spark::lookup_cell {0, 1},  // first value
                                               spark::lookup_cell {2, 3}   // second value
    );

    ASSERT_THROW(lu_table.get_address(100, 0), std::out_of_range);

    auto& lu_chan = lu_table.get_address(5, 0);

    ASSERT_EQ(lu_chan.addr.size(), 2);

    ASSERT_EQ(lu_chan.addr[0].val, 0);
    ASSERT_EQ(lu_chan.addr[0].min, 0);
    ASSERT_EQ(lu_chan.addr[0].max, 1);

    ASSERT_EQ(lu_chan.addr[1].val, 0);
    ASSERT_EQ(lu_chan.addr[1].min, 2);
    ASSERT_EQ(lu_chan.addr[1].max, 3);

    auto f = TFile::Open("TestsLookup_LookupTable.root", "RECREATE");
    lu_table.Write();
    f->Close();
}
