#pragma once

#include <luisa/luisa-compute.h>
#include <luisa/dsl/sugar.h>
#include <luisa/gui/input.h>
#include <luisa/gui/window.h>
#include <luisa/runtime/rhi/resource.h>
#include <luisa/backends/ext/native_resource_ext_interface.h>

#ifdef _WIN32
#define DUMMY_DECLSPEC_DLL_EXPORT __declspec(dllexport)
#define DUMMY_DECLSPEC_DLL_IMPORT __declspec(dllimport)
#else
#define DUMMY_DECLSPEC_DLL_EXPORT __attribute__((visibility("default")))
#define DUMMY_DECLSPEC_DLL_IMPORT
#endif

#ifdef DUMMY_DLL_EXPORTS
#define DUMMY_API DUMMY_DECLSPEC_DLL_EXPORT
#else
#define DUMMY_API DUMMY_DECLSPEC_DLL_IMPORT
#endif
// #define DUMMY_API

struct DUMMY_API App {
    luisa::optional<luisa::compute::Context> context;
    luisa::compute::Device device;
    luisa::compute::Stream stream;
    luisa::compute::CommandList cmd_list;
    luisa::compute::Image<float> framebuffer;
    luisa::compute::Image<float> accum_image;
    luisa::compute::Image<uint> seed_image;
    luisa::compute::Image<float> dummy_image;
    luisa::compute::DeviceConfigExt *device_config_ext{};
    luisa::compute::Shader<2, luisa::compute::Image<float>> clear_shader;
    luisa::compute::Shader<2, luisa::compute::Image<float>, float, luisa::compute::float2> draw_shader;
    luisa::uint2 resolution;
    luisa::Clock clk;
    void* vk_physical_device{};
    void* vk_instance{};
    uint32_t vk_queue_family_idx{};
    luisa::uint2 dx_adaptor_luid;

    void init(luisa::compute::Context &&ctx, const char *backend_name);
    int64_t create_texture(uint width, uint height);
    void update();
};
