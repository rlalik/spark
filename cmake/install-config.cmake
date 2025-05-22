find_package(fmt @fmt_VERSION@ REQUIRED HINTS @CMAKE_INSTALL_PREFIX@)
find_package(spdlog @spdlog_VERSION@ REQUIRED HINTS @CMAKE_INSTALL_PREFIX@)
find_package(alpaca @alpaca_VERSION@ REQUIRED HINTS @CMAKE_INSTALL_PREFIX@)

@PACKAGE_INIT@

message(STATUS "*spark* found: @spark_VERSION@")
include("${CMAKE_CURRENT_LIST_DIR}/spark-targets.cmake")
