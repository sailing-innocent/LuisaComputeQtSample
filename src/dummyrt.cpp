#include "dummyrt.h"
#include "luisa/runtime/rhi/pixel.h"
#include <luisa/backends/ext/native_resource_ext.hpp>
#include "base/device_config.h"

using namespace luisa;
using namespace luisa::compute;

void App::init(const char *ws, const char *backend_name, void *rhi_device, void *rhi_instance /*/only for vulkan*/, void *rhi_physical_device /*only for vulkan*/) {
    Context context(ws);
    luisa::string_view backend = backend_name;
    bool gpu_dump;
#ifdef NDEBUG
    gpu_dump = false;
#else
    gpu_dump = true;
#endif
    DeviceConfig device_config = {};
#ifdef LUISA_QT_SAMPLE_ENABLE_DX
    if (backend == "dx") {
        device_config.extension = make_dx_device_config(rhi_device, gpu_dump);
    }
#endif
#ifdef LUISA_QT_SAMPLE_ENABLE_VK
    if (backend == "vk") {
        device_config.extension = make_vk_device_config(rhi_device, rhi_instance, rhi_physical_device);
    }
#endif
#ifdef LUISA_QT_SAMPLE_ENABLE_METAL
    if (backend == "metal") {
        device_config.extension = nullptr;
    }
#endif
    device_config_ext = device_config.extension.get();
    device = context.create_device(backend, &device_config);
    stream = device.create_stream(StreamTag::GRAPHICS);
    Kernel2D clear_kernel = [](ImageFloat image) noexcept {
        image.write(dispatch_id().xy(), make_float4(1.0f));
    };
    clear_shader = device.compile(clear_kernel);

    Kernel2D draw_kernel = [](ImageFloat image, Float time, Float2 resolution) noexcept {
        auto p = dispatch_id().xy();
        auto uv = make_float2(p) / make_float2(resolution) * 2.0f - 1.0f;
        auto color = def(make_float4());
        Constant scales{pi, luisa::exp(1.f), luisa::sqrt(2.f)};
        for (auto i = 0u; i < 3u; i++) {
            color[i] = cos(time * scales[i] + uv.y * 11.f +
                           sin(-time * scales[2u - i] + uv.x * 7.f) * 4.f) *
                           .5f +
                       .5f;
        }
        color[3] = 1.0f;
        image.write(p, color);
    };
    draw_shader = device.compile(draw_kernel);
}

int64_t App::create_texture(uint width, uint height) {
    dummy_image = device.create_image<float>(PixelStorage::BYTE4, width, height);
    resolution = {width, height};
    return (int64_t)dummy_image.native_handle();
}

void App::update() {
    // cmd_list << clear_shader(dummy_image).dispatch(resolution);
    float2 f_res = {(float)resolution.x, (float)resolution.y};
    cmd_list << draw_shader(dummy_image, clk.toc() * 1e-3, f_res).dispatch(resolution);
    stream << cmd_list.commit();
}
