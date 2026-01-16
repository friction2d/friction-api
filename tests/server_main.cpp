// Part of Friction <https://friction.graphics>
// SPDX-FileCopyrightText: Copyright (c) Ole-André Rodlie and contributors
// SPDX-License-Identifier: BSD-3-Clause

#include "api.h"
#include "plugins.h"

#include <QApplication>
#include <QMainWindow>
#include <QDir>

using namespace Friction::Api;

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    Server server(&app);

    qDebug() << "--- Friction API Server Running ---";
    qDebug() << "Socket:" << FRICTION_API_SOCKET;

    QMainWindow win;
    Plugins manager;
    QString path = QApplication::applicationDirPath() + "/plugins";

    if (!QDir(path).exists()) {
#ifdef FRICTION_PLUGIN_DIR
        path = FRICTION_PLUGIN_DIR;
#endif
    }

    manager.scanAndLoad(path, &win);
    if (!manager.activePlugins().isEmpty()) {
        win.setWindowTitle("Friction Plugin Test");
        auto* p = manager.activePlugins().first();
        QWidget* pluginWidget = p->createWidget(&win);
        win.setCentralWidget(pluginWidget);
        win.show();
    }

    return app.exec();
}

