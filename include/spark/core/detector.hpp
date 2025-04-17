/*************************************************************************
 * Copyright (C) 2025, Rafał Lalik <rafal.lalik@uj.edu.pl>               *
 * All rights reserved.                                                  *
 *                                                                       *
 * For the licensing terms see LICENSE file.                             *
 * For the list of contributors see README.md file.                      *
 *************************************************************************/

#pragma once

#include "spark/spark_export.hpp"

#include <string>

#include <TNamed.h>

namespace spark
{

class category_manager;
class database;
class detector_manager;
class task_manager;

class SPARK_EXPORT detector : public TNamed
{
private:
    // constructor
    explicit detector(const char* name)
        : TNamed(name, name)
    {
    }

    friend class spark::detector_manager;

public:
    // methods
    /// Initialize categories
    /// \return success
    virtual auto setup_categories(spark::category_manager& cat_mgr) -> void = 0;

    /// Initialize containers
    /// \return success
    virtual auto setup_containers(spark::database& rundb) -> void = 0;

    /// Setup tasks chain
    /// \return success
    virtual auto setup_tasks(spark::task_manager& task_mgr) -> void = 0;
};

}  // namespace spark
