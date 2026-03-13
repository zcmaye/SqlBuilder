cmake_minimum_required(VERSION 3.10)

project(SqlBuilder CXX)

set(EXPORT_NAME SqlBuilder)

set(CMAKE_VISIBILITY_INLINES_HIDDEN ON)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_VISIBILITY_PRESET hidden)

include(GNUInstallDirs)

set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules)

if(NOT DEFINED SQLBUILDER_PARENTPATH)
    set(SQLBUILDER_PARENTPATH ${CMAKE_CURRENT_SOURCE_DIR}/../)
endif()

if(NOT DEFINED SQLBUILDER_HEADERPATH)
    set(SQLBUILDER_HEADERPATH ${CMAKE_CURRENT_SOURCE_DIR}/../SqlBuilder/src/)
endif()

if(NOT DEFINED SQLBUILDER_SOURCEPATH)
    set(SQLBUILDER_SOURCEPATH ${CMAKE_CURRENT_SOURCE_DIR}/../SqlBuilder/src/)
endif()

option(BUILD_TESTS "Create test target for SqlBuilder" OFF)
# Due historical reasons this value is kept off
option(BUILD_SHARED_LIBS "Build in shared lib mode" OFF)

if(MSVC)
	#add_compile_options("/:bigobj  /:utf-8")
endif()

file(GLOB HEADER_FILES "src/*.h" "src/*.hpp")
file(GLOB SOURCE_FILES "src/*.cpp")

#include_directories("src")
add_library(SqlBuilder ${SOURCE_FILES})

add_library(SqlBuilder::SqlBuilder ALIAS SqlBuilder)


if (BUILD_SHARED_LIBS)
    target_compile_definitions(SqlBuilder PUBLIC SqlBuilder_SHAREDLIB)
endif()

target_include_directories(SqlBuilder
  PRIVATE
    ${SQLBUILDER_HEADERPATH}
  PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
    $<INSTALL_INTERFACE:include>    
)

if(NOT BUILD_SHARED_LIBS)
  target_compile_definitions(SqlBuilder PUBLIC "SqlBuilder_STATICLIB")
endif()

set_target_properties(SqlBuilder PROPERTIES
    OUTPUT_NAME ${EXPORT_NAME}
    VERSION ${PROJECT_VERSION}
    SOVERSION ${PROJECT_VERSION_MAJOR}
    PUBLIC_HEADER "${HEADER_FILES}"
)

install(TARGETS SqlBuilder
    EXPORT ${EXPORT_NAME}Targets DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR} COMPONENT runtime
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR} COMPONENT devel
    PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/SqlBuilder COMPONENT devel
)

install(EXPORT ${EXPORT_NAME}Targets
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${EXPORT_NAME}/
    FILE ${EXPORT_NAME}Targets.cmake
    NAMESPACE SqlBuilder::
    COMPONENT devel
)

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules/sqlbuilder-config.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/${EXPORT_NAME}Config.cmake
    @ONLY
)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules/sqlbuilder-config-version.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/${EXPORT_NAME}ConfigVersion.cmake
    @ONLY
)
install(
    FILES
        ${CMAKE_CURRENT_BINARY_DIR}/${EXPORT_NAME}Config.cmake
        ${CMAKE_CURRENT_BINARY_DIR}/${EXPORT_NAME}ConfigVersion.cmake
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${EXPORT_NAME}/
)
include(CPackConfig)


if(BUILD_TESTS)
  include_directories("src")
  file(GLOB TEST_SOURCE "tests/*.cpp")
  add_executable(test_sql_builder ${TEST_SOURCE})
  target_link_libraries(test_sql_builder SqlBuilder)
endif()
