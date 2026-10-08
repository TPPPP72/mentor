cmake_minimum_required(VERSION 3.25)

project(mentor)

list(APPEND CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake")

option(BUILD_MENTOR_TEST OFF)

add_library(mentor_lib STATIC)
target_compile_features(mentor_lib PRIVATE cxx_std_20)
target_include_directories(mentor_lib
  PUBLIC
    include
    thirdparty
)
target_sources(mentor_lib
  PRIVATE
    src/Codec/Model.cpp
)

if(BUILD_MENTOR_TEST)
  enable_testing()
  add_subdirectory(test)
endif()
