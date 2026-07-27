cmake_minimum_required(VERSION 3.15)

project(SqlBuilder
    VERSION 1.0.0
    DESCRIPTION "A C++20 SQL Builder library"
    LANGUAGES CXX
)

option(BUILD_TESTS "Build tests" ON)
option(SQLBUILDER_USE_OPTIONAL "Enable std::optional support" ON)
option(SQLBUILDER_USE_OATPP "Enable oatpp support" OFF)
option(BUILD_SHARED_LIBS "Build shared library" OFF)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# 隐藏符号可见性（用于共享库）
set(CMAKE_VISIBILITY_INLINES_HIDDEN ON)
set(CMAKE_CXX_VISIBILITY_PRESET hidden)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

# 编译器警告
if(MSVC)
    add_compile_options(/W4 /utf-8 /bigobj)
else()
    add_compile_options(-Wall -Wextra -Wpedantic -Werror)
endif()

add_library(SqlBuilder
    src/Field.h
    src/Field.cpp
    src/Value.h
    src/Value.cpp
    src/Table.h
    src/Table.cpp
    src/Function.h
    src/SqlBuilder.h
    src/SqlException.h
    src/TableDeclare.h
    src/Config.h
    src/Condition.h
    src/Assign.h
    src/detail/TypeTraits.h
    src/detail/FormatValue.h
    src/detail/StringUtils.h
    src/detail/StringUtils.cpp
)
target_include_directories(SqlBuilder
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
        $<INSTALL_INTERFACE:include>
)

if(SQLBUILDER_USE_OPTIONAL)
    target_compile_definitions(SqlBuilder PUBLIC SQLBUILDER_USE_OPTIONAL)
endif()
if(SQLBUILDER_USE_OATPP)
    target_compile_definitions(SqlBuilder PUBLIC SQLBUILDER_USE_OATPP)
endif()

# 安装
include(GNUInstallDirs)
install(TARGETS SqlBuilder
    EXPORT SqlBuilderTargets
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
)
# 安装头文件（注意：*.inc 文件被 Field.h 通过 #include 引用，必须一起安装）
install(DIRECTORY src/ DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
    FILES_MATCHING
        PATTERN "*.h"
        PATTERN "*.hpp"
        PATTERN "*.inc"
)

# ========== 生成 Config 文件以支持 find_package(SqlBuilder) ==========
include(CMakePackageConfigHelpers)

# 1. 生成版本文件
write_basic_package_version_file(
    "${CMAKE_CURRENT_BINARY_DIR}/SqlBuilderConfigVersion.cmake"
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY SameMajorVersion
)

# 2. 生成 Config 文件（从模板 SqlBuilderConfig.cmake.in 替换变量）
configure_package_config_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake/SqlBuilderConfig.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/SqlBuilderConfig.cmake"
    INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/SqlBuilder
)

# 3. 导出 Targets 文件（命名空间为 SqlBuilder::，使用方链接 SqlBuilder::SqlBuilder）
install(EXPORT SqlBuilderTargets
    FILE SqlBuilderTargets.cmake
    NAMESPACE SqlBuilder::
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/SqlBuilder
)

# 4. 安装 Config 和 ConfigVersion 文件
install(FILES
    "${CMAKE_CURRENT_BINARY_DIR}/SqlBuilderConfig.cmake"
    "${CMAKE_CURRENT_BINARY_DIR}/SqlBuilderConfigVersion.cmake"
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/SqlBuilder
)

# 测试
if(BUILD_TESTS)
    enable_testing()

    add_executable(test_field tests/test_field.cpp)
    target_link_libraries(test_field PRIVATE SqlBuilder)
    add_test(NAME test_field COMMAND test_field)

    add_executable(test_condition tests/test_condition.cpp)
    target_link_libraries(test_condition PRIVATE SqlBuilder)
    add_test(NAME test_condition COMMAND test_condition)

    add_executable(test_assign tests/test_assign.cpp)
    target_link_libraries(test_assign PRIVATE SqlBuilder)
    add_test(NAME test_assign COMMAND test_assign)

    add_executable(test_select tests/test_select.cpp)
    target_link_libraries(test_select PRIVATE SqlBuilder)
    add_test(NAME test_select COMMAND test_select)

    add_executable(test_insert tests/test_insert.cpp)
    target_link_libraries(test_insert PRIVATE SqlBuilder)
    add_test(NAME test_insert COMMAND test_insert)

    add_executable(test_update_delete tests/test_update_delete.cpp)
    target_link_libraries(test_update_delete PRIVATE SqlBuilder)
    add_test(NAME test_update_delete COMMAND test_update_delete)

    add_executable(test_function tests/test_function.cpp)
    target_link_libraries(test_function PRIVATE SqlBuilder)
    add_test(NAME test_function COMMAND test_function)

    add_executable(test_table tests/test_table.cpp)
    target_link_libraries(test_table PRIVATE SqlBuilder)
    add_test(NAME test_table COMMAND test_table)

    add_executable(test_table_declare tests/test_table_declare.cpp)
    target_link_libraries(test_table_declare PRIVATE SqlBuilder)
    add_test(NAME test_table_declare COMMAND test_table_declare)

    add_executable(test_sql_builder tests/test_sql_builder_xxx.cpp)
    target_link_libraries(test_sql_builder PRIVATE SqlBuilder)
    add_test(NAME test_sql_builder COMMAND test_sql_builder)
endif()
