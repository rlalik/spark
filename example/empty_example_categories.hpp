#pragma once

#include <cstdint>

#include <TObject.h>

enum class ExampleCategories : std::uint8_t
{
    Dummy1d = 0,
    Dummy2d = 1,
    Dummy3d = 2,
};

struct Dummy1DCategory : public TObject
{
    Dummy1DCategory() = default;

    int a {13};  ///<

    ClassDef(Dummy1DCategory, 1)
};

struct Dummy2DCategory : public TObject
{
    Dummy2DCategory() = default;

    int a {13};  ///<

    ClassDef(Dummy2DCategory, 1)
};

struct Dummy3DCategory : public TObject
{
    Dummy3DCategory() = default;

    int a {13};  ///<

    ClassDef(Dummy3DCategory, 1)
};
