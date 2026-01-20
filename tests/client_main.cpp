// Part of Friction <https://friction.graphics>
// SPDX-FileCopyrightText: Copyright (c) Ole-André Rodlie and contributors
// SPDX-License-Identifier: BSD-3-Clause

#include "rep_api_replica.h"

#include <QCoreApplication>
#include <QRemoteObjectNode>
#include <QDebug>

int main(int argc, char *argv[]) {
    QCoreApplication app(argc, argv);
    QRemoteObjectNode repNode;
    repNode.connectToNode(QUrl(QStringLiteral("local:friction_api")));

    auto ptr = repNode.acquire<FrictionApiReplica>();
    QObject::connect(ptr, &FrictionApiReplica::initialized,
                     [ptr, &app]() {
        qWarning() << "--- Connected to Friction Server ---";

        const QList<Scene> list = ptr->scenes();

        qWarning() << "Total Scenes:" << list.size();
        for (const auto &s : list) {
            qWarning() << " -> ID:" << s.id() << "Title:" << s.title();
        }
        app.quit();
    });

    return app.exec();
}
