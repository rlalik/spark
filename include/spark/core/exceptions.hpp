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

class container_parsing_error : public std::runtime_error
{
public:
    explicit container_parsing_error(std::string_view name)
        : std::runtime_error(fmt::format("Error parsing ASCII container for [{}]", name))
    {
    }
};

}  // namespace spark
