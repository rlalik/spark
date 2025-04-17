#pragma once

#include <spark/parameters/container.hpp>

#include <filesystem>

#include <scn/scan.h>
#include <spdlog/spdlog.h>

class test_container_empty : public spark::container
{
public:
    static auto build(std::string_view /*_unused_name*/) -> test_container_empty { return {}; }

    auto from_view(ascii_snapshot& view) -> bool override
    {
        spdlog::warn("Function [{}] exception", __PRETTY_FUNCTION__);
        return view.size() <= 0;
    }

    auto to_container(ascii_snapshot& view) const -> void override {}
};

class test_container_lookup : public spark::container
{
public:
    static auto build(std::string_view /*_unused_name*/) -> test_container_lookup { return {}; }

    auto from_view(ascii_snapshot& view) -> bool override
    {
        spdlog::warn("Function [{}] exception", __PRETTY_FUNCTION__);

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
    fmt::print(tmpfile, "[ContainerEmpty1]\n");
    fmt::print(tmpfile, "[ContainerLookup1]\n"
                        "0x1000 0    0 0\n"
                        "0x1000 1    0 1\n"
                        "0x1000 2    0 2\n"
                        "0x1001 0    1 0\n");
    fmt::print(tmpfile, "[ContainerLookup2]\n"
                        "0x1000 0    0 0\n"
                        "0x1000 1    0 1\n"
                        "0x1000 2    0 2\n"
                        "0x1001 0    1 0\n");
    fmt::print(tmpfile, "[ContainerLookup3Broken]\n"
                        "0x1000 0    0 0\n"
                        "0x1000 1    0 1\n"
                        "0x1000 2    0\n"   // here is the broken part
                        "0x1001 0    1 0\n");
    fmt::print(tmpfile, "[ContainerEmpty2] \t# inline comment\n");
    fmt::print(tmpfile, "[ContainerEmpty3]\n");
    tmpobj.close();

    return tmpobj;
}
