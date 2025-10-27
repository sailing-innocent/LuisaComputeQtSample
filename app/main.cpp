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
#include "rhiwindow.h"

int main(int argc, char **argv)
{
    // 使用QApplication而不是QGuiApplication以支持widgets
    QApplication app(argc, argv);

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

    // 创建渲染窗口
    // TODO: external device
    HelloWindow *renderWindow = new HelloWindow;
    renderWindow->workspace_path = argv[0]; // set runtime workspace path

    QWidget *renderContainer = QWidget::createWindowContainer(renderWindow, &mainWindow);
    renderContainer->setMinimumSize(800, 600);
    renderContainer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    mainLayout->addWidget(renderContainer);

    // 底部控制面板
    QGroupBox *controlGroup = new QGroupBox("Render Controls");
    QHBoxLayout *controlLayout = new QHBoxLayout(controlGroup);

    // 左侧按钮组
    QVBoxLayout *buttonLayout = new QVBoxLayout();
    QPushButton *startButton = new QPushButton("Start Render");
    QPushButton *stopButton = new QPushButton("Stop Render");
    QPushButton *resetButton = new QPushButton("Reset Scene");
    
    startButton->setEnabled(false); // 示例：默认已经在渲染
    buttonLayout->addWidget(startButton);
    buttonLayout->addWidget(stopButton);
    buttonLayout->addWidget(resetButton);
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

    // 连接信号槽（示例，不实际实现功能）
    QObject::connect(sampleSlider, &QSlider::valueChanged, [sampleValueLabel](int value) {
        sampleValueLabel->setText(QString::number(value));
    });
    
    QObject::connect(bounceSlider, &QSlider::valueChanged, [bounceValueLabel](int value) {
        bounceValueLabel->setText(QString::number(value));
    });

    // 显示主窗口
    mainWindow.show();

    int ret = app.exec();

    // 清理资源
    if (renderWindow->handle())
        renderWindow->releaseSwapChain();

    return ret;
}
