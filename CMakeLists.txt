cmake_minimum_required(VERSION 3.25)

include(cmake/prelude.cmake)

project(
    spark
    VERSION 0.1.1
    DESCRIPTION "Short description"
    HOMEPAGE_URL "https://example.com/"
    LANGUAGES CXX
)

include(cmake/project-is-top-level.cmake)
include(cmake/variables.cmake)

set(BUILD_SHARED_LIBS ON CACHE BOOL "Spark can be only build as shared ATM" FORCE)

# ---- Dependencies ----

set(FETCHCONTENT_TRY_FIND_PACKAGE_MODE OPT_IN)

include(FetchContent)

set(FMT_INSTALL ON)
FetchContent_Declare(
  fmt
  GIT_REPOSITORY https://github.com/fmtlib/fmt
  GIT_TAG        40626af88bd7df9a5fb80be7b25ac85b122d6c21 # v11.2.0
  FIND_PACKAGE_ARGS 11.2.0
)

set(SPDLOG_INSTALL ON)
set(SPDLOG_BUILD_SHARED ON)
set(SPDLOG_SYSTEM_INCLUDE ON)
set(SPDLOG_USE_STD_FORMAT ON)
set(SPDLOG_FMT_EXTERNAL OFF)
set(SPDLOG_FMT_EXTERNAL_HO OFF)
FetchContent_Declare(
  spdlog
  GIT_REPOSITORY https://github.com/gabime/spdlog
  GIT_TAG        486b55554f11c9cccc913e11a87085b2a91f706f # v1.16.0
  # FIND_PACKAGE_ARGS 1.16.0
)

FetchContent_Declare(
  magic_enum
  GIT_REPOSITORY https://github.com/Neargye/magic_enum
  GIT_TAG        e046b69a3736d314fad813e159b1c192eaef92cd # v0.9.7
  # FIND_PACKAGE_ARGS
)

FetchContent_Declare(
  alpaca
  GIT_REPOSITORY https://github.com/p-ranav/alpaca
  GIT_TAG        master
  FIND_PACKAGE_ARGS 0.2.1
)

set(INDICATORS_INSTALL OFF)
FetchContent_Declare(
  indicators
  GIT_REPOSITORY https://github.com/p-ranav/indicators
  GIT_TAG        master
  # FIND_PACKAGE_ARGS
)

block()
  set(BUILD_SHARED_LIBS ON)
  set(SCN_HEADERS_AS_SYSTEM ON)
  set(SCN_USE_EXCEPTIONS ON)
  set(SCN_INSTALL ON)
  FetchContent_Declare(
    scn
    GIT_REPOSITORY https://github.com/eliaskosunen/scnlib
    GIT_TAG        master
    # FIND_PACKAGE_ARGS
  )
  FetchContent_MakeAvailable(scn)
endblock()

# This will try calling find_package() first for all dependencies
FetchContent_MakeAvailable(fmt spdlog magic_enum alpaca indicators)

# We just need a single header from the whole magic_enum
file(COPY
    ${magic_enum_SOURCE_DIR}/include/magic_enum/magic_enum.hpp
  DESTINATION ${PROJECT_BINARY_DIR}/external/spark/external
)

# find ROOT
find_package(ROOT QUIET REQUIRED COMPONENTS Core RIO ROOTNTuple Tree)

configure_file(templates/external_clang-tidy ${CMAKE_CURRENT_BINARY_DIR}/.clang-tidy)

configure_file(templates/spark_config.hpp.in ${CMAKE_CURRENT_BINARY_DIR}/export/spark/spark_config.hpp)

# ---- Declare library ----

message(STATUS "Build *spark*: ${PROJECT_VERSION}  Shared: ${BUILD_SHARED_LIBS}  Type: ${CMAKE_BUILD_TYPE}")

add_subdirectory(source)

# ---- Install rules ----

if(NOT CMAKE_SKIP_INSTALL_RULES)
  include(cmake/install-rules.cmake)
endif()

# ---- Examples ----

if(PROJECT_IS_TOP_LEVEL)
  option(BUILD_EXAMPLES "Build examples tree." "${spark_DEVELOPER_MODE}")
  if(BUILD_EXAMPLES)
    add_subdirectory(example)
  endif()
endif()

# ---- Developer mode ----

if(NOT spark_DEVELOPER_MODE)
  return()
elseif(NOT PROJECT_IS_TOP_LEVEL)
  message(
      AUTHOR_WARNING
      "Developer mode is intended for developers of spark"
  )
endif()

include(cmake/dev-mode.cmake)
