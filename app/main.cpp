// Copyright (C) 2023 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR BSD-3-Clause

#include <QApplication>
#include <QCommandLineParser>
#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QSlider>
#include <QGroupBox>
#include <QKeyEvent>
#include <QMouseEvent>
#include <QDebug>
#include "qlogging.h"
#include "rhi/qrhi.h"
#include "rhiwindow.h"

// 自定义容器Widget，用于转发键盘和鼠标事件到嵌入的RhiWindow
class WindowContainerWidget : public QWidget {
public:
    RhiWindow *rhiWindow = nullptr;
    
    WindowContainerWidget(RhiWindow *window, QWidget *parent = nullptr)
        : QWidget(parent), rhiWindow(window) {
        setFocusPolicy(Qt::StrongFocus);
        setMouseTracking(true);
    }

protected:
    void keyPressEvent(QKeyEvent *event) override {
        qInfo() << "WindowContainerWidget::keyPressEvent - forwarding to RhiWindow";
        if (rhiWindow) {
            QCoreApplication::sendEvent(rhiWindow, event);
        }
        QWidget::keyPressEvent(event);
    }

    void keyReleaseEvent(QKeyEvent *event) override {
        if (rhiWindow) {
            QCoreApplication::sendEvent(rhiWindow, event);
        }
        QWidget::keyReleaseEvent(event);
    }

    void mousePressEvent(QMouseEvent *event) override {
        qInfo() << "WindowContainerWidget::mousePressEvent - setting focus and forwarding";
        setFocus();  // 点击时获取焦点
        if (rhiWindow) {
            QCoreApplication::sendEvent(rhiWindow, event);
        }
        QWidget::mousePressEvent(event);
    }

    void mouseReleaseEvent(QMouseEvent *event) override {
        if (rhiWindow) {
            QCoreApplication::sendEvent(rhiWindow, event);
        }
        QWidget::mouseReleaseEvent(event);
    }

    void mouseMoveEvent(QMouseEvent *event) override {
        if (rhiWindow) {
            QCoreApplication::sendEvent(rhiWindow, event);
        }
        QWidget::mouseMoveEvent(event);
    }

    void wheelEvent(QWheelEvent *event) override {
        if (rhiWindow) {
            QCoreApplication::sendEvent(rhiWindow, event);
        }
        QWidget::wheelEvent(event);
    }
};

struct RendererImpl : public IRenderer {
    QRhi::Implementation backend;
    App *render_app;
    bool is_paused = false;
    void init(QRhiNativeHandles &handles_base) override {
        if (backend == QRhi::D3D12) {
            auto &handles = static_cast<QRhiD3D12NativeHandles &>(handles_base);
            handles.dev = render_app->device.native_handle();
            handles.minimumFeatureLevel = 0;
            handles.adapterLuidHigh = render_app->dx_adaptor_luid.x;
            handles.adapterLuidLow = render_app->dx_adaptor_luid.y;
            handles.commandQueue = render_app->stream.native_handle();
        } else if (backend == QRhi::Vulkan) {
            auto &handles = static_cast<QRhiVulkanNativeHandles &>(handles_base);
            handles.physDev = (VkPhysicalDevice)render_app->vk_physical_device;
            handles.dev = (VkDevice)render_app->device.native_handle();
            handles.gfxQueue = (VkQueue)render_app->stream.native_handle();
        } else {
            LUISA_ERROR("Backend unsupported.");
        }
    }
    void update() override {
        if (is_paused) return;
        render_app->update();
    }
    void pause() override { is_paused = true; }
    void resume() override { is_paused = false; }
    uint64_t get_present_texture(luisa::uint2 resolution) override {
        return render_app->create_texture(resolution.x, resolution.y);
    }
};

int main(int argc, char **argv) {
    // 使用QApplication而不是QGuiApplication以支持widgets
    if (argc < 2) {
        qInfo("use 'rhi_window_sample dx/vk/metal' to select backend");
    }
    luisa::compute::Context ctx{argv[0]};
    App render_app;
    luisa::string backend = argv[1];
    QApplication app(argc, argv);
    QRhi::Implementation graphicsApi;
    if (backend == "dx") {
        graphicsApi = QRhi::D3D12;
    } else if (backend == "vk") {
        graphicsApi = QRhi::Vulkan;
    } else if (backend == "metal") {
        graphicsApi = QRhi::Metal;
    } else {
        qInfo("Invalid backend, choose between dx/vk");
    }

#if QT_CONFIG(vulkan)
    QVulkanInstance inst;
    if (graphicsApi == QRhi::Vulkan) {
        inst.setVkInstance(static_cast<VkInstance>(render_app.init_vulkan(ctx)));
        if (!inst.create()) {
            LUISA_ERROR("Vulkan init failed.");
        }
    }
#endif
    // Init renderer
    render_app.init(ctx, backend.c_str());
    // 创建主窗口容器
    QWidget mainWindow;
    mainWindow.setWindowTitle("LuisaCompute Qt Sample");
    mainWindow.resize(1280, 800);

    // 创建主布局
    QVBoxLayout *mainLayout = new QVBoxLayout(&mainWindow);
    mainLayout->setSpacing(10);
    mainLayout->setContentsMargins(10, 10, 10, 10);

    // 顶部信息栏
    QHBoxLayout *topBarLayout = new QHBoxLayout();
    QLabel *titleLabel = new QLabel("LuisaCompute Real-time Renderer");
    QFont titleFont = titleLabel->font();
    titleFont.setPointSize(12);
    titleFont.setBold(true);
    titleLabel->setFont(titleFont);
    topBarLayout->addWidget(titleLabel);
    topBarLayout->addStretch();

    QLabel *statusLabel = new QLabel("Status: Running");
    statusLabel->setStyleSheet("QLabel { color: green; }");
    topBarLayout->addWidget(statusLabel);

    mainLayout->addLayout(topBarLayout);

    // 添加输入事件调试信息显示
    QLabel *inputDebugLabel = new QLabel("Input Events: Click on the render window and press keys (W/A/S/D)");
    inputDebugLabel->setStyleSheet("QLabel { background-color: #f0f0f0; padding: 5px; border: 1px solid #ccc; }");
    inputDebugLabel->setWordWrap(true);
    mainLayout->addWidget(inputDebugLabel);

    // ============ 核心模块：创建LC-Driven Viewport ===============
    RhiWindow *renderWindow = new RhiWindow(graphicsApi);
    RendererImpl renderer_impl;
    renderer_impl.backend = graphicsApi;
    renderer_impl.render_app = &render_app;
    renderWindow->renderer = &renderer_impl;
    renderWindow->workspace_path = luisa::to_string(luisa::filesystem::path(argv[0]).parent_path());// set runtime workspace path

#if QT_CONFIG(vulkan)
    if (graphicsApi == QRhi::Vulkan)
        renderWindow->setVulkanInstance(&inst);
#endif

    // ============ 核心模块：创建LC-Driven Viewport ===============
    // 创建自定义容器Widget来转发事件
    WindowContainerWidget *renderContainerWrapper = new WindowContainerWidget(renderWindow, &mainWindow);
    renderContainerWrapper->setMinimumSize(800, 600);
    renderContainerWrapper->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    
    // 创建布局并将RhiWindow的容器嵌入其中
    QVBoxLayout *containerLayout = new QVBoxLayout(renderContainerWrapper);
    containerLayout->setContentsMargins(0, 0, 0, 0);
    
    QWidget *renderContainer = QWidget::createWindowContainer(renderWindow, renderContainerWrapper);
    renderContainer->setFocusPolicy(Qt::NoFocus); // 容器本身不接收焦点
    containerLayout->addWidget(renderContainer);
    
    // 设置初始焦点
    renderContainerWrapper->setFocus();
    // ============ 核心模块：创建LC-Driven Viewport ===============
    mainLayout->addWidget(renderContainerWrapper);

    // 底部控制面板
    QGroupBox *controlGroup = new QGroupBox("Render Controls");
    QHBoxLayout *controlLayout = new QHBoxLayout(controlGroup);

    // 左侧按钮组
    QVBoxLayout *buttonLayout = new QVBoxLayout();
    QPushButton *startButton = new QPushButton("Start Render");
    startButton->setEnabled(false);// 示例：默认已经在渲染
    QPushButton *pauseButton = new QPushButton("Pause Render");
    QObject::connect(pauseButton, &QPushButton::pressed, [renderWindow]() {
        renderWindow->renderer->pause();
    });
    QPushButton *resumeButton = new QPushButton("Resume Render");
    QObject::connect(resumeButton, &QPushButton::pressed, [renderWindow]() {
        renderWindow->renderer->resume();
    });
    buttonLayout->addWidget(startButton);
    buttonLayout->addWidget(pauseButton);
    buttonLayout->addWidget(resumeButton);
    buttonLayout->addStretch();

    controlLayout->addLayout(buttonLayout);

    // 中间参数控制
    QVBoxLayout *paramLayout = new QVBoxLayout();

    QHBoxLayout *sampleLayout = new QHBoxLayout();
    QLabel *sampleLabel = new QLabel("Samples:");
    QSlider *sampleSlider = new QSlider(Qt::Horizontal);
    sampleSlider->setRange(1, 100);
    sampleSlider->setValue(50);
    QLabel *sampleValueLabel = new QLabel("50");
    sampleLayout->addWidget(sampleLabel);
    sampleLayout->addWidget(sampleSlider);
    sampleLayout->addWidget(sampleValueLabel);

    QHBoxLayout *bounceLayout = new QHBoxLayout();
    QLabel *bounceLabel = new QLabel("Max Bounce:");
    QSlider *bounceSlider = new QSlider(Qt::Horizontal);
    bounceSlider->setRange(1, 10);
    bounceSlider->setValue(5);
    QLabel *bounceValueLabel = new QLabel("5");
    bounceLayout->addWidget(bounceLabel);
    bounceLayout->addWidget(bounceSlider);
    bounceLayout->addWidget(bounceValueLabel);

    paramLayout->addLayout(sampleLayout);
    paramLayout->addLayout(bounceLayout);

    controlLayout->addLayout(paramLayout);

    // 右侧统计信息
    QVBoxLayout *statsLayout = new QVBoxLayout();
    QLabel *fpsLabel = new QLabel("FPS: 60");
    QLabel *frameTimeLabel = new QLabel("Frame Time: 16.6ms");
    QLabel *resolutionLabel = new QLabel("Resolution: 1280x720");
    statsLayout->addWidget(fpsLabel);
    statsLayout->addWidget(frameTimeLabel);
    statsLayout->addWidget(resolutionLabel);
    statsLayout->addStretch();

    controlLayout->addLayout(statsLayout);

    mainLayout->addWidget(controlGroup);

    // 底部状态栏
    QHBoxLayout *bottomLayout = new QHBoxLayout();
    QLabel *apiLabel = new QLabel(QString("API: %1").arg(renderWindow->graphicsApiName()));
    QLabel *deviceLabel = new QLabel("Device: GPU 0");
    bottomLayout->addWidget(apiLabel);
    bottomLayout->addStretch();
    bottomLayout->addWidget(deviceLabel);

    mainLayout->addLayout(bottomLayout);

    // 连接Slider和Label的信号槽
    QObject::connect(sampleSlider, &QSlider::valueChanged, [sampleValueLabel](int value) {
        sampleValueLabel->setText(QString::number(value));
    });

    QObject::connect(bounceSlider, &QSlider::valueChanged, [bounceValueLabel](int value) {
        bounceValueLabel->setText(QString::number(value));
    });

    // 连接RhiWindow的输入事件信号到UI标签
    QObject::connect(renderWindow, &RhiWindow::keyPressed, [inputDebugLabel](const QString &keyInfo) {
        inputDebugLabel->setText(QString("Input Events: %1").arg(keyInfo));
        inputDebugLabel->setStyleSheet("QLabel { background-color: #d4edda; padding: 5px; border: 1px solid #28a745; color: #155724; }");
    });

    QObject::connect(renderWindow, &RhiWindow::mouseClicked, [inputDebugLabel](const QString &mouseInfo) {
        inputDebugLabel->setText(QString("Input Events: %1").arg(mouseInfo));
        inputDebugLabel->setStyleSheet("QLabel { background-color: #d1ecf1; padding: 5px; border: 1px solid #17a2b8; color: #0c5460; }");
    });

    // 显示主窗口
    mainWindow.show();

    int ret = app.exec();
    // Move stream here to make it destroy later than QT
    // 清理资源
    if (renderWindow->handle())
        renderWindow->releaseSwapChain();
    return ret;
}
