target("rhi_window_sample")
    _config_project({
        project_kind = "binary"
    })
    add_rules("qt.widgetapp")
    add_headerfiles("*.h")
    add_files("*.cpp")
    add_files("rhiwindow.h")
    add_files("rhiwindow.qrc")
    add_deps("dummyrt")
target_end()