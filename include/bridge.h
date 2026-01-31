// Friction API <https://friction.graphics>
// SPDX-FileCopyrightText: Copyright (c) Ole-André Rodlie and contributors
// SPDX-License-Identifier: BSD-3-Clause

#ifndef FRICTION_API_BRIDGE_H
#define FRICTION_API_BRIDGE_H

#include <QObject>
#include <QLocalServer>
#include <QLocalSocket>
#include <QRemoteObjectNode>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QPointer>
#include <QList>

#include "rep_api_replica.h"

namespace Friction::Api
{
    class Bridge : public QObject
    {
        Q_OBJECT

    public:
        explicit Bridge(QObject *parent = nullptr);
        ~Bridge();

        bool start(const QString &socketName = FRICTION_API_BRIDGE);

    private slots:
        void handleNewConnection();
        void handleClientDisconnect();

        void handleReplicaStateChanged(QRemoteObjectReplica::State state);
        void processIncomingData();

    private:
        void processClientRequest(QLocalSocket *client,
                                  const QJsonObject &req);

        QJsonObject serializeScenes(QList<fScene> scenes);

        void broadcast(const QJsonObject &json);
        void sendResponse(QLocalSocket *client,
                          const QString &id,
                          const QJsonValue &result);
        void sendError(QLocalSocket *client,
                       const QString &id,
                       const QString &message);

        QRemoteObjectNode mNode;
        QPointer<FrictionApiReplica> mReplica;

        QLocalServer *mServer;
        QList<QLocalSocket*> mClients;

        bool mConnected;
    };
}

#endif // FRICTION_API_BRIDGE_H
