if(PROJECT_IS_TOP_LEVEL)
  set(
      CMAKE_INSTALL_INCLUDEDIR "include/spark-${PROJECT_VERSION}"
      CACHE STRING ""
  )
  set_property(CACHE CMAKE_INSTALL_INCLUDEDIR PROPERTY TYPE PATH)
endif()

include(CMakePackageConfigHelpers)
include(GNUInstallDirs)

set(SPARK_INSTALL_PREFIX ${CMAKE_INSTALL_PREFIX} CACHE INTERNAL "")
set(SPARK_INSTALL_BINDIR ${CMAKE_INSTALL_FULL_BINDIR} CACHE INTERNAL "")
set(SPARK_INSTALL_LIBDIR ${CMAKE_INSTALL_FULL_LIBDIR} CACHE INTERNAL "")
set(SPARK_INSTALL_INCDIR ${CMAKE_INSTALL_FULL_INCLUDEDIR} CACHE INTERNAL "")

# find_package(<package>) call for consumers to find this project
set(package spark)

install(
    DIRECTORY
    include/
    "${PROJECT_BINARY_DIR}/export/"
    "${PROJECT_BINARY_DIR}/external/"
    DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}"
    COMPONENT spark_Development
)

install(
    TARGETS spark
    EXPORT spark-targets
    RUNTIME #
    COMPONENT spark_Runtime
    LIBRARY #
    COMPONENT spark_Runtime
    NAMELINK_COMPONENT spark_Development
    ARCHIVE #
    COMPONENT spark_Development
    INCLUDES #
    DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}"
)

install(
    FILES
        ${PROJECT_BINARY_DIR}/source/libspark_rdict.pcm
        ${PROJECT_BINARY_DIR}/source/libspark.rootmap
    DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

write_basic_package_version_file(
    "${package}-config-version.cmake"
    COMPATIBILITY SameMajorVersion
)

# Allow package maintainers to freely override the path for the configs
set(
    spark_INSTALL_CMAKEDIR "${CMAKE_INSTALL_LIBDIR}/cmake/${package}"
    CACHE STRING "CMake package config location relative to the install prefix"
)
set_property(CACHE spark_INSTALL_CMAKEDIR PROPERTY TYPE PATH)
mark_as_advanced(spark_INSTALL_CMAKEDIR)

# install(
#     FILES cmake/install-config.cmake
#     DESTINATION "${spark_INSTALL_CMAKEDIR}"
#     RENAME "${package}-config.cmake"
#     COMPONENT spark_Development
# )

configure_package_config_file(${PROJECT_SOURCE_DIR}/cmake/install-config.cmake
    ${PROJECT_BINARY_DIR}/${package}-config.cmake
    INSTALL_DESTINATION "${spark_INSTALL_CMAKEDIR}"
)

install(
    FILES ${PROJECT_BINARY_DIR}/${package}-config.cmake
    DESTINATION "${spark_INSTALL_CMAKEDIR}"
    COMPONENT spark_Development
)

install(
    FILES "${PROJECT_BINARY_DIR}/${package}-config-version.cmake"
    DESTINATION "${spark_INSTALL_CMAKEDIR}"
    COMPONENT spark_Development
)

install(
    EXPORT spark-targets
    NAMESPACE spark::
    DESTINATION "${spark_INSTALL_CMAKEDIR}"
    COMPONENT spark_Development
)

configure_file(templates/profile.sh.in ${PROJECT_BINARY_DIR}/spark_profile.sh @ONLY)
install(
    FILES
        ${PROJECT_BINARY_DIR}/spark_profile.sh
    DESTINATION "${CMAKE_INSTALL_BINDIR}"
)

if(PROJECT_IS_TOP_LEVEL)
  include(CPack)
endif()
