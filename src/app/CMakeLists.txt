cmake_minimum_required(VERSION 3.18)

find_package(Qt6 6.2 COMPONENTS Gui Quick REQUIRED)

qt_add_executable(extrico_qt main.cpp)

target_compile_features(extrico_qt PUBLIC cxx_std_20)
target_link_libraries(extrico_qt PRIVATE libextrico fmt::fmt Qt6::Gui Qt6::Quick)
set_property(TARGET extrico_qt PROPERTY AUTOMOC ON)


qt_add_qml_module(extrico_qt
    URI extrico
    VERSION 1.0
    QML_FILES
        app.qml
        Editor.qml
)
