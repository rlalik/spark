/*************************************************************************
 * Copyright (C) 2025, Rafał Lalik <rafal.lalik@uj.edu.pl>               *
 * All rights reserved.                                                  *
 *                                                                       *
 * For the licensing terms see LICENSE file.                             *
 * For the list of contributors see README.md file.                      *
 *************************************************************************/

#pragma once

namespace spark
{

class sparksys;

class spark_dep
{
public:
    spark_dep() = delete;

    explicit spark_dep(sparksys* sprk)
        : sys {sprk}
    {
    }

    auto spark() -> sparksys* { return sys; }

private:
    sparksys* sys {nullptr};
};

}  // namespace spark
