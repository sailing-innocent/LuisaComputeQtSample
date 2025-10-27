file(GLOB LUISA_QT_SAMPLE_LIB_SOURCES CONFIGURE_DEPENDS *.cpp)
add_library(luisa-qt-sample-lib SHARED ${LUISA_QT_SAMPLE_LIB_SOURCES})

if (LUISA_QT_SAMPLE_ENABLE_DX)
    target_sources(luisa-qt-sample-lib PRIVATE base/dx_device_config.cpp base/dx_device_config.h)
    target_compile_definitions(luisa-qt-sample-lib PUBLIC LUISA_QT_SAMPLE_ENABLE_DX)
endif ()

if (LUISA_QT_SAMPLE_ENABLE_VK)
    target_sources(luisa-qt-sample-lib PRIVATE base/vk_device_config.cpp base/vk_device_config.h)
    target_compile_definitions(luisa-qt-sample-lib PUBLIC LUISA_QT_SAMPLE_ENABLE_VK)
    target_link_libraries(luisa-qt-sample-lib PRIVATE luisa-compute-ext-volk)
endif ()

if (LUISA_QT_SAMPLE_ENABLE_METAL)
    target_compile_definitions(luisa-qt-sample-lib PUBLIC LUISA_QT_SAMPLE_ENABLE_METAL)
endif ()

target_include_directories(luisa-qt-sample-lib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_compile_definitions(luisa-qt-sample-lib PRIVATE DUMMY_DLL_EXPORTS)
target_link_libraries(luisa-qt-sample-lib PUBLIC luisa::compute)
