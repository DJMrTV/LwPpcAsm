cmake_minimum_required(VERSION 3.28.1)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Debug)
endif()

set (CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_FLAGS_RELEASE "-O2")
set(CMAKE_CXX_FLAGS_DEBUG "-O2")

project(LwPpcAsm)

file( GLOB SOURCE_FILES Source/*.cpp )

add_library( LwPpcAsm STATIC ${SOURCE_FILES} )

target_include_directories( LwPpcAsm PUBLIC Source )

if( GTEST )
  include( FetchContent )
  FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
  )
  set( gtest_force_shared_crt ON CACHE BOOL "" FORCE )
  FetchContent_MakeAvailable( googletest )

  enable_testing()

  file( GLOB TESTS_SOURCE_FILES Tests/*.cpp )

  add_executable( tests ${TESTS_SOURCE_FILES} )

  target_link_libraries( tests GTest::gtest_main LwPpcAsm )

  include( GoogleTest )

  gtest_discover_tests( tests )
endif()
