// Part of Friction <https://friction.graphics>
// SPDX-FileCopyrightText: Copyright (c) Ole-André Rodlie and contributors
// SPDX-License-Identifier: BSD-3-Clause

#include "api.h"

#include <QDebug>

#ifdef FRICTION_HAS_DBUS
#include "api_dbus.h"
#include <QtDBus/QDBusConnection>
Friction::Api::ApiAdaptor::ApiAdaptor(ApiServer *parent)
    : QDBusAbstractAdaptor(parent)
{
    connect(parent, SIGNAL(message(QString)),
            this, SIGNAL(message(QString)));
}

void Friction::Api::ApiAdaptor::testMethod()
{
    static_cast<ApiServer*>(parent())->testMethod();
}
#endif

using namespace Friction::Api;

ApiServer::ApiServer(QObject *parent)
    : FrictionApiSource(parent)
    , mDBusConnected(false)
    , mHostConnected(false)
    , mRemoteHost(nullptr)
{
    setupDBus();
    setupRemoteObjects();
}

void ApiServer::setupDBus()
{
#ifdef FRICTION_HAS_DBUS
    new ApiAdaptor(this);
    if (QDBusConnection::sessionBus().registerService(FRICTION_API_ID)) {
        mDBusConnected = QDBusConnection::sessionBus().registerObject(FRICTION_API_DBUS_PATH, this);
    }
    qDebug() << "API: D-Bus server status:" << (mDBusConnected ? "Online" : "Offline");
#endif
}

void ApiServer::setupRemoteObjects()
{
    mRemoteHost = new QRemoteObjectHost(QUrl(QStringLiteral(FRICTION_API_SOCKET)), this);
    mHostConnected = mRemoteHost->enableRemoting(this);
    qDebug() << "API: RemoteObjects server status:" << (mHostConnected ? "Online" : "Offline");
}

void ApiServer::testMethod()
{
    qDebug() << "API: testMethod() called";
    emit message(tr("Hello from Friction API!"));
}

bool ApiServer::isDBusConnected() const
{
    return mDBusConnected;
}

bool ApiServer::isHostConnected() const
{
    return mHostConnected;
}
