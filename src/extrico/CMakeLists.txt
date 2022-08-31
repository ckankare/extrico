set(SOURCES
    src/bits_view.cpp
    src/layout.cpp
    src/lexer.cpp
    src/parser.cpp
    src/scanner.cpp
    src/token.cpp
)

add_library(libextrico ${SOURCES})
target_compile_features(libextrico PUBLIC cxx_std_20)
set_target_properties(libextrico PROPERTIES CXX_EXTENSIONS OFF)

target_include_directories(libextrico
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/include/extrico
)

target_link_libraries(libextrico fmt::fmt)
