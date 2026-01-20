// Part of Friction <https://friction.graphics>
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
    qDBusRegisterMetaType<DBusScene>();
    qDBusRegisterMetaType<QList<DBusScene>>();
#endif

    QCoreApplication app(argc, argv);
    Server server(&app);

    qDebug() << "--- Friction API Server Running ---";
    qDebug() << "Socket:" << FRICTION_API_SOCKET;

    return app.exec();
}

