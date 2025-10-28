#pragma once

#include <luisa/runtime/rhi/device_interface.h>
#include <luisa/runtime/image.h>
#include <luisa/runtime/rhi/command.h>

luisa::unique_ptr<luisa::compute::DeviceConfigExt> make_vk_device_config(
    void *device,
    void *vk_instance,
    void *vk_physical_device);

luisa::unique_ptr<luisa::compute::DeviceConfigExt> make_dx_device_config(
    void *device,
    bool gpu_dump);

void get_dx_device(
    luisa::compute::DeviceConfigExt *device_config_ext,
    void *&device,
    luisa::uint2 &adaptor_luid);
void get_vk_device(
    luisa::compute::DeviceConfigExt *device_config_ext,
    void *&device,
    void *&physical_device,
    void *&vk_instance,
    uint32_t &gfx_queue_family_index);

enum class D3D12EnhancedResourceUsageType : uint32_t {
    ComputeRead,
    ComputeAccelRead,
    ComputeUAV,
    CopySource,
    CopyDest,
    BuildAccel,
    CopyAccelSrc,
    CopyAccelDst,
    DepthRead,
    DepthWrite,
    IndirectArgs,
    VertexRead,
    IndexRead,
    RenderTarget,
    AccelInstanceBuffer,
    RasterRead,
    RasterAccelRead,
    RasterUAV,
    VideoEncodeRead,
    VideoEncodeWrite,
    VideoProcessRead,
    VideoProcessWrite,
    VideoDecodeRead,
    VideoDecodeWrite,
};
enum class VkResourceUsageType {
    ComputeRead,
    ComputeAccelRead,
    ComputeUAV,
    CopySource,
    CopyDest,
    BuildAccel,
    BuildAccelScratch,
    CopyAccelSrc,
    CopyAccelDst,
    DepthRead,
    DepthWrite,
    IndirectArgs,
    VertexRead,
    IndexRead,
    RenderTarget,
    AccelInstanceBuffer,
    RasterRead,
    RasterAccelRead,
    RasterUAV,
};