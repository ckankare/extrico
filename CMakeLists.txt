cmake_minimum_required(VERSION 3.22)

project(
  extrico
  VERSION 1.0
  LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

add_subdirectory(third_party/Catch2)
add_subdirectory(third_party/fmt)

add_subdirectory(src)
add_subdirectory(tests)
