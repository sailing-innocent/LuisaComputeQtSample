target("dummy_base")
    _config_project({
        project_kind = "static"
    })
    add_deps("lc-core", "lc-runtime", "lc-vstl", "lc-gui", "lc-volk")
    add_files("base/**.cpp")
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
    add_deps("dummy_base")
    add_deps("lc-dsl")
    add_includedirs(".", {public=true})
    add_headerfiles("dummyrt.h")
    add_defines("DUMMY_DLL_EXPORTS")
    add_files("dummyrt.cpp")
target_end()