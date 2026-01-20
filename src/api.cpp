// Part of Friction <https://friction.graphics>
// SPDX-FileCopyrightText: Copyright (c) Ole-André Rodlie and contributors
// SPDX-License-Identifier: BSD-3-Clause

#include "api.h"

#include <QDebug>

#ifdef FRICTION_HAS_DBUS
#include "api_dbus.h"
#include <QtDBus/QDBusConnection>
#endif

using namespace Friction::Api;

Server::Server(QObject *parent)
    : FrictionApiSimpleSource(parent)
    , mDBusConnected(false)
    , mHostConnected(false)
    , mRemoteHost(nullptr)
{
    // add dummy data
    QList<Scene> initialScenes;
    initialScenes << Scene(0, "Scene 0")
                  << Scene(1, "Scene 1")
                  << Scene(2, "Scene 2");
    FrictionApiSimpleSource::setScenes(initialScenes);
    FrictionApiSimpleSource::setCurrentScene(0);

    setupDBus();
    setupRemoteObjects();
}

void Server::setupDBus()
{
#ifdef FRICTION_HAS_DBUS
    new Adaptor(this);
    if (QDBusConnection::sessionBus().registerService(FRICTION_API_ID)) {
        mDBusConnected = QDBusConnection::sessionBus().registerObject(FRICTION_API_DBUS_PATH, this);
    }
    qDebug() << "API: D-Bus server status:" << (mDBusConnected ? "Online" : "Offline");
#endif
}

void Server::setupRemoteObjects()
{
    mRemoteHost = new QRemoteObjectHost(QUrl(QStringLiteral(FRICTION_API_SOCKET)), this);
    mHostConnected = mRemoteHost->enableRemoting(this);
    qDebug() << "API: IPC server status:" << (mHostConnected ? "Online" : "Offline");
}

bool Server::isDBusConnected() const
{
    return mDBusConnected;
}

bool Server::isHostConnected() const
{
    return mHostConnected;
}

bool Server::isConnected() const
{
    return mDBusConnected || mHostConnected;
}

void Server::setScenes(const QList<Scene> &scenes)
{
    FrictionApiSimpleSource::setScenes(scenes);
}

void Server::setCurrentScene(const int id)
{
    FrictionApiSimpleSource::setCurrentScene(id);
}
