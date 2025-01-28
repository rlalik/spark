cmake_minimum_required(VERSION 3.14)

project(sparkTests LANGUAGES CXX)

include(../cmake/project-is-top-level.cmake)
include(../cmake/folders.cmake)

# ---- Dependencies ----

if(PROJECT_IS_TOP_LEVEL)
  find_package(spark REQUIRED)
  enable_testing()
endif()

# ---- Tests ----

add_executable(spark_test source/spark_test.cpp)
target_link_libraries(spark_test PRIVATE spark::spark)
target_compile_features(spark_test PRIVATE cxx_std_20)

add_test(NAME spark_test COMMAND spark_test)

# ---- End-of-file commands ----

add_folders(Test)
