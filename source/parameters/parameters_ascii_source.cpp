/*************************************************************************
 * Copyright (C) 2025, Rafał Lalik <rafal.lalik@uj.edu.pl>               *
 * All rights reserved.                                                  *
 *                                                                       *
 * For the licensing terms see LICENSE file.                             *
 * For the list of contributors see README.md file.                      *
 *************************************************************************/

#include "spark/parameters/parameters_ascii_source.hpp"

#include "spark/parameters/container.hpp"
#include "spark/parameters/parameters_source.hpp"
#include "spark/utils/string_functions.hpp"

#include <cstdlib>
#include <fstream>
#include <regex>
#include <string>
#include <vector>

#include <spdlog/spdlog.h>

/**
 * \class parameters_ascii_source

\ingroup lib_base_database

Reads parameters from ascii file. Can be also used as a target to write parameters.
*/

namespace spark
{
/**
 * Constructor.
 *
 * \param source ascii source file name
 */
parameters_ascii_source::parameters_ascii_source(const std::string& source)
    : source(source)
{
    parse_source();
}

/**
 * Constructor.
 *
 * \param source ascii source file name
 */
parameters_ascii_source::parameters_ascii_source(std::string&& source)
    : source(source)
{
    parse_source();
}

/**
 * Parse source file
 *
 * \return success
 */
auto parameters_ascii_source::parse_source() -> bool
{
    std::ifstream ifs(source);

    if (!ifs.is_open()) {
        auto err_str = fmt::format("Source file {} could not be opened!", source);
        spdlog::critical(err_str);
        throw std::runtime_error(std::move(err_str));
    }

    size_t length = 0;
    if (ifs) {
        ifs.seekg(0, std::ifstream::end);
        length = ifs.tellg();
        ifs.seekg(0, std::ifstream::beg);
    }

    enum WhatNext
    {
        WNContainer,
        WNContainerOrParam,
        WNParam,
        WNParamCont
    } wn = WNContainer;

    const std::regex section_header_pattern(R"(^\[(.+?)\]$)");
    std::string cont_name;
    std::string buff;
    auto cont = container::ascii_snapshot();

    while (getline(ifs, buff)) {
        spark::utils::strip_comments(buff);
        spark::utils::simplify(buff);
        spark::utils::trim(buff);

        // check if comment or empty line
        if (buff[0] == '#' or (buff.empty() and wn != WNParamCont)) {
            continue;
        }

        std::smatch match;

        if (std::regex_match(buff, match, section_header_pattern)) {
            if (!cont_name.empty()) {
                containers_views.emplace(cont_name, cont);
                add_provided_container_name(cont_name);
            }

            cont_name = match[1];
            cont.clear();
        } else {
            cont.push_back(buff);
        }
    }

    if (!cont_name.empty()) {
        containers_views.emplace(cont_name, cont);
        add_provided_container_name(cont_name);
    }

    return true;
}

void parameters_ascii_source::print() const
{
    spdlog::info("=== Ascii Source Info ===");
    spdlog::info("    File name: {}", source);
}

// auto parameters_ascii_source::get_container_view(std::string_view name) -> ascii_source_view
// {
//     auto it = containers_views.find(std::string(name));
//     if (it == containers_views.end()) {
//         throw std::runtime_error(fmt::format("Container [{}] does not exist in the current source", name));
//     }
//
//     return it->second;
// }

}  // namespace spark
