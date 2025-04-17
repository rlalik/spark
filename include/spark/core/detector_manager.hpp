/*************************************************************************
 * Copyright (C) 2025, Rafał Lalik <rafal.lalik@uj.edu.pl>               *
 * All rights reserved.                                                  *
 *                                                                       *
 * For the licensing terms see LICENSE file.                             *
 * For the list of contributors see README.md file.                      *
 *************************************************************************/

#pragma once

#include "spark/spark_export.hpp"

#include <memory>
#include <vector>

namespace spark
{

class detector;

class SPARK_EXPORT detector_manager
{
public:
    template<typename T, typename... Args>
    auto make_detector(Args... args) -> std::shared_ptr<spark::detector>
    {
        return detectors.emplace_back(new T(args...));
    }

    auto begin() { return detectors.begin(); }

    auto end() { return detectors.end(); }

private:
    std::vector<std::shared_ptr<detector>> detectors;
};
}  // namespace spark
