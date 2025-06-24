/*************************************************************************
 * Copyright (C) 2025, Rafał Lalik <rafal.lalik@uj.edu.pl>               *
 * All rights reserved.                                                  *
 *                                                                       *
 * For the licensing terms see LICENSE file.                             *
 * For the list of contributors see README.md file.                      *
 *************************************************************************/

#pragma once

#include "spark/spark_export.hpp"

#include "spark/core/types.hpp"
#include "spark/utils/demangler.hpp"

#include <initializer_list>
#include <numeric>

#include <Rtypes.h>
#include <TClass.h>
#include <TClonesArray.h>
#include <TObject.h>

#include <fmt/core.h>
#include <fmt/ranges.h>
#include <spdlog/spdlog.h>

namespace spark
{

using types::dim_t;
using locator_t = std::initializer_list<types::loc_t>;

namespace details
{

class SPARK_EXPORT category_internals
{
public:
    // header part
    std::string name;             ///< name of the category
    bool simulation {false};      ///< simulation category
    size_t dim {0};               ///< dimensions
    std::vector<size_t> sizes;    ///< dimension sizes
    std::vector<size_t> offsets;  ///< dimension offsets
    size_t data_size {0};         ///< data array size
    bool writable {false};        ///< is writable

private:
    // index part
    std::map<size_t, int> idxmap;  ///< map of indexes
    // flags
    bool compressed {false};  ///< compressed

public:
    /**
     * Map uncompressed index pos into compressed index val.
     *
     * \param pos input index of the original coordinate
     * \param val index of the mapped value
     * \return false is the object is compressed and no further indexes can be map
     */
    auto set_map_index(size_t pos, int val) -> bool;
    auto get_map_index(size_t pos) const -> int;
    auto get_pos_by_index(int idx) -> size_t;

    /// Is category compressed already
    /// \return compressed
    auto is_compressed() const -> bool { return compressed; }

    /// Get number of categories
    /// \return number
    auto size() const -> size_t { return idxmap.size(); }

    auto clear() -> void;
    auto compress() -> void;

    /**
     * Check whether the locator loc dimension fits to the container dimension
     *
     * \param loc locator
     * \return success
     */
    template<typename Loc = std::initializer_list<size_t>>
    constexpr auto check_dim(Loc loc) -> bool
        requires types::LocatorContainer<Loc>
    {
        if (dim != loc.size()) {
            spdlog::critical("Dimension of locator = {:d} does not fit to category of = {:d}\n", loc.size(), dim);
            return false;
        }
        return true;
    }

    /**
     * Translate n-dimension locator loc into linear i coordinate.
     *
     * \param loc locator
     * \return linear coordinate of the array
     */
    template<typename Loc = std::initializer_list<size_t>>
    constexpr auto loc2pos(Loc loc) -> size_t
        requires types::LocatorContainer<Loc>
    {
        if (!check_dim(loc)) {
            throw std::runtime_error("Dimension mismatch");
        }

        return std::inner_product(loc.begin(), loc.end(), offsets.begin(), types::loc_t {});
    }

    /**
     * Converts uncompressed position into vector of locator values.
     *
     * \param loc locator
     * \return linear coordinate of the array
     */
    constexpr auto pos2loc(size_t pos) -> std::vector<size_t>
    {
        auto ret_val = std::vector<size_t>();
        ret_val.reserve(dim);

        for (auto val : offsets) {
            ret_val.push_back(pos / val);
            pos %= val;
        }

        return ret_val;
    }
};

/**
 * Setup category
 *
 * \param hader category_header object
 * \param name name of the category
 * \param dim number of dimensions
 * \param sizes array of sizes of dimensions
 * \param simulation set true if category for simulation data
 */
SPARK_EXPORT auto setup_header(category_internals& header,
                               const char* name,
                               std::initializer_list<size_t> sizes,
                               bool simulation) -> void;

}  // namespace details

// template<size_t N>
// struct SPARK_EXPORT basic_category : public TObject
// {
//     std::array<int, N> loc;
//     ClassDef(basic_category, 1)
// };

class SPARK_EXPORT category : public TObject
{
private:
    // members
    details::category_internals header;  ///< header information
    TClonesArray* data {nullptr};        ///<-> holds category data

public:
    // constructors
    category() = default;

    /**
     * Constructor
     *
     * \param tclass TClass object
     * \param sizes array of sizes of dimensions
     * \param simulation set true if category for simulation data
     */
    category(TClass* tclass, std::initializer_list<size_t> sizes, bool simulation);

    category(const category&) = delete;
    category(category&&) = delete;

    auto operator=(const category&) -> category& = delete;
    auto operator=(category&&) -> category& = default;

    ~category() override
    {
        data->Clear("C");
        delete data;
    }

    /**
     * Returns a reference to an object at the address from locator n.
     * Dimension of n must match dimension of the container.
     *
     * \param n locator object
     * \return reference to a slot
     */
    template<typename T, typename Loc = std::initializer_list<size_t>>
    auto get_slot(Loc loc) -> T*&
        requires types::LocatorContainer<Loc>
    {
        const auto pos = header.loc2pos(loc);

        if (!header.set_map_index(pos, pos)) {
            spdlog::warn("Category {} was already compressed, can't add new slots.", header.name);
            throw std::runtime_error("Cannot access compressed category");
        }

        // if (get_object<T>(loc) == nullptr) {
        //     throw std::runtime_error(std::format("Cannot read object {} at given location {} in {}:{}",
        //                                          typeid(T).name(),
        //                                          pos,
        //                                          __PRETTY_FUNCTION__,
        //                                          __LINE__));
        //     // return {};  // FIXME what here?
        // };

        return reinterpret_cast<T*&>(data->operator[](types::size_t2int(pos)));
    }

    /**
     * Create a next slot at the end of the array and returns it reference.
     * Awailable only for 1d categories.
     *
     * \bug assure that it really creates new slot, not n+1 which might overlap
     * with existing slot.
     * \return reference to a slot
     */
    template<typename T>
    auto get_next_slot() -> T*&
    {
        if (header.dim != 1) {
            spdlog::critical("getNewSlot allowed only for linear categories.");
            throw std::runtime_error("Dimension mismatch");
        }

        return get_slot<T>({types::int2size_t(data->GetEntries())});
    }

    /**
     * Returns object at the given location.
     *
     * Dimension of loc must match dimension of the container.
     *
     * \param loc object location
     * \return pointer to the object
     */
    template<typename T, typename Loc = std::initializer_list<size_t>>
    auto get_object(Loc loc) -> T*
        requires types::LocatorContainer<Loc>
    {
        auto pos = header.loc2pos(loc);
        auto map_pos = header.get_map_index(pos);
        if (map_pos < 0) {
            return nullptr;
        }

        return static_cast<T*>(data->At(map_pos));
    }

    /**
     * Returns object at given index idx.
     *
     * \param i index
     * \return pointer to the object
     */
    template<typename T>
    auto get_object(int idx) -> T*
    {
        if (!header.is_compressed()) {
            compress();
        }

        return dynamic_cast<T*>(data->At(idx));
    }

    /**
     * Takes slot and creates object at given location.
     *
     * This will overwrite existing object. One should call get_object() before using make_object_unsafe(), e.g.
     *
     * auto obj = cat->get_object<SomeClass>({0, 1, 2});
     * i *f (!obj) obj = cat->make_object_unsafe<SomeClass>({0, 1, 2});
     *
     * Dimension of loc must match dimension of the container.
     *
     * \param loc object location
     * \return pointer to the object
     */
    template<typename T, typename Loc = std::initializer_list<size_t>>
    auto make_object_unsafe(Loc loc) -> T*
        requires types::LocatorContainer<Loc>
    {
        auto obj = get_slot<T>(loc);
        new (obj) T();
        return obj;
    }

    /**
     * Takes next slot and creates object.
     *
     * \return pointer to the object
     */
    template<typename T>
    auto make_next_object() -> T*
    {
        auto obj = get_next_slot<T>();
        const std::tuple ttt = {get_entries()};
        new (obj) T();
        return obj;
    }

    /**
     * Get locator for given object index.
     */
    auto get_locator(types::dim_t idx) -> std::vector<types::dim_t>
    {
        return header.pos2loc(header.is_compressed() ? header.get_pos_by_index(types::size_t2int(idx)) : idx);
    }

    /// Returns name of the container
    /// \return container name
    auto get_name() const -> TString { return header.name; }

    /// Returns number of entries in the category
    /// \return number of entries
    auto get_entries() const -> Int_t { return data->GetEntries(); }

    /// \sa TObject::IsFolder()
    /// \return is a folder
    auto IsFolder() const -> Bool_t override { return kTRUE; }

    /**
     * Compress the mapped indexes. After the category is compressed to linear array
     * of the number of elements size, the mapping of the indexes must be also
     * updated. The compression is linear though the mapped indexes receive new
     * values based of their order in the map.
     * Set compressed flag to true.
     *
     * \sa SCategory::compress()
     */
    auto compress() -> void;

    auto clear() -> void;

    auto print() const -> void;

    auto begin() -> TIter { return data->begin(); }

    auto end() -> TIter { return data->begin(); }

    ClassDefOverride(category, 1)
};

}  // namespace spark
