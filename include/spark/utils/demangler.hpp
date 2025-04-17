/*************************************************************************
 * Copyright (C) 2025, Rafał Lalik <rafal.lalik@uj.edu.pl>               *
 * All rights reserved.                                                  *
 *                                                                       *
 * For the licensing terms see LICENSE file.                             *
 * For the list of contributors see README.md file.                      *
 *************************************************************************/

#pragma once

#include "spark/core/types.hpp"

#include <memory>

#ifdef __GNUG__
#    include <cxxabi.h>
#endif

namespace spark::utils
{

/**
 * from https://stackoverflow.com/questions/4939636/function-to-mangle-demangle-functions
 */
inline auto cpp_demangle(const char* abi_name)
#ifdef __GNUG__
{
    //
    // This function allocates and returns storage in ret
    //
    int status {0};
    char* ret = abi::__cxa_demangle(abi_name, nullptr /* output buffer */, nullptr /* length */, &status);

    auto deallocator = ( [](char *mem) { if (mem){ free(static_cast<void*>(mem));
    } } );

    if (status < 0) {
        // 0: The demangling operation succeeded.
        // -1: A memory allocation failure occurred.
        // -2: mangled_name is not a valid name under the C++ ABI mangling rules.
        // -3: One of the arguments is invalid.
        std::unique_ptr<char, decltype(deallocator)> retval(nullptr, deallocator);
        return retval;
    }

    //
    // Create a unique pointer to take ownership of the returned string so it
    // is freed when that pointers goes out of scope
    //
    std::unique_ptr<char, decltype(deallocator)> retval(ret, deallocator);
    return retval;
}
#else
{
    return abi_name;
}
#endif

}  // namespace spark::utils
