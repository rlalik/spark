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
#include <spark/parameters/parameters_ascii_source.hpp>
#include <spark/parameters/tabular.hpp>

#include <TFile.h>

TEST(TestsTabular, TabularPar)
{
    // auto tab_par = Tabular1Par::build("Tabular1", "{:x} {}", "{} {} {}");
    // tab_par->print();

    // ASSERT_THROW(lu_table.get_address(100, 0), std::out_of_range);
    //
    // auto& lu_chan = lu_table.get_address(5, 0);
    //
    // ASSERT_EQ(lu_chan.addr.size(), 2);
    //
    // ASSERT_EQ(lu_chan.addr[0].val, 0);
    // ASSERT_EQ(lu_chan.addr[0].min, 0);
    // ASSERT_EQ(lu_chan.addr[0].max, 1);
    //
    // ASSERT_EQ(lu_chan.addr[1].val, 0);
    // ASSERT_EQ(lu_chan.addr[1].min, 2);
    // ASSERT_EQ(lu_chan.addr[1].max, 3);

    // auto f = TFile::Open("TestsTabular_TabularTable.root", "RECREATE");
    // tab_par.Write();
    // f->Close();
}

TEST(TestsTabular, CreateParametersAsciiSourceBadContent)
{
    auto parfile = create_ascii_par_file_cont();
    auto src_ascii = spark::parameters_ascii_source(parfile.name());

    // ASSERT_NO_THROW(src_ascii.build_container<Tabular1Par>("Tabular1", "{:x} {}", "{} {} {}"));
    //
    // auto cont = src_ascii.get_container<Tabular1Par>("Tabular1", 0);
    //
    // ASSERT_EQ((cont->at({0x1000, 'a'})), (std::tuple {0, 0, 3.14}));
    // ASSERT_EQ((cont->at({0x1000, 'b'})), (std::tuple {0, 1, 2.71}));
    // ASSERT_EQ((cont->at({0x1000, 'c'})), (std::tuple {0, 2, 1115}));
    // ASSERT_EQ((cont->at({0x1001, 'd'})), (std::tuple {1, 0, 137}));
}
