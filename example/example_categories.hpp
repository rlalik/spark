#pragma once

#include <spark/parameters/lookup.hpp>
#include <spark/parameters/tabular.hpp>

#include <cstdint>

#include <TObject.h>

enum class DummyCategories : std::uint8_t
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

enum class ExampleCategories : std::uint8_t
{
    ExampleRaw = 20,
    ExampleCal = 21,
};

struct ExampleRaw : public TObject
{
    ExampleRaw() = default;

    ExampleRaw(int _board, int _channel)
        : board {_board}
        , channel {_channel}
    {
    }

    int board {-1};
    int channel {-1};
    int toa {0};
    int tot {0};

    ClassDef(ExampleRaw, 1)
};

struct ExampleCal : public TObject
{
    ExampleCal() = default;

    int board {-1};
    int channel {-1};
    float toa {0};
    float energy {0};

    ClassDef(ExampleCal, 1)
};

using ExampleLookup = spark::lookup_table<std::tuple<size_t, size_t>, std::tuple<size_t, size_t>>;
using ExampleCalPar = spark::tabular_par<std::tuple<size_t, size_t>, std::tuple<int, int>>;
