target("dummy")    
    _config_project({
        project_kind = "binary"
    })
    add_deps("lc-core", "lc-runtime", "lc-vstl", "lc-gui")
    add_deps("lc-dsl", "lc-backends-dummy")
    add_files("path_tracing_camera.cpp")
target_end()

-- target("dummystand")
--     _config_project({
--         project_kind = "binary"
--     })
--     add_deps("lc-core", "lc-runtime", "lc-vstl", "lc-gui")
--     add_deps("lc-dsl", "lc-backends-dummy")
--     add_files("pt_rt.cpp")
-- target_end()

target("dummyrt")
    _config_project({
        project_kind = "shared"
    })
    
    add_deps("lc-core", "lc-runtime", "lc-vstl", "lc-gui")
    add_deps("lc-dsl", "lc-backends-dummy")
    add_includedirs(".", {public=true})
    add_headerfiles("pt_rt.h")
    add_defines("DUMMY_DLL_EXPORTS")
    add_files("pt_rt.cpp")
target_end()