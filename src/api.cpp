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
    : FrictionApiSource(parent)
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

void Server::testMethod()
{
    qDebug() << "API: testMethod() called";
    emit message(tr("Hello from Friction API!"));
}

void Server::newProject(const int width,
                        const int height,
                        const double fps,
                        const int start,
                        const int end,
                        const double r,
                        const double g,
                        const double b,
                        const double a)
{
    qDebug() << "API: newProject() called" << width
                                           << height
                                           << fps
                                           << start
                                           << end
                                           << r
                                           << g
                                           << b
                                           << a;
    emit projectCreated(width,
                        height,
                        fps,
                        start,
                        end,
                        r,
                        g,
                        b,
                        a);
}

void Server::loadProject(const QString &path)
{
    qDebug() << "API: loadProject() called" << path;
    emit loadedProject(false, path);
}

bool Server::isDBusConnected() const
{
    return mDBusConnected;
}

bool Server::isHostConnected() const
{
    return mHostConnected;
}

#ifdef FRICTION_HAS_DBUS
Adaptor::Adaptor(Server *parent)
    : QDBusAbstractAdaptor(parent)
{
    connect(parent, SIGNAL(message(QString)),
            this, SIGNAL(message(QString)));

    connect(parent, SIGNAL(projectCreated(int,
                                          int,
                                          double,
                                          int,
                                          int,
                                          double,
                                          double,
                                          double,
                                          double)),
            this, SIGNAL(projectCreated(int,
                                        int,
                                        double,
                                        int,
                                        int,
                                        double,
                                        double,
                                        double,
                                        double)));
}

void Adaptor::testMethod()
{
    static_cast<Server*>(parent())->testMethod();
}

void Adaptor::newProject(const int width,
                         const int height,
                         const double fps,
                         const int start,
                         const int end,
                         const double r,
                         const double g,
                         const double b,
                         const double a)
{
    static_cast<Server*>(parent())->newProject(width,
                                               height,
                                               fps,
                                               start,
                                               end,
                                               r,
                                               g,
                                               b,
                                               a);
}

void Adaptor::loadProject(const QString &path)
{
    static_cast<Server*>(parent())->loadProject(path);
}
#endif
