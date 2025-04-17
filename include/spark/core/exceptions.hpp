/*************************************************************************
 * Copyright (C) 2025, Rafał Lalik <rafal.lalik@uj.edu.pl>               *
 * All rights reserved.                                                  *
 *                                                                       *
 * For the licensing terms see LICENSE file.                             *
 * For the list of contributors see README.md file.                      *
 *************************************************************************/

#pragma once

#include <stdexcept>
#include <string_view>

#include <fmt/core.h>
#include <fmt/format.h>

namespace spark
{

class container_invalid_type : public std::runtime_error
{
public:
    explicit container_invalid_type(std::string_view name)
        : std::runtime_error(fmt::format("Requested [{}] container to be of another type than existing", name))
    {
    }
};

class container_missing_in_source : public std::runtime_error
{
public:
    explicit container_missing_in_source(std::string_view name)
        : std::runtime_error(fmt::format("Error finding container [{}] in any source", name))
    {
    }
};

class container_parsing_error : public std::runtime_error
{
public:
    explicit container_parsing_error(std::string_view name)
        : std::runtime_error(fmt::format("Error parsing ASCII container for [{}]", name))
    {
    }
};

class container_registration_error : public std::runtime_error
{
public:
    explicit container_registration_error(std::string_view name,
                                          std::string_view registered_name,
                                          std::string_view requested_name)
        : std::runtime_error(fmt::format("The container registsred for name [{}] was of type {}, requested type {}",
                                         name,
                                         registered_name,
                                         requested_name))
    {
    }
};

class lookup_address_out_of_range : public std::out_of_range
{
public:
    using out_of_range::out_of_range;
};

class lookup_channel_out_of_range : public std::out_of_range
{
public:
    using out_of_range::out_of_range;
};

}  // namespace spark
