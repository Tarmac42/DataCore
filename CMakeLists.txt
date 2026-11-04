cmake_minimum_required(VERSION 3.20)
project(DataCore VERSION 0.1.0 LANGUAGES CXX)
include(GNUInstallDirs)
include(CMakePackageConfigHelpers)
include(CTest)

# Interfaces are header-only; concrete backends will be separately reviewed targets.
add_library(DataCore INTERFACE)
add_library(DataCore::DataCore ALIAS DataCore)
target_compile_features(DataCore INTERFACE cxx_std_20)
target_include_directories(DataCore INTERFACE
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>)
install(TARGETS DataCore EXPORT DataCoreTargets)
install(DIRECTORY include/ DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
install(EXPORT DataCoreTargets NAMESPACE DataCore:: DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/DataCore)
configure_package_config_file(cmake/DataCoreConfig.cmake.in
  ${CMAKE_CURRENT_BINARY_DIR}/DataCoreConfig.cmake
  INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/DataCore)
write_basic_package_version_file(${CMAKE_CURRENT_BINARY_DIR}/DataCoreConfigVersion.cmake
  VERSION ${PROJECT_VERSION} COMPATIBILITY SameMajorVersion)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/DataCoreConfig.cmake
  ${CMAKE_CURRENT_BINARY_DIR}/DataCoreConfigVersion.cmake
  DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/DataCore)

if(BUILD_TESTING)
  add_executable(datacore_contracts tests/contracts.cpp)
  target_link_libraries(datacore_contracts PRIVATE DataCore::DataCore)
  add_test(NAME datacore_contracts COMMAND datacore_contracts)
  foreach(header models/RecordEnvelope.hpp writers/RecordWriter.hpp Error.hpp)
    string(MAKE_C_IDENTIFIER "${header}" name)
    file(WRITE ${CMAKE_CURRENT_BINARY_DIR}/${name}.cpp "#include <tarmac/data/${header}>\n")
    add_library(header_${name} OBJECT ${CMAKE_CURRENT_BINARY_DIR}/${name}.cpp)
    target_link_libraries(header_${name} PRIVATE DataCore::DataCore)
  endforeach()
endif()
