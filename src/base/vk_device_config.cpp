#include <volk.h>
#include "vk_device_config.h"
VkDeviceConfig::VkDeviceConfig() {}
VkDeviceConfig::~VkDeviceConfig() {}
void VkDeviceConfig::readback_vulkan_device(
    VkInstance instance,
    VkPhysicalDevice physical_device,
    VkDevice device,
    VkAllocationCallbacks *alloc_callback,
    VkPipelineCacheHeaderVersionOne const &pso_meta,
    VkQueue graphics_queue,
    VkQueue compute_queue,
    VkQueue copy_queue,
    IDxcCompiler3 *dxc_compiler,
    IDxcLibrary *dxc_library,
    IDxcUtils *dxc_utils) noexcept {
    this->instance = instance;
    this->physical_device = physical_device;
    this->device = device;
    this->alloc_callback = alloc_callback;
    this->graphics_queue = graphics_queue;
    this->compute_queue = compute_queue;
    this->copy_queue = copy_queue;
    this->dxc_compiler = dxc_compiler;
    this->dxc_library = dxc_library;
    this->dxc_utils = dxc_utils;
}
luisa::unique_ptr<luisa::compute::DeviceConfigExt> make_vk_device_config(
    void *device,
    void *vk_instance,
    void *vk_physical_device) {
    auto device_config = luisa::make_unique<VkDeviceConfig>();
    device_config->instance = reinterpret_cast<VkInstance>(vk_instance);
    device_config->physical_device = reinterpret_cast<VkPhysicalDevice>(vk_physical_device);
    device_config->device = reinterpret_cast<VkDevice>(device);
    return device_config;
}