#pragma once
#include <luisa/runtime/rhi/device_interface.h>
luisa::unique_ptr<luisa::compute::DeviceConfigExt> make_vk_device_config(
    void* device,
    void* vk_instance,
    void* vk_physical_device
);
luisa::unique_ptr<luisa::compute::DeviceConfigExt> make_dx_device_config(
    void* device,
    bool gpu_dump
);