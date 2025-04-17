# ---- Declare library ----

add_library(
    spark

    spark.cpp

    core/category.cpp
    core/data_source.cpp
    core/root_file_header.cpp
    core/root_source.cpp
    core/task_manager.cpp
    core/unpacker.cpp
    core/reader_tree.cpp
    core/writer_tree.cpp

    parameters/container.cpp
    parameters/database.cpp
    parameters/parameters_ascii_source.cpp
)

add_library(spark::spark ALIAS spark)

target_link_libraries(spark
    PUBLIC
        ROOT::Core
        ROOT::RIO
        ROOT::Tree
        ROOT::ROOTNTuple
        fmt::fmt
        alpaca::alpaca
        spdlog::spdlog
        scn::scn
    PRIVATE
        indicators::indicators
)


include(GenerateExportHeader)
generate_export_header(
    spark
    BASE_NAME spark
    EXPORT_FILE_NAME ${PROJECT_BINARY_DIR}/export/spark/spark_export.hpp
    CUSTOM_CONTENT_FROM_VARIABLE pragma_suppress_c4251
)

if(NOT BUILD_SHARED_LIBS)
  target_compile_definitions(spark PUBLIC SPARK_STATIC_DEFINE)
endif()

set_target_properties(
    spark PROPERTIES
    CXX_VISIBILITY_PRESET hidden
    VISIBILITY_INLINES_HIDDEN YES
    VERSION "${PROJECT_VERSION}"
    SOVERSION "${PROJECT_VERSION_MAJOR}"
    EXPORT_NAME spark
    OUTPUT_NAME spark
)

target_include_directories(
    spark ${warning_guard}
    PUBLIC
    "\$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>"
)

target_include_directories(
    spark SYSTEM
    PUBLIC
    "\$<BUILD_INTERFACE:${PROJECT_BINARY_DIR}/export>"
    "\$<BUILD_INTERFACE:${PROJECT_BINARY_DIR}/external>"
)

target_compile_features(spark PUBLIC cxx_std_23)

ROOT_GENERATE_DICTIONARY(G__spark_cc
    ${PROJECT_SOURCE_DIR}/include/spark/core/category.hpp
    ${PROJECT_SOURCE_DIR}/include/spark/parameters/container.hpp
    ${PROJECT_SOURCE_DIR}/include/spark/parameters/lookup.hpp
    ${PROJECT_SOURCE_DIR}/include/spark/parameters/tabular.hpp
    MODULE spark
    LINKDEF Linkdef.h
    NO_CXXMODULE
)
