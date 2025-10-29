qt_binary_target("rhi_window_sample")
    _config_project({
        project_kind = "binary"
    })
    add_rules("qt.widgetapp")
    add_headerfiles("*.h")
    add_files("*.cpp") -- source file
    add_files("rhiwindow.h") -- qt meta header
    add_files("rhiwindow.qrc") -- qt resource files

    add_deps("dummyrt")
target_end()