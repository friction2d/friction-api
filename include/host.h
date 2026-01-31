// Friction API <https://friction.graphics>
// SPDX-FileCopyrightText: Copyright (c) Ole-André Rodlie and contributors
// SPDX-License-Identifier: BSD-3-Clause

#ifndef FRICTION_API_H
#define FRICTION_API_H

#include "api_global.h"
#include "rep_api_source.h"

#include <QObject>
#include <QString>
#include <QList>
#include <QRemoteObjectHost>
#include <QLocalServer>
#include <QLocalSocket>
#include <QJsonValue>

#ifdef FRICTION_HAS_DBUS
#include <QtDBus/QDBusAbstractAdaptor>
#include <QtDBus/QDBusArgument>
#endif

namespace Friction::Api
{
    class FRICTION_API_EXPORT Host : public FrictionApiSimpleSource
    {
        Q_OBJECT

    public:
        explicit Host(QObject *parent = nullptr,
                      const QString &socketName = FRICTION_API_SOCKET,
                      const QString &socketBridge = FRICTION_API_BRIDGE,
                      const QString &sessionName = FRICTION_API_DBUS_ID,
                      const QString &sessionPath = FRICTION_API_DBUS_PATH);

        bool isDBusConnected() const;
        bool isHostConnected() const;
        bool isBridgeConnected() const;
        bool isConnected() const;

        void setScenes(QList<fScene> scenes) override;
        void setCurrentScene(const int id) override;

    private:
        void setupDBus(const QString &sessionName,
                       const QString &sessionPath);
        void setupRemoteObjects(const QString &socketName);
        void setupBridge(const QString &socketName);

        void handleNewBridgeConnection();
        void processBridgeRequest();
        void handleBridgeDisconnect();
        void broadcastToBridge(const QJsonObject &json);
        void sendBridgeResponse(QLocalSocket *client,
                                const QString &id,
                                const QJsonValue &result);
        void sendBridgeError(QLocalSocket *client,
                             const QString &id,
                             const QString &message);

        QJsonObject serializeScenes(const QList<fScene> &scenes);

        bool mDBusConnected;
        bool mHostConnected;
        bool mBridgeConnected;
        QRemoteObjectHost *mRemoteHost;
        QLocalServer *mBridgeServer;
        QList<QLocalSocket*> mBridgeClients;
    };

#ifdef FRICTION_HAS_DBUS
    struct FRICTION_API_EXPORT DBusRange
    {
        int first;
        int last;

        friend QDBusArgument &operator<<(QDBusArgument &arg,
                                         const DBusRange &range)
        {
            arg.beginStructure();
            arg << range.first << range.last;
            arg.endStructure();
            return arg;
        }

        friend const QDBusArgument &operator>>(const QDBusArgument &arg,
                                               DBusRange &range)
        {
            arg.beginStructure();
            arg >> range.first >> range.last;
            arg.endStructure();
            return arg;
        }
    };

    struct FRICTION_API_EXPORT DBusScene
    {
        int id;
        QString title;
        int width;
        int height;
        double fps;
        DBusRange range;

        friend QDBusArgument &operator<<(QDBusArgument &arg,
                                         const DBusScene &scene)
        {
            arg.beginStructure();
            arg << scene.id
                << scene.title
                << scene.width
                << scene.height
                << scene.fps
                << scene.range;
            arg.endStructure();
            return arg;
        }

        friend const QDBusArgument &operator>>(const QDBusArgument &arg,
                                               DBusScene &scene)
        {
            arg.beginStructure();
            arg >> scene.id
                >> scene.title
                >> scene.width
                >> scene.height
                >> scene.fps
                >> scene.range;
            arg.endStructure();
            return arg;
        }
    };

    class FRICTION_API_EXPORT Adaptor : public QDBusAbstractAdaptor
    {
        Q_OBJECT
        Q_CLASSINFO("D-Bus Interface",
                    FRICTION_API_DBUS_ID)

    public:
        explicit Adaptor(Host *parent);

    public Q_SLOTS:
        QList<Friction::Api::DBusScene> getScenes();
        int getCurrentScene();
        bool setCurrentScene(const int id);

    Q_SIGNALS:
        void message(const QString &text);
        void scenesChanged();
        void currentSceneChanged(const int id);
    };
#endif // FRICTION_HAS_DBUS
}

#ifdef FRICTION_HAS_DBUS
Q_DECLARE_METATYPE(Friction::Api::DBusRange)
Q_DECLARE_METATYPE(Friction::Api::DBusScene)
Q_DECLARE_METATYPE(QList<Friction::Api::DBusScene>)
#endif

#endif // FRICTION_API_H
