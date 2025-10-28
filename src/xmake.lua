target("dummy_base")
    _config_project({
        project_kind = "static"
    })
    add_deps("lc-core", "lc-runtime", "lc-vstl", "lc-gui", "lc-volk")
    add_files("base/**.cpp")
target_end()

target("dummyrt")
    _config_project({
        project_kind = "shared"
    })
    add_deps("dummy_base")
    add_deps("lc-dsl")
    add_defines("LUISA_QT_SAMPLE_ENABLE_DX", "LUISA_QT_SAMPLE_ENABLE_VK")
    add_includedirs(".", {public=true})
    add_headerfiles("dummyrt.h")
    add_defines("DUMMY_DLL_EXPORTS")
    add_files("dummyrt.cpp")
target_end()