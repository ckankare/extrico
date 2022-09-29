add_subdirectory(extrico)
add_subdirectory(app)

add_executable(extrico main.cpp)

target_compile_features(extrico PUBLIC cxx_std_20)
target_link_libraries(extrico PRIVATE libextrico fmt::fmt)
