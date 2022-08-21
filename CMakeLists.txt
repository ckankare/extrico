cmake_minimum_required(VERSION 3.22)

project(
  extrico
  VERSION 1.0
  LANGUAGES CXX)

add_subdirectory(third_party/Catch2)
add_subdirectory(third_party/fmt)

add_subdirectory(src)
add_subdirectory(tests)
