/*************************************************************************
 * Copyright (C) 2025, Rafał Lalik <rafal.lalik@uj.edu.pl>               *
 * All rights reserved.                                                  *
 *                                                                       *
 * For the licensing terms see LICENSE file.                             *
 * For the list of contributors see README.md file.                      *
 *************************************************************************/

#pragma once

#include <spark/parameters/container.hpp>
#include <spark/parameters/lookup.hpp>
#include <spark/parameters/tabular.hpp>

#include <filesystem>

#include <scn/scan.h>
#include <spdlog/spdlog.h>

using Lookup1Lut = spark::lookup_table<std::tuple<int, int>, std::tuple<int, int>>;
using Tabular1Par = spark::tabular_par<std::tuple<int, char>, std::tuple<int, int, double>>;

class test_container_empty : public spark::container
{
public:
    static auto build(std::string_view /*_unused_name*/) -> test_container_empty { return {}; }

    auto from_view(ascii_snapshot& view) -> bool override { return view.size() <= 0; }

    auto to_container(ascii_snapshot& view) const -> void override {}
};

class test_container_lookup : public spark::container
{
public:
    static auto build(std::string_view /*_unused_name*/) -> test_container_lookup { return {}; }

    auto from_view(ascii_snapshot& view) -> bool override
    {
        for (const auto& line : view) {
            // auto result = scn::scan<int, int, int, int>(line, "{:x} {} {} {}");
            //
            // auto [a, b, c, d] = result->values();
            //
            // spdlog::info("Read: {:#x} {} {} {}", a, b, c, d);
        }

        return true;
    }

    auto to_container(ascii_snapshot& view) const -> void override {}
};

namespace fs = std::filesystem;

class tmp_par_file
{
public:
    tmp_par_file()
    {
        if (mkstemp(tmp_name) == -1) {
            throw std::runtime_error("Temporary file could not be created.");
        }
    }

    ~tmp_par_file()
    {
        close();
        fs::remove(name());
    }

    constexpr auto name() -> const char* { return tmp_name; }

    constexpr auto open() -> FILE*
    {
        tmp_ptr = std::fopen(tmp_name, "w+e");
        return tmp_ptr;
    }

    constexpr auto close() -> void
    {
        if (tmp_ptr != nullptr) {
            fclose(tmp_ptr);
            tmp_ptr = nullptr;
        }
    }

private:
    char tmp_name[20] {"/tmp/sparkXXXXXX"};
    FILE* tmp_ptr {nullptr};
};

inline auto create_ascii_par_file_cont() -> tmp_par_file
{
    auto tmpobj = tmp_par_file();
    auto* tmpfile = tmpobj.open();
    fmt::print(tmpfile, " line comment\n");
    fmt::print(tmpfile, "[Empty1]\n");
    fmt::print(tmpfile, "[Lookup1]\n"       // here is the correct lookup table
                         "0x1000 0    0 0\n"
                         "0x1000 1    0 1\n"
                         "0x1000 2    0 2\n"
                         "0x1001 0    1 0\n");
    fmt::print(tmpfile, "[Lookup2]\n"       // another good lookup
                        "0x1000 0    0 0\n"
                        "0x1000 1    0 1\n"
                        "0x1000 2    0 2\n"
                        "0x1001 0    1 0\n");
    fmt::print(tmpfile, "[Lookup3Broken]\n"
                        "0x1000 0    0 0       \n" // extra spaces on purpose
                        " 0x1000 1    0 1 \n"   // extra spaces on purpose
                        "0x1000 2    3  \n"   // here is the broken part
                        "0x1001 0    1 0\n");
    fmt::print(tmpfile, "[Tabular1]\n"
                        "0x1000 a    0 0 3.14\n"
                        "0x1000 b    0 1 2.71\n"
                        "0x1000 c    0 2 1115\n"   // here is the broken part
                        "0x1001 d    1 0 137\n");
    fmt::print(tmpfile, "[Empty2] \t# inline comment\n");
    fmt::print(tmpfile, "[Empty3]\n");
    tmpobj.close();

    return tmpobj;
}
