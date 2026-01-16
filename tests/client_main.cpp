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
        QCoreApplication::quit();
    });

    QObject::connect(ptr,
                     &FrictionApiReplica::initialized,
                     [ptr]() {
        ptr->testMethod();
    });

    return app.exec();
}
