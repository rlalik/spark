#include <spark/spark.hpp>

#include "empty_example_categories.hpp"

#include <cstdint>
#include <memory>

#include <fmt/core.h>

// #include <magic_enum/magic_enum.hpp>

// template<typename T, size_t N>
// std::array<std::pair<T, std::string>, N> convertArray(const std::array<std::pair<T, std::string_view>, N>& input)
// {
//     std::array<std::pair<T, std::string>, N> result {};
//     for (std::size_t i = 0; i < N; ++i) {
//         result[i] = {input[i].first, std::string(input[i].second)};
//     }
//     return result;
// }

// template<typename T>
// auto print_each_enum() -> auto
// {
//     constexpr auto entries = magic_enum::enum_entries<T>();
//     for (const auto entry : entries) {
//         fmt::print("{} = {}\n", entry.second, magic_enum::enum_integer(entry.first));
//     }
//
//     return convertArray(entries);
//
//     // magic_enum::enum_for_each<T>(
//     // [](auto val)
//     // {
//     // constexpr std::underlying_type_t<T> v = val;
//     // fmt::print("enum = {:s}\n", magic_enum::enum_name(v));
//     // });
// }

auto main() -> int
{
    spdlog::set_level(spdlog::level::debug);

    auto sprk = spark::sparksys::create<ExampleCategories>("output_example.root", "T");

    sprk.register_category(ExampleCategories::Dummy1d, "Dummy1D", {160}, true);
    sprk.register_category(ExampleCategories::Dummy2d, "Dummy2D", {10, 5}, true);
    sprk.register_category(ExampleCategories::Dummy3d, "Dummy2D", {3, 4, 5}, true);

    sprk.book();

    auto cat1d = sprk.build_category<Dummy1DCategory>(ExampleCategories::Dummy1d, true);
    auto cat2d = sprk.build_category<Dummy2DCategory>(ExampleCategories::Dummy2d, true);
    auto cat3d = sprk.build_category<Dummy3DCategory>(ExampleCategories::Dummy3d, true);

    if (cat1d == nullptr) {
        spdlog::critical("Category \"Dummy1D\" not created.");
        abort();
    }

    if (cat2d == nullptr) {
        spdlog::critical("Category \"Dummy2D\" not created.");
        abort();
    }

    if (cat3d == nullptr) {
        spdlog::critical("Category \"Dummy3D\" not created.");
        abort();
    }

    sprk.init_system();

    for (int i = 0; i < 100; i++) {
        cat1d->clear();
        cat2d->clear();
        cat3d->clear();

        for (int j = 0; j < 5; j++) {
            auto slot = cat1d->get_next_slot<Dummy1DCategory>();
            new (slot) Dummy1DCategory();
            slot->a += i + j;
        }

        {
            auto slot = cat2d->get_slot<Dummy2DCategory>({0, 1});
            new (slot) Dummy2DCategory();
            slot->a = i;
        }

        {
            auto slot = cat3d->get_slot<Dummy3DCategory>({0, 1, 2});
            new (slot) Dummy3DCategory();
            slot->a = i;
        }

        sprk.fill();
    }

    sprk.save();

    // sp.open();

    return 0;
}
