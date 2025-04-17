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
  GIT_TAG        v1.15.2
  # FIND_PACKAGE_ARGS 1.15.2
)

# This will try calling find_package() first for all dependencies
FetchContent_MakeAvailable(GTest)

# find ROOT
find_package(ROOT QUIET REQUIRED COMPONENTS Core Hist)

# ---- Tests ----

set(tests_SRCS
    core/tests_category.cpp
    core/tests_container.cpp
    core/tests_database.cpp
    core/tests_lookup.cpp
    core/tests_task_manager.cpp
    core/tests_types.cpp
    core/tests_utils.cpp
)

add_executable(spark_test ${tests_SRCS})
target_link_libraries(spark_test
  PRIVATE
    spark::spark
    GTest::gtest_main
    fmt::fmt
    scn::scn
)
# target_compile_features(spark_test PRIVATE cxx_std_20)

add_test(NAME spark_test COMMAND spark_test)

# ---- End-of-file commands ----

include(GoogleTest)
gtest_discover_tests(spark_test)

add_folders(Test)
