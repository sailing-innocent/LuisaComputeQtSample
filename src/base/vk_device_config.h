#pragma once
#include <luisa/backends/ext/vk_config_ext.h>
#include <volk.h>

using namespace luisa;
using namespace luisa::compute;
struct  VkDeviceConfig : public VulkanDeviceConfigExt {
    VkInstance instance{};
    VkPhysicalDevice physical_device{};
    VkDevice device{};
    VkAllocationCallbacks* alloc_callback{};
    VkQueue graphics_queue{};
    VkQueue compute_queue{};
    VkQueue copy_queue{};
    IDxcCompiler3* dxc_compiler{};
    IDxcLibrary* dxc_library{};
    IDxcUtils* dxc_utils{};
    VkDeviceConfig();
    ~VkDeviceConfig();
    bool load_dxc() const override
    {
        return true; // TODO: we may don't want to load DXC in random target platform
    }
    void readback_vulkan_device(
        VkInstance instance,
        VkPhysicalDevice physical_device,
        VkDevice device,
        VkAllocationCallbacks* alloc_callback,
        VkPipelineCacheHeaderVersionOne const& pso_meta,
        VkQueue graphics_queue,
        VkQueue compute_queue,
        VkQueue copy_queue,
        IDxcCompiler3* dxc_compiler,
        IDxcLibrary* dxc_library,
        IDxcUtils* dxc_utils
    ) noexcept override;
};