# LuisaCompute+Qt Sample

这是一个样例来展示如何使用LuisaCompute和Qt搭建比较复杂的桌面端样例

make sure Qt 6.6+ installed
- `xmake l setup.lua #LuisaComput dir#`: Set LC path
- `xmake f --qt="your/qt/sdk/6.6+/msvc"`: Set valid qt path
- `xmake`: compile
- `xmake run rhi_window_sample dx`, run with Direct3D 12 backend (change to `vk` for Vulkan backend)

结果如图所示，UI组件只是为了展示，没有实际效果

![](doc/result.png)