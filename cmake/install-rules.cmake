if(PROJECT_IS_TOP_LEVEL)
  set(
      CMAKE_INSTALL_INCLUDEDIR "include/spark-${PROJECT_VERSION}"
      CACHE STRING ""
  )
  set_property(CACHE CMAKE_INSTALL_INCLUDEDIR PROPERTY TYPE PATH)
endif()

include(CMakePackageConfigHelpers)
include(GNUInstallDirs)

# find_package(<package>) call for consumers to find this project
set(package spark)

install(
    DIRECTORY
    include/
    "${PROJECT_BINARY_DIR}/export/"
    DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}"
    COMPONENT spark_Development
)

install(
    TARGETS spark_spark
    EXPORT sparkTargets
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

write_basic_package_version_file(
    "${package}ConfigVersion.cmake"
    COMPATIBILITY SameMajorVersion
)

# Allow package maintainers to freely override the path for the configs
set(
    spark_INSTALL_CMAKEDIR "${CMAKE_INSTALL_LIBDIR}/cmake/${package}"
    CACHE STRING "CMake package config location relative to the install prefix"
)
set_property(CACHE spark_INSTALL_CMAKEDIR PROPERTY TYPE PATH)
mark_as_advanced(spark_INSTALL_CMAKEDIR)

install(
    FILES cmake/install-config.cmake
    DESTINATION "${spark_INSTALL_CMAKEDIR}"
    RENAME "${package}Config.cmake"
    COMPONENT spark_Development
)

install(
    FILES "${PROJECT_BINARY_DIR}/${package}ConfigVersion.cmake"
    DESTINATION "${spark_INSTALL_CMAKEDIR}"
    COMPONENT spark_Development
)

install(
    EXPORT sparkTargets
    NAMESPACE spark::
    DESTINATION "${spark_INSTALL_CMAKEDIR}"
    COMPONENT spark_Development
)

if(PROJECT_IS_TOP_LEVEL)
  include(CPack)
endif()
