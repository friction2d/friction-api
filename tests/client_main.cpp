// Part of Friction <https://friction.graphics>
// SPDX-FileCopyrightText: Copyright (c) Ole-André Rodlie and contributors
// SPDX-License-Identifier: BSD-3-Clause

#include "rep_api_replica.h"

#include <QCoreApplication>
#include <QRemoteObjectNode>
#include <QDebug>

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);

    QRemoteObjectNode repNode;
    repNode.connectToNode(QUrl(QStringLiteral("local:friction_api")));

    FrictionApiReplica *ptr = repNode.acquire<FrictionApiReplica>();

    QObject::connect(ptr,
                     &FrictionApiReplica::message,
                     [](const QString &text) {
        qWarning() << text;
        //QCoreApplication::quit();
    });
    QObject::connect(ptr,
                     &FrictionApiReplica::projectCreated,
                     [](const int width,
                        const int height,
                        const double fps,
                        const int start,
                        const int end,
                        const double r,
                        const double g,
                        const double b,
                        const double a) {
        qWarning() << "Created new project" << width << height << fps << start << end << r << g << b << a;
        //QCoreApplication::quit();
    });
    QObject::connect(ptr,
                     &FrictionApiReplica::loadedProject,
                     [](const bool success,
                        const QString &path) {
        qWarning() << "Loaded project" << success << path;
        //QCoreApplication::quit();
    });

    QObject::connect(ptr,
                     &FrictionApiReplica::initialized,
                     [ptr]() {
        ptr->testMethod();
        ptr->newProject(1920, 1080, 30, 0, 299, 1., 0., 0., 1.);
        ptr->loadProject("/some/path/to/project/file.friction");
    });

    return app.exec();
}
