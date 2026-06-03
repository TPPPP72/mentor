cmake_minimum_required(VERSION 3.25)

project(mentor)

add_library(mentor_lib STATIC)
target_compile_features(mentor_lib PRIVATE cxx_std_20)
target_include_directories(mentor_lib
  PUBLIC
    include
    thirdparty
)
target_sources(mentor_lib
  PRIVATE
    src/Base/Codec.cpp
    src/Tool/ConfigIO.cpp
)

add_subdirectory(src/CUI)