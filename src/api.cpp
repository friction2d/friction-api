// Friction API <https://friction.graphics>
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
    setupDBus();
    setupRemoteObjects();
}

void Server::setupDBus()
{
#ifdef FRICTION_HAS_DBUS
    new Adaptor(this);
    if (QDBusConnection::sessionBus().registerService(FRICTION_API_DBUS_ID)) {
        mDBusConnected = QDBusConnection::sessionBus().registerObject(FRICTION_API_DBUS_PATH, this);
    }
    qWarning() << "D-Bus server status:" << (mDBusConnected ? "Online" : "Offline");
#endif
}

void Server::setupRemoteObjects()
{
    mRemoteHost = new QRemoteObjectHost(QUrl(QStringLiteral(FRICTION_API_SOCKET)), this);
    mHostConnected = mRemoteHost->enableRemoting(this);
    qWarning() << "IPC server status:" << (mHostConnected ? "Online" : "Offline");
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

void Server::setScenes(QList<fScene> scenes)
{
    qWarning() << "setScenes called" << scenes.length();
    Q_EMIT message(tr("setScenes called"));
    FrictionApiSimpleSource::setScenes(scenes);
}

void Server::setCurrentScene(const int id)
{
    qWarning() << "setCurrentScene called" << id;
    Q_EMIT message(tr("setCurrentScene called"));
    FrictionApiSimpleSource::setCurrentScene(id);
}
