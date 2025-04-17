/*************************************************************************
 * Copyright (C) 2025, Rafał Lalik <rafal.lalik@uj.edu.pl>               *
 * All rights reserved.                                                  *
 *                                                                       *
 * For the licensing terms see LICENSE file.                             *
 * For the list of contributors see README.md file.                      *
 *************************************************************************/

#pragma once

#include <cmath>
#include <string>

namespace spark::utils
{

/**
 * Trim from start (in place)
 *
 * \param s string
 */
inline void ltrim(std::string& s)
{
    s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](int ch) { return !std::isspace(ch); }));
}

/**
 * Trim from end (in place)
 *
 * \param s string
 */
inline void rtrim(std::string& s)
{
    s.erase(std::find_if(s.rbegin(), s.rend(), [](int ch) { return !std::isspace(ch); }).base(), s.end());
}

/**
 * Trim from both ends (in place)
 *
 * \param s string
 */
inline void trim(std::string& s)
{
    ltrim(s);
    rtrim(s);
}

/**
 * Trim from start (copying)
 *
 * \param s string
 * \return trimmed string
 */
inline auto ltrim_copy(std::string s) -> std::string
{
    ltrim(s);
    return s;
}

/**
 * Trim from end (copying)
 *
 * \param s string
 * \return trimmed string
 */
inline auto rtrim_copy(std::string s) -> std::string
{
    rtrim(s);
    return s;
}

/**
 * Trim from both ends (copying)
 *
 * \param s string
 * \return trimmed string
 */
inline auto trim_copy(std::string s) -> std::string
{
    trim(s);
    return s;
}

/**
 * Remove all tabs (in place)
 *
 * \param s string
 */
inline void simplify(std::string& s)
{
    size_t pos = 0;
    while (1) {
        pos = s.find_first_of('\t', pos);
        if (pos == std::string::npos) {
            return;
        }
        s.replace(pos, 1, " ");
    }
}

/**
 * Remove all text after comment sign.
 *
 * \param [in-out] s string to process
 */
inline void strip_comments(std::string& s)
{
    auto pos = s.find_first_of('#', 0);
    if (pos == std::string::npos) {
        return;
    }
    s.erase(pos);
}

/**
 * Check if float.
 *
 * \sa Stackoverflow #447206
 *
 * \param str string
 * \return is float
 */
inline auto is_float(const std::string& str) -> bool
{
    std::istringstream iss(str);
    float f = NAN;
    iss >> std::noskipws >> f;
    return iss.eof() && !iss.fail();
}

}  // namespace spark::utils
