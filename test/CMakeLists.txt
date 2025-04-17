cmake_minimum_required(VERSION 3.14)

project(sparkTests LANGUAGES CXX)

include(../cmake/project-is-top-level.cmake)
include(../cmake/folders.cmake)

# ---- Dependencies ----

if(PROJECT_IS_TOP_LEVEL)
  find_package(spark REQUIRED)
  enable_testing()
endif()

include(FetchContent)

set(INSTALL_GTEST OFF)
FetchContent_Declare(
  GTest
  GIT_REPOSITORY https://github.com/google/googletest
  GIT_TAG        52eb8108c5bdec04579160ae17225d66034bd723 # v1.17.0
  FIND_PACKAGE_ARGS 1.17.0
)

# This will try calling find_package() first for all dependencies
FetchContent_MakeAvailable(GTest)

# find ROOT
find_package(ROOT QUIET REQUIRED COMPONENTS Core Hist)

# ---- Tests ----

add_library(spark_test_lib SHARED source/spark_test.cpp)
target_link_libraries(spark_test_lib
  PUBLIC
    spark::spark
    scn::scn
)

set(tests_SRCS
    core/test_container.hpp
    core/tests_category.cpp
    core/tests_container.cpp
    core/tests_database.cpp
    core/tests_lookup.cpp
    core/tests_tabular.cpp
    core/tests_task_manager.cpp
    core/tests_types.cpp
    core/tests_utils.cpp
)

add_executable(spark_test ${tests_SRCS})
target_link_libraries(spark_test
  PRIVATE
    spark_test_lib
    GTest::gtest_main
    fmt::fmt
)
# target_compile_features(spark_test PRIVATE cxx_std_20)

add_test(NAME spark_test COMMAND spark_test)

ROOT_GENERATE_DICTIONARY(G__spark_test_cc
    ${PROJECT_SOURCE_DIR}/../include/spark/parameters/container.hpp
    ${PROJECT_SOURCE_DIR}/../include/spark/parameters/database.hpp
    ${PROJECT_SOURCE_DIR}/core/test_container.hpp
    MODULE spark_test_lib
    LINKDEF Linkdef.h
)

# ---- End-of-file commands ----

include(GoogleTest)
gtest_discover_tests(spark_test)

add_folders(Test)

configure_file(${PROJECT_SOURCE_DIR}/rootrc ${PROJECT_BINARY_DIR}/../.rootrc)
