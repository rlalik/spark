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
