add_rules("mode.release", "mode.debug", "mode.releasedbg")
set_languages("c++20")
includes("lc_options.generated.lua")
includes(lc_dir)

includes("app")
includes("src")