// Friction API <https://friction.graphics>
// SPDX-FileCopyrightText: Copyright (c) Ole-André Rodlie and contributors
// SPDX-License-Identifier: BSD-3-Clause

#include "api.h"

#include <QCoreApplication>

#ifdef FRICTION_HAS_DBUS
#include "api_dbus.h"
#include <QtDBus>
#endif

using namespace Friction::Api;

int main(int argc, char *argv[])
{
#ifdef FRICTION_HAS_DBUS
    qDBusRegisterMetaType<DBusRange>();
    qDBusRegisterMetaType<DBusScene>();
    qDBusRegisterMetaType<QList<DBusScene>>();
#endif

    QCoreApplication app(argc, argv);

    qDebug() << "--- Friction Server Running ---";
    qDebug() << "Socket:" << FRICTION_API_SOCKET;

    Server server(&app);

    qDebug() << "Adding dummy data";
    QList<fScene> initialScenes;
    initialScenes << fScene(0, "Scene 0", 1920, 1080, 30, fRange{0, 299})
                  << fScene(1, "Scene 1", 1920, 1080, 30, fRange{0, 299})
                  << fScene(2, "Scene 2", 1920, 1080, 30, fRange{0, 299});
    server.setScenes(initialScenes);
    server.setCurrentScene(0);

    return app.exec();
}

