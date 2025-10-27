#include "rhiwindow.h"
#include <QPlatformSurfaceEvent>
#include <QPainter>
#include <QFile>
#include <rhi/qshader.h>

RhiWindow::RhiWindow()
{
    setSurfaceType(Direct3DSurface);
}

QString RhiWindow::graphicsApiName() const
{
    return QLatin1String("Direct3D 12");
}

void RhiWindow::exposeEvent(QExposeEvent *)
{
    // initialize and start rendering when the window becomes usable for graphics purposes
    if (isExposed() && !m_initialized) {
        init();
        resizeSwapChain();
        m_initialized = true;
    }

    const QSize surfaceSize = m_hasSwapChain ? m_sc->surfacePixelSize() : QSize();

    // stop pushing frames when not exposed (or size is 0)
    if ((!isExposed() || (m_hasSwapChain && surfaceSize.isEmpty())) && m_initialized && !m_notExposed)
        m_notExposed = true;

    // Continue when exposed again and the surface has a valid size. Note that
    // surfaceSize can be (0, 0) even though size() reports a valid one, hence
    // trusting surfacePixelSize() and not QWindow.
    if (isExposed() && m_initialized && m_notExposed && !surfaceSize.isEmpty()) {
        m_notExposed = false;
        m_newlyExposed = true;
    }

    // always render a frame on exposeEvent() (when exposed) in order to update
    // immediately on window resize.
    if (isExposed() && !surfaceSize.isEmpty())
        render();
}
bool RhiWindow::event(QEvent *e)
{
    switch (e->type()) {
    case QEvent::UpdateRequest:
        render();
        break;

    case QEvent::PlatformSurface:
        // this is the proper time to tear down the swapchain (while the native window and surface are still around)
        if (static_cast<QPlatformSurfaceEvent *>(e)->surfaceEventType() == QPlatformSurfaceEvent::SurfaceAboutToBeDestroyed)
            releaseSwapChain();
        break;

    default:
        break;
    }

    return QWindow::event(e);
}
void RhiWindow::init()
{
    QRhiD3D12InitParams params;
    params.enableDebugLayer = true;
    m_rhi.reset(QRhi::create(QRhi::D3D12, &params));
    if (!m_rhi)
        qFatal("Failed to create RHI backend");

    m_sc.reset(m_rhi->newSwapChain());
    m_ds.reset(m_rhi->newRenderBuffer(QRhiRenderBuffer::DepthStencil,
                                      QSize(), // no need to set the size here, due to UsedWithSwapChainOnly
                                      1,
                                      QRhiRenderBuffer::UsedWithSwapChainOnly));
    m_sc->setWindow(this);
    m_sc->setDepthStencil(m_ds.get());
    m_rp.reset(m_sc->newCompatibleRenderPassDescriptor());
    m_sc->setRenderPassDescriptor(m_rp.get());
    customInit();
}

void RhiWindow::resizeSwapChain()
{
    m_hasSwapChain = m_sc->createOrResize(); // also handles m_ds
}
void RhiWindow::releaseSwapChain()
{
    if (m_hasSwapChain) {
        m_hasSwapChain = false;
        m_sc->destroy();
    }
}
void RhiWindow::render()
{
    if (!m_hasSwapChain || m_notExposed)
        return;
    if (m_sc->currentPixelSize() != m_sc->surfacePixelSize() || m_newlyExposed) {
        resizeSwapChain();
        if (!m_hasSwapChain)
            return;
        m_newlyExposed = false;
    }

    QRhi::FrameOpResult result = m_rhi->beginFrame(m_sc.get());

    if (result == QRhi::FrameOpSwapChainOutOfDate) {
        resizeSwapChain();
        if (!m_hasSwapChain)
            return;
        result = m_rhi->beginFrame(m_sc.get());
    }
    if (result != QRhi::FrameOpSuccess) {
        qWarning("beginFrame failed with %d, will retry", result);
        requestUpdate();
        return;
    }

    customRender();

    m_rhi->endFrame(m_sc.get());
    requestUpdate();
}

static QShader getShader(const QString &name)
{
    QFile f(name);
    if (f.open(QIODevice::ReadOnly))
        return QShader::fromSerialized(f.readAll());

    return QShader();
}

HelloWindow::HelloWindow(const char* ws, void* rhi_device, void* rhi_instance /*/only for vulkan*/, void* rhi_physical_device /*only for vulkan*/)
{
    app.init(ws, rhi_device, rhi_instance, rhi_physical_device);
}

void HelloWindow::ensureFullscreenTexture(const QSize &pixelSize, QRhiResourceUpdateBatch *u)
{
    if (m_texture && m_texture->pixelSize() == pixelSize)
        return;

    if (!m_texture)
        m_texture.reset(m_rhi->newTexture(QRhiTexture::RGBA8, pixelSize));
    else
        m_texture->setPixelSize(pixelSize);

    uint64_t handle = app.create_texture(pixelSize.width(), pixelSize.height());
    m_texture->createFrom({handle, 0});


    // m_texture->create();
    // uint64_t handle = m_texture->nativeTexture().object;
    // uint width = m_texture->pixelSize().width();
    // uint height = m_texture->pixelSize().height();
    // app.update(handle, width, height);

    // QImage image(pixelSize, QImage::Format_RGBA8888_Premultiplied);
    // QPainter painter(&image);
    // painter.fillRect(QRectF(QPointF(0, 0), pixelSize), QColor::fromRgbF(0.4f, 0.7f, 0.0f, 1.0f));
    // painter.setPen(Qt::transparent);
    // painter.setBrush({ QGradient(QGradient::DeepBlue) });
    // painter.drawRoundedRect(QRectF(QPointF(20, 20), pixelSize - QSize(40, 40)), 16, 16);
    // painter.setPen(Qt::black);
    // QFont font;
    // font.setPixelSize(0.05 * qMin(pixelSize.width(), pixelSize.height()));
    // painter.setFont(font);
    // painter.drawText(QRectF(QPointF(60, 60), pixelSize - QSize(120, 120)), 0,
    //                  QLatin1String("Rendering with QRhi to a resizable QWindow.\nThe 3D API is %1.\nUse the command-line options to choose a different API.")
    //                  .arg(graphicsApiName()));
    // painter.end();

    // if (m_rhi->isYUpInNDC())
    //     image = image.mirrored();
    // u->uploadTexture(m_texture.get(), image);

}

void HelloWindow::customInit()
{
    m_initialUpdates = m_rhi->nextResourceUpdateBatch();

    ensureFullscreenTexture(m_sc->surfacePixelSize(), m_initialUpdates);

    m_sampler.reset(m_rhi->newSampler(QRhiSampler::Linear, QRhiSampler::Linear, QRhiSampler::None,
                                      QRhiSampler::ClampToEdge, QRhiSampler::ClampToEdge));
    m_sampler->create();

    m_fullscreenQuadSrb.reset(m_rhi->newShaderResourceBindings());
    m_fullscreenQuadSrb->setBindings({
            QRhiShaderResourceBinding::sampledTexture(0, QRhiShaderResourceBinding::FragmentStage,
                                       m_texture.get(), m_sampler.get())
    });
    m_fullscreenQuadSrb->create();

    m_fullscreenQuadPipeline.reset(m_rhi->newGraphicsPipeline());
    m_fullscreenQuadPipeline->setShaderStages({
        { QRhiShaderStage::Vertex, getShader(QLatin1String(":/quad.vert.qsb")) },
        { QRhiShaderStage::Fragment, getShader(QLatin1String(":/quad.frag.qsb")) }
    });
    m_fullscreenQuadPipeline->setVertexInputLayout({});
    m_fullscreenQuadPipeline->setShaderResourceBindings(m_fullscreenQuadSrb.get());
    m_fullscreenQuadPipeline->setRenderPassDescriptor(m_rp.get());
    m_fullscreenQuadPipeline->create();
}

void HelloWindow::customRender()
{
    QRhiResourceUpdateBatch *resourceUpdates = m_rhi->nextResourceUpdateBatch();

    if (m_initialUpdates) {
        resourceUpdates->merge(m_initialUpdates);
        m_initialUpdates->release();
        m_initialUpdates = nullptr;
    }

    QRhiCommandBuffer *cb = m_sc->currentFrameCommandBuffer();
    const QSize outputSizeInPixels = m_sc->currentPixelSize();
    ensureFullscreenTexture(outputSizeInPixels, resourceUpdates);

    app.update();

    cb->beginPass(m_sc->currentFrameRenderTarget(), Qt::black, { 1.0f, 0 }, resourceUpdates);
    cb->setGraphicsPipeline(m_fullscreenQuadPipeline.get());
    cb->setViewport({ 0, 0, float(outputSizeInPixels.width()), float(outputSizeInPixels.height()) });
    cb->setShaderResources();
    cb->draw(3);
    cb->endPass();
}
