// @(#)lib/base:$Id$
// Author: Rafal Lalik  18/11/2017

/*************************************************************************
 * Copyright (C) 2017-2018, Rafał Lalik.                                 *
 * All rights reserved.                                                  *
 *                                                                       *
 * For the licensing terms see $SiFiSYS/LICENSE.                         *
 * For the list of contributors see $SiFiSYS/README/CREDITS.             *
 *************************************************************************/

#pragma once

#include "spark/spark_export.hpp"

#include "spark/core/spark_dep.hpp"
#include "spark/core/task_manager.hpp"
#include "spark/spark.hpp"

#include <string>

namespace spark
{

class sparksys;

class SPARK_EXPORT detector
    : public spark_dep
    , public TNamed
{
private:
    using spark_dep::spark_dep;

    // constructor
    detector(sparksys* sprk, const std::string& name)
        : spark_dep(sprk)
        , TNamed(name.c_str(), name.c_str())
    {
    }

    template<typename T, typename... Args>
    friend auto spark::sparksys::make_detector(Args... /*args*/) -> T*;

    // friend class spark::sparksys;

public:
    // methods
    /// Initialize categories
    /// \return success
    virtual auto init_categories() -> bool = 0;

    /// Initialize containers
    /// \return success
    virtual auto init_containers() -> bool = 0;

    /// Setup tasks chain
    /// \return success
    virtual auto setup_tasks(spark::task_manager& task_mgr) -> bool = 0;
};

}  // namespace spark
