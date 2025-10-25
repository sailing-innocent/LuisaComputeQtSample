// Copyright (C) 2023 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR BSD-3-Clause

#include <QGuiApplication>
#include <QCommandLineParser>
#include "rhiwindow.h"

int main(int argc, char **argv)
{
    QGuiApplication app(argc, argv);
    HelloWindow window;
    window.resize(1280, 720);
    window.setTitle(QCoreApplication::applicationName() + QLatin1String(" - ") + window.graphicsApiName());
    window.show();

    int ret = app.exec();

    // RhiWindow::event() will not get invoked when the
    // PlatformSurfaceAboutToBeDestroyed event is sent during the QWindow
    // destruction. That happens only when exiting via app::quit() instead of
    // the more common QWindow::close(). Take care of it: if the QPlatformWindow
    // is still around (there was no close() yet), get rid of the swapchain
    // while it's not too late.
    if (window.handle())
        window.releaseSwapChain();

    return ret;
}
