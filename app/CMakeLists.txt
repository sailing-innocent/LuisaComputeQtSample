find_package(Qt6 6.6 REQUIRED COMPONENTS Widgets)

qt_standard_project_setup()

qt_add_executable(luisa-qt-sample
        main.cpp
        rhiwindow.cpp
        rhiwindow.h
        rhiwindow.qrc
)

target_link_libraries(luisa-qt-sample PRIVATE Qt6::Widgets Qt6::GuiPrivate luisa-qt-sample-lib)
set_target_properties(luisa-qt-sample PROPERTIES
        MACOSX_BUNDLE TRUE
        WIN32_EXECUTABLE TRUE
)
