# LuisaCompute+Qt Sample

这是一个样例来展示如何使用LuisaCompute和Qt搭建比较复杂的桌面端样例

make sure Qt 6.6+ installed
- `xmake l setup.lua #LuisaComput dir# #QT-SDK dir#`: Set LC path and valid qt path
- `xmake f -c"` config
- `xmake`: compile
- `xmake run rhi_window_sample dx`, run with Direct3D 12 backend (change to `vk` for Vulkan backend)

结果如图所示，UI组件只是为了展示，没有实际效果

![](doc/result.png)