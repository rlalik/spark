#include <string>

#include "spark/spark.hpp"

auto main() -> int
{
  auto const exported = exported_class {};

  return std::string("spark") == exported.name() ? 0 : 1;
}
