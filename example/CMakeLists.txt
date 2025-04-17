cmake_minimum_required(VERSION 3.14)

project(sparkExamples CXX)

include(../cmake/project-is-top-level.cmake)
include(../cmake/folders.cmake)

if(PROJECT_IS_TOP_LEVEL)
  find_package(spark REQUIRED)
endif()

add_custom_target(run-examples)

function(add_example NAME)
  add_executable("${NAME}" "${NAME}.cpp")
  target_link_libraries("${NAME}" PRIVATE spark::spark)
  # target_compile_features("${NAME}" PRIVATE cxx_std_20)
  add_custom_target("run_${NAME}" COMMAND "${NAME}" VERBATIM)
  add_dependencies("run_${NAME}" "${NAME}")
  add_dependencies(run-examples "run_${NAME}")
endfunction()

add_example(example_writer_tree)
add_example(example_reader_tree)
add_example(category_mgr_demo)

ROOT_GENERATE_DICTIONARY(G__lib_writer_tree_cc
    ${CMAKE_SOURCE_DIR}/example/example_categories.hpp
    spark/core/category.hpp

    MODULE example_writer_tree
    LINKDEF Linkdef.h
)

ROOT_GENERATE_DICTIONARY(G__lib_reader_tree_cc
    ${CMAKE_SOURCE_DIR}/example/example_categories.hpp
    spark/core/category.hpp

    MODULE example_reader_tree
    LINKDEF Linkdef.h
)

ROOT_GENERATE_DICTIONARY(G__lib_category_mgr_demo_cc
    ${CMAKE_SOURCE_DIR}/example/example_categories.hpp
    spark/core/category.hpp

    MODULE category_mgr_demo
    LINKDEF Linkdef.h
)

add_folders(Example)
