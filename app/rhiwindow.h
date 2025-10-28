#pragma once
#include <QWindow>
#include <QOffscreenSurface>
#include <rhi/qrhi.h>
#include "dummyrt.h"

class RhiWindow : public QWindow {
public:
    luisa::compute::Context *context{};
    RhiWindow(QRhi::Implementation graphicsApi);
    QString graphicsApiName() const;
    void releaseSwapChain();
    std::string workspace_path;

protected:
    virtual void customInit(luisa::compute::Context &&ctx) = 0;
    virtual void customRender() = 0;
    std::unique_ptr<QRhi> m_rhi;
    std::unique_ptr<QRhiSwapChain> m_sc;
    std::unique_ptr<QRhiRenderBuffer> m_ds;
    std::unique_ptr<QRhiRenderPassDescriptor> m_rp;
    bool m_hasSwapChain = false;
    QRhi::Implementation m_graphicsApi = QRhi::D3D12;

private:
    void init();
    void resizeSwapChain();
    void render();

    void exposeEvent(QExposeEvent *) override;
    bool event(QEvent *) override;

    bool m_initialized = false;
    bool m_notExposed = false;
    bool m_newlyExposed = false;
};

class HelloWindow : public RhiWindow {
public:
    App app;
    HelloWindow(QRhi::Implementation graphicsApi);
    void customInit(luisa::compute::Context &&ctx) override;
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
};
