#include <string>

#include "spark/spark.hpp"

exported_class::exported_class()
    : m_name {"spark"}
{
}

auto exported_class::name() const -> char const*
{
  return m_name.c_str();
}
