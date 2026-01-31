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
    QRemoteObjectNode node;
    node.connectToNode(QUrl(QStringLiteral(FRICTION_API_SOCKET)));
    auto replica = node.acquire<FrictionApiReplica>();
    QObject::connect(replica, &FrictionApiReplica::initialized,
                     [replica, &app]() {
        qWarning() << "--- Connected to Friction Host ---";

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

        /*
        QObject::connect(replica, &FrictionApiReplica::scenesChanged,
                         [replica](QList<fScene> scenes){
            qDebug() << "Scenes Changed:";
            for (const auto &s : scenes) {
                qWarning() << "--> ID:" << s.id()
                << "Title:" << s.title()
                << "Width:" << s.width()
                << "Height:" << s.height()
                << "Fps:" << s.fps()
                << "Start Frame:" << s.range().first()
                << "End Frame:" << s.range().last();
            }
        });
        QList<fScene> initialScenes;
        initialScenes << fScene(01, "Scene 0", 1920, 1080, 30, fRange{0, 299})
                      << fScene(122, "Scene 1", 1920, 1080, 30, fRange{0, 299})
                      << fScene(2, "Scene 2", 1920, 1080, 30, fRange{0, 299});
        replica->pushScenes(initialScenes);
        */

        QTimer::singleShot(1000, &app,
                           &QCoreApplication::quit);
    });
    return app.exec();
}
