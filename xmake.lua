add_rules("mode.release", "mode.debug", "mode.releasedbg")
set_languages("c++20")
includes("lc_options.generated.lua")
includes(lc_dir)
set_config("qt", qt_path)
function tool_target_with_lc(name)
    local bin_name = name .. "_b"
    target(name)
    set_kind("phony")
    add_rules("lc_run_target")
    add_deps(bin_name, "lc-backends-dummy", {
        inherit = false
    })
    before_build(function(target)
        local qt_path = get_config("qt")
        os.addenvs({PATH = qt_path})
    end)
    target_end()

    target(bin_name)
    set_basename(name)
end

includes("app")
includes("src")
