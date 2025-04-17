/*************************************************************************
 * Copyright (C) 2025, Rafał Lalik <rafal.lalik@uj.edu.pl>               *
 * All rights reserved.                                                  *
 *                                                                       *
 * For the licensing terms see LICENSE file.                             *
 * For the list of contributors see README.md file.                      *
 *************************************************************************/

#include "spark/core/category.hpp"

#include "spark/core/types.hpp"

#include <algorithm>
#include <cassert>
#include <cstdio>
#include <functional>
#include <initializer_list>
#include <iterator>
#include <numeric>
#include <stdexcept>
#include <utility>
#include <vector>

#include <TClass.h>
#include <TObject.h>

#include <fmt/base.h>
#include <fmt/core.h>
#include <fmt/format.h>
#include <fmt/ranges.h>
#include <spdlog/spdlog.h>

// #include <magic_enum/magic_enum_utility.hpp>

/**
 * \class category
\ingroup lib_core

A Category abstract interface

*/

namespace spark
{

namespace details
{

auto setup_header(category_internals& header, const char* name, std::initializer_list<size_t> sizes, bool simulation)
    -> void
{
    header.name = name;
    header.dim = sizes.size();
    header.simulation = simulation;
    header.sizes = sizes;
    header.offsets.reserve(header.dim);
    header.offsets.push_back(1);

    /* To calculate the position in linearized array we need to calculate scalar product of location and offsets.
     * Offsets is just a volume on contained dimensions for given dimension. An example, for {2, 5, 4}, the first
     * dimension contains 20 elements, the second contains 4.
     *
     * Thus, the offsets array is {20, 4, 1} See tests for examples.
     */
    std::vector<size_t> tmp = sizes;
    std::reverse(tmp.begin(), tmp.end());  // One need to reverse the sizes

    // Skip the last element because its offset is always 1.
    std::inclusive_scan(tmp.cbegin(), tmp.cend() - 1, std::back_inserter(header.offsets), std::multiplies<> {});
    std::reverse(header.offsets.begin(), header.offsets.end());

    namespace rng = std::ranges;
    header.data_size = rng::fold_left(sizes, 1, std::multiplies());
}

auto category_internals::set_map_index(size_t pos, int val) -> bool
{
    if (compressed) {
        return false;
        // throw std::runtime_error("Category is already compressed");
    }

    idxmap[pos] = val;

    return true;
}

/**
 * Return map index for given position
 *
 * \param pos input index of the original coordinate
 * \return index of the mapped value
 */
auto category_internals::get_map_index(size_t pos) const -> int
{
    const auto& iter = idxmap.find(pos);
    if (iter == idxmap.end()) {
        return -1;
    }
    return iter->second;
}

auto category_internals::get_pos_by_index(int idx) -> size_t
{
    auto find_result = std::find_if(
        std::begin(idxmap), std::end(idxmap), [&](const std::pair<size_t, int>& pair) { return pair.second == idx; });

    if (find_result == idxmap.end()) {
        throw std::runtime_error(fmt::format("Index {} not found in the map.", idx));
    }

    return find_result->first;
}

/**
 * Clear object
 */
auto category_internals::clear() -> void
{
    compressed = false;
    idxmap.clear();
}

auto category_internals::compress() -> void
{
    auto index = 0;
    for (auto& iter : idxmap) {
        iter.second = index++;
    }

    compressed = true;
}

}  // namespace details

category::category(TClass* tclass, std::initializer_list<size_t> sizes, bool simulation)
{
    spdlog::debug(
        "Construct category {} with class {} with sizes {}", header.name, tclass->GetName(), fmt::join(sizes, ","));
    header.clear();
    details::setup_header(header, tclass->GetName(), sizes, simulation);
    data = new TClonesArray(tclass, types::size_t2int(header.data_size));

    spdlog::info("    -> Category {:s} created with linear size of {:d}", tclass->GetName(), header.data_size);
    header.writable = true;
}

// auto category::setup(const char* name, std::initializer_list<size_t> sizes, bool simulation) -> void
// {
//     header.name = name;
//     header.dim = sizeof(sizes);
//     header.simulation = simulation;
//     header.dim_sizes.reserve(header.dim);
//     header.dim_offsets.reserve(header.dim);
//
//     header.dim_offsets[0] = 1;
//     header.dim_sizes = sizes;
//
//     // header.data_size = sizes[0];
//     // for (size_t i = 1; i < dim; ++i) {
//     //     header.dim_offsets[i] = header.dim_offsets[i - 1] * sizes[i - 1];
//     //     header.dim_sizes[i] = sizes[i];
//     //     header.data_size *= sizes[i];
//     // }
//
//     std::inclusive_scan(
//         header.dim_sizes.cbegin(), header.dim_sizes.cend(), header.dim_offsets.begin(), std::multiplies<> {}, 1);
//
//     namespace rng = std::ranges;
//     header.data_size = rng::fold_left(sizes, 1.0, std::multiplies());
//
//     data = TClonesArray(name, header.data_size);
//
//     std::print("Category {:s} created with linear size of {:d}\n", name, header.data_size);
// }

// /**
//  * Returns the object at the address from n locator.
//  * Dimension of n must match dimension of the container.
//  *
//  * \param n locator
//  * \return pointer to an object
//  */
// TObject * category::operator[](const SLocator & n)
// {
//     if (!checkDim(header.dim))     return nullptr;
//
//     size_t pos = loc2pos(n);
//     Int_t p = header.getMapIndex(pos);
//     if (p < 0) return nullptr;
//     return data->ConstructedAt(p);
// }

/**
 * Prints info about the category.
 */
auto category::print() const -> void
{
    fmt::print("Category: {}  length={}  sim={}\n", header.name, header.data_size, header.simulation);
    fmt::print("  header: objects={}  compressed={}\n", header.size(), header.is_compressed());
    fmt::print("  {} objects in the category\n", data->GetEntries());
}

/**
 * Compress the category to reduce size in the memnory. After compression it is
 * not possible to add new slots.
 */
auto category::compress() -> void
{
    if (!header.writable) {
        spdlog::critical("Category not writable");
        return;
    }

    data->Compress();
    header.compress();
}

/**
 * Clear all objects and call Clear() methods of the stored objects.
 */
auto category::clear() -> void
{
    data->Clear("C");
    header.clear();
}

}  // namespace spark
