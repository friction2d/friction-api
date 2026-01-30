// Friction API <https://friction.graphics>
// SPDX-FileCopyrightText: Copyright (c) Ole-André Rodlie and contributors
// SPDX-License-Identifier: BSD-3-Clause

// the client only need the rep file
// generated from qt6_re_generate_replica or qt5_generate_repc in CMakeLists.txt
// see include/api.rep
#include "rep_api_replica.h"

#include <QCoreApplication>
#include <QRemoteObjectNode>
#include <QDebug>
#include <QTimer>

int main(int argc, char *argv[]) {
    QCoreApplication app(argc, argv);
    QRemoteObjectNode socket;
    socket.connectToNode(QUrl(QStringLiteral("local:graphics.friction.api")));
    auto replica = socket.acquire<FrictionApiReplica>();
    QObject::connect(replica, &FrictionApiReplica::initialized,
                     [replica, &app]() {
        qWarning() << "--- Connected to Friction Server ---";

        qWarning() << "Scenes:";
        for (const auto &s : replica->scenes()) {
            qWarning() << "--> ID:" << s.id()
                       << "Title:" << s.title()
                       << "Width:" << s.width()
                       << "Height:" << s.height()
                       << "Fps:" << s.fps()
                       << "Start Frame:" << s.range().first()
                       << "End Frame:" << s.range().last();
        }

        QTimer::singleShot(1000, &app,
                           &QCoreApplication::quit);
    });
    return app.exec();
}
