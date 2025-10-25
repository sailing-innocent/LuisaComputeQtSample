#pragma once 
#include <luisa/luisa-compute.h>
#include <luisa/dsl/sugar.h>
#include <luisa/gui/input.h>
#include <luisa/gui/window.h>
#include <luisa/runtime/rhi/resource.h>
#include <luisa/backends/ext/native_resource_ext_interface.h>


#ifdef DUMMY_DLL_EXPORTS
    #define DUMMY_API __declspec(dllexport)
#else
    #define DUMMY_API __declspec(dllimport)
#endif
// #define DUMMY_API

struct DUMMY_API App {
    luisa::compute::Device device;
    luisa::compute::Stream stream;
    luisa::compute::CommandList cmd_list;
    luisa::compute::Image<float> framebuffer;
    luisa::compute::Image<float> accum_image;
    luisa::compute::Image<uint> seed_image;
    luisa::compute::Image<float> dummy_image;
    luisa::compute::Shader<2, luisa::compute::Image<float>> clear_shader;
    luisa::compute::Shader<2, luisa::compute::Image<float>, float, luisa::compute::float2> draw_shader;
    luisa::uint2 resolution;
    luisa::Clock clk;
    void init(const char* ws);
    int64_t create_texture(uint width, uint height);
    void update(int64_t dx_texture_handle, uint width, uint height);
    void update();
};