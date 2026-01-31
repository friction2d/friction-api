// Friction API <https://friction.graphics>
// SPDX-FileCopyrightText: Copyright (c) Ole-André Rodlie and contributors
// SPDX-License-Identifier: BSD-3-Clause

#include "host.h"

#include <QDebug>

#ifdef FRICTION_HAS_DBUS
#include "adaptor.h"
#include <QtDBus/QDBusConnection>
#endif

using namespace Friction::Api;

Host::Host(QObject *parent,
           const QString &socketName,
           const QString &sessionName,
           const QString &sessionPath)
    : FrictionApiSimpleSource(parent)
    , mDBusConnected(false)
    , mHostConnected(false)
    , mRemoteHost(nullptr)
{
    qWarning() << "--- Friction Host Running ---";
    setupDBus(sessionName, sessionPath);
    setupRemoteObjects(socketName);
}

void Host::setupDBus(const QString &sessionName,
                     const QString &sessionPath)
{
#ifdef FRICTION_HAS_DBUS
    new Adaptor(this);
    if (QDBusConnection::sessionBus().registerService(sessionName)) {
        mDBusConnected = QDBusConnection::sessionBus().registerObject(sessionPath, this);
    }
    qWarning() << "Host: D-Bus server is"
               << (mDBusConnected ? "Online" : "Offline")
               << sessionName << sessionPath;
#endif
}

void Host::setupRemoteObjects(const QString &socketName)
{
    mRemoteHost = new QRemoteObjectHost(QUrl(socketName), this);
    mHostConnected = mRemoteHost->enableRemoting(this);
    qWarning() << "Host: IPC server is"
               << (mHostConnected ? "Online" : "Offline")
               << socketName;
}

bool Host::isDBusConnected() const
{
    return mDBusConnected;
}

bool Host::isHostConnected() const
{
    return mHostConnected;
}

bool Host::isConnected() const
{
    return mDBusConnected || mHostConnected;
}

void Host::setScenes(QList<fScene> scenes)
{
    qWarning() << "Host: setScenes called" << scenes.length();
    Q_EMIT message(tr("setScenes called in Host"));
    FrictionApiSimpleSource::setScenes(scenes);
}

void Host::setCurrentScene(const int id)
{
    qWarning() << "Host: setCurrentScene called" << id;
    Q_EMIT message(tr("setCurrentScene called in Host"));
    FrictionApiSimpleSource::setCurrentScene(id);
}
