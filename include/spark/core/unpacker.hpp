/*************************************************************************
 * Copyright (C) 2025, Rafał Lalik <rafal.lalik@uj.edu.pl>               *
 * All rights reserved.                                                  *
 *                                                                       *
 * For the licensing terms see LICENSE file.                             *
 * For the list of contributors see README.md file.                      *
 *************************************************************************/

#pragma once

#include "spark/spark_export.hpp"

#include "spark/core/task.hpp"

#include <TNamed.h>

#include <cstddef>
#include <cstdint>      // for uint16_t
#include <istream>
#include <sys/types.h>  // for ulong

#include <fmt/format.h>

namespace spark
{

class sparksys;

/**
 * Abstract base class for unpacker. The interface contains of following
 * functions:
 *  * init() - init unpacker, called only once,
 *  * reinit() - called when some of the data has changed, e.g. parameters and
 *    lookup tables,
 *  * execute() - executes the task, accept several fields describing the event
 *    and data passed to the unpacker,
 *  * finalize() - makes final steps, e.g. release containers.
 */
class SPARK_EXPORT unpacker : public TNamed
{
public:
    unpacker(category_manager* catmgr, database* dbmgr, const char* name)
        : TNamed(name, name)
        , cat_mgr {catmgr}
        , db_mgr {dbmgr}
    {
    }

    unpacker(const unpacker&) = delete;
    unpacker(unpacker&&) = delete;

    auto operator=(const unpacker&) -> unpacker& = delete;
    auto operator=(unpacker&&) -> unpacker& = delete;

    ~unpacker() override = default;

    auto model() -> category_manager* { return cat_mgr; }

    auto db() -> database* { return db_mgr; }

    /// Initialize task
    /// \return success
    virtual auto init() -> bool { return true; }

    /// Called when reinitializatoin is required
    /// \return success
    virtual auto reinit() -> bool { return true; }

    /// Execute task
    /// \param event global event number
    /// \param seq_number sequence number (par file)
    /// \param subevent address of subevent
    /// \param buffer data buffer
    /// \param length size of the buffer in bytes (uint8_t)
    /// \return success
    virtual auto execute(ulong event, ulong seq_number, uint16_t subevent, std::istream& source, size_t length)
        -> bool = 0;

    /// Finalize task
    /// \return success
    virtual auto deinit() -> bool { return true; }

    /// Set size of the time bin (sample) in ns
    /// \param bin_value size of the time bin in ns
    // void setSampleTimeBin(float bin_value) { sample_to_ns = bin_value; } TODO remove it?

    /// Set ADC to V conversion
    /// \param coeff conversion coefficient in V/LSB
    // void setADCTomV(float coeff) { ADC_to_mV = coeff; } TODO remove it?

    /// Get size of the time bin
    /// \return time bin size in ns
    // float getSampleTimeBin(void) { return sample_to_ns; } TODO remove it?

    /// Get ADC coefficient conversion
    /// \return coefficient value in V/LSBd
    // float getADCTomV(void) { return ADC_to_mV; } TODO remove it?

private:
    // float sample_to_ns; ///< conversion factor sample to time TODO remove it?
    // float ADC_to_mV;    ///< conversion factor for ADC to V TODO remove it?

    category_manager* cat_mgr {nullptr};
    database* db_mgr {nullptr};
};

}  // namespace spark

template<>
struct fmt::formatter<spark::unpacker> : fmt::formatter<std::string>
{
    static auto format(const spark::unpacker& unpacker, format_context& ctx) -> format_context::iterator
    {
        return fmt::format_to(ctx.out(), "{:32s} at {:p}", unpacker.GetName(), static_cast<const void*>(&unpacker));
    }
};
