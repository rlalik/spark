#include <gtest/gtest.h>

#include "test_container.hpp"

#include <spark/core/exceptions.hpp>
#include <spark/parameters/container.hpp>
#include <spark/parameters/database.hpp>
#include <spark/parameters/lookup.hpp>
#include <spark/parameters/parameters_ascii_source.hpp>

#include <cstdlib>
#include <memory>
#include <string>

#include <fmt/core.h>

using namespace std::string_literals;

TEST(TestParameters, CreateParametersAsciiSource1)
{
    auto parfile = create_ascii_par_file_cont();
    auto src_ascii = spark::parameters_ascii_source(parfile.name());
}

TEST(TestParameters, CreateParametersAsciiSource2)
{
    std::string bad_file {"unknown_file.dat"};
    ASSERT_THROW((spark::parameters_ascii_source(bad_file)), std::runtime_error);
}

TEST(TestParameters, CreateParametersAsciiSourceBadName)
{
    auto parfile = create_ascii_par_file_cont();
    auto src_ascii = spark::parameters_ascii_source(parfile.name());

    ASSERT_THROW(src_ascii.build_container<test_container_empty>("ContainerNotExisting"), std::out_of_range);
}

TEST(TestParameters, CreateParametersAsciiSourceGoodName)
{
    auto parfile = create_ascii_par_file_cont();
    auto src_ascii = spark::parameters_ascii_source(parfile.name());

    ASSERT_NO_THROW(src_ascii.build_container<test_container_empty>("ContainerEmpty1"));

    // auto cont2 = src_ascii.build_container<test_container_empty>("ContainerLookup1");
    // ASSERT_NE(cont2, nullptr);

    ASSERT_NO_THROW(src_ascii.build_container<test_container_empty>("ContainerEmpty2"));
}

TEST(TestParameters, CreateParametersAsciiSourceGoodContent)
{
    auto parfile = create_ascii_par_file_cont();
    auto src_ascii = spark::parameters_ascii_source(parfile.name());

    ASSERT_NO_THROW(src_ascii.build_container<test_container_empty>("ContainerEmpty1"));
}

TEST(TestParameters, CreateParametersAsciiSourceBadContent)
{
    auto parfile = create_ascii_par_file_cont();
    auto src_ascii = spark::parameters_ascii_source(parfile.name());

    ASSERT_NO_THROW(src_ascii.build_container<test_container_lookup>("ContainerLookup1"));

    ASSERT_NO_THROW(src_ascii.build_container<spark::lookup_table>(
        "ContainerLookup2", 0x1000, 0x1001, 64, spark::lookup_cell {0, 1}, spark::lookup_cell {0, 31}));

    ASSERT_THROW(
        src_ascii.build_container<spark::lookup_table>(
            "ContainerLookup3Broken", 0x1000, 0x1001, 64, spark::lookup_cell {0, 1}, spark::lookup_cell {0, 31}),
        spark::container_parsing_error);

    auto cont = src_ascii.get_container<spark::lookup_table>("ContainerLookup2", 0);
    cont->print();
}

TEST(TestParameters, CreateDatabase)
{
    auto db = spark::database();

    auto parfile = create_ascii_par_file_cont();
    auto src_ascii = std::make_unique<spark::parameters_ascii_source>(parfile.name());

    db.add_source(src_ascii.get());
}
