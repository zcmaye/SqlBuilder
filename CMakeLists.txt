cmake_minimum_required(VERSION 3.10)

project(SqlBuilder CXX)

option(BUILD_TESTS "Create test target for SqlBuilder" ON)

set(CMAKE_VISIBILITY_INLINES_HIDDEN ON)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_VISIBILITY_PRESET hidden)

if(MSVC)
	#add_compile_options("/:bigobj  /:utf-8")
endif()

file(GLOB HEADER_FILES "src/*.h" "src/*.hpp")
file(GLOB SOURCE_FILES "src/*.cpp")

include_directories("src")

#add_compile_definitions(PROJECT_PATH="${CMAKE_CURRENT_SOURCE_DIR}")

add_library(SqlBuilder ${SOURCE_FILES})

#target_include_directories(SqlBuilder
#  PUBLIC
#    $<INSTALL_INTERFACE:include>    
#    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
#  PRIVATE
#    ${CMAKE_CURRENT_SOURCE_DIR}/src
#)

if(NOT BUILD_SHARED_LIBS)
  target_compile_definitions(SqlBuilder PUBLIC "SqlBuilder")
endif()

install(
	FILES ${HEADER_FILES} 
	DESTINATION include
)

install(
	TARGETS SqlBuilder 
	RUNTIME DESTINATION bin 
	LIBRARY DESTINATION lib 
	ARCHIVE DESTINATION lib
)

#message(STATUS ${BUILD_TESTS})

if(BUILD_TESTS)
  include_directories("src")
  file(GLOB TEST_SOURCE "tests/*.cpp")
  add_executable(test_sql_builder ${TEST_SOURCE})
  target_link_libraries(test_sql_builder SqlBuilder)
endif()
