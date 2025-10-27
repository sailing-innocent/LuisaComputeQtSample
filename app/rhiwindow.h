#pragma once 
#include <QWindow>
#include <QOffscreenSurface>
#include <rhi/qrhi.h>
#include "pt_rt.h"

class RhiWindow : public QWindow
{
public:
    RhiWindow();
    QString graphicsApiName() const;
    void releaseSwapChain();

protected:
    virtual void customInit() = 0;
    virtual void customRender() = 0;
    std::unique_ptr<QRhi> m_rhi;
    std::unique_ptr<QRhiSwapChain> m_sc;
    std::unique_ptr<QRhiRenderBuffer> m_ds;
    std::unique_ptr<QRhiRenderPassDescriptor> m_rp;
    bool m_hasSwapChain = false;

private:
    void init();
    void resizeSwapChain();
    void render();

    void exposeEvent(QExposeEvent *) override;
    bool event(QEvent *) override;

    QRhi::Implementation m_graphicsApi = QRhi::D3D12;

    bool m_initialized = false;
    bool m_notExposed = false;
    bool m_newlyExposed = false;
};

class HelloWindow : public RhiWindow
{
public:
    HelloWindow(const char* ws, void* rhi_device, void* rhi_instance /*/only for vulkan*/, void* rhi_physical_device /*only for vulkan*/);
    void customInit() override;
    void customRender() override;

private:
    void ensureFullscreenTexture(const QSize &pixelSize, QRhiResourceUpdateBatch *u);

    std::unique_ptr<QRhiBuffer> m_vbuf;
    std::unique_ptr<QRhiBuffer> m_ubuf;
    std::unique_ptr<QRhiTexture> m_texture;
    std::unique_ptr<QRhiSampler> m_sampler;
    std::unique_ptr<QRhiShaderResourceBindings> m_colorTriSrb;
    std::unique_ptr<QRhiGraphicsPipeline> m_colorPipeline;
    std::unique_ptr<QRhiShaderResourceBindings> m_fullscreenQuadSrb;
    std::unique_ptr<QRhiGraphicsPipeline> m_fullscreenQuadPipeline;

    QRhiResourceUpdateBatch *m_initialUpdates = nullptr;

    App app;
};
