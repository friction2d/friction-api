// Friction API <https://friction.graphics>
// SPDX-FileCopyrightText: Copyright (c) Ole-André Rodlie and contributors
// SPDX-License-Identifier: BSD-3-Clause

#include "bridge.h"
#include <QDebug>

using namespace Friction::Api;

Bridge::Bridge(QObject *parent)
    : QObject(parent)
    , mServer(new QLocalServer(this))
    , mConnected(false)
{

}

Bridge::~Bridge()
{
    if (mServer->isListening()) {
        mServer->close();
    }
}

bool Bridge::start(const QString &socketName)
{
    qWarning() << "--- Friction Bridge Running ---";
    qWarning() << "Bridge: Using socket" << socketName;

    QLocalServer::removeServer(socketName);

    if (!mServer->listen(socketName)) {
        qCritical() << "Bridge: Unable to start socket!" << mServer->errorString();
        return false;
    }

    mNode.connectToNode(QUrl(QStringLiteral(FRICTION_API_SOCKET)));
    mReplica = mNode.acquire<FrictionApiReplica>();

    if (!mReplica) {
        qCritical() << "Bridge: Unable to acquire replica!";
        return false;
    }

    connect(mReplica.data(), &FrictionApiReplica::initialized,
            this, [this]() {
        qWarning() << "Bridge: Connected to host";
        mConnected = true;
    });

    connect(mReplica.data(), &FrictionApiReplica::stateChanged,
            this, &Bridge::handleReplicaStateChanged);

    connect(mReplica.data(), &FrictionApiReplica::scenesChanged,
            this, [this](QList<fScene> scenes) {
        broadcast(QJsonObject{
            {"type", "signal"},
            {"name", "scenesChanged"},
            {"data", serializeScenes(scenes)}
        });
    });

    //connect(mReplica.data(), &FrictionApiReplica::currentSceneChanged,
      //      this, &Bridge::???);

    connect(mReplica.data(), &FrictionApiReplica::message,
            this, [this](const QString &text) {
        broadcast(QJsonObject{
            {"type", "signal"},
            {"name", "message"},
            {"data", text}
        });
    });

    connect(mServer, &QLocalServer::newConnection,
            this, &Bridge::handleNewConnection);

    return true;
}

void Bridge::handleNewConnection()
{
    QLocalSocket *client = mServer->nextPendingConnection();
    if (!client) { return; }

    connect(client, &QLocalSocket::disconnected,
            this, &Bridge::handleClientDisconnect);
    connect(client, &QLocalSocket::readyRead,
            this, &Bridge::processIncomingData);

    mClients.append(client);
    qWarning() << "Bridge: New client connected";
}

void Bridge::processIncomingData()
{
    QLocalSocket *client = qobject_cast<QLocalSocket*>(sender());
    if (!client) { return; }

    while (client->canReadLine()) {
        QByteArray line = client->readLine().trimmed();
        if (line.isEmpty()) { continue; }

        QJsonDocument doc = QJsonDocument::fromJson(line);
        if (!doc.isNull() && doc.isObject()) {
            processClientRequest(client, doc.object());
        }
    }
}

void Bridge::processClientRequest(QLocalSocket *client,
                                  const QJsonObject &req)
{
    QString id = req["id"].toString();
    QString method = req["method"].toString();
    QJsonObject params = req["params"].toObject();

    if (!mConnected) {
        sendError(client, id, QStringLiteral("Host is not connected!"));
        return;
    }

    if (method == "getScenes") {
        sendResponse(client, id, serializeScenes(mReplica->scenes())["scenes"]);
    }
    else if (method == "setCurrentScene") {
        if (params.contains("id")) {
            mReplica->pushCurrentScene(params["id"].toInt());
            sendResponse(client, id, true);
        } else {
            sendError(client, id, "Missing parameter: id");
        }
    } else {
        sendError(client, id, "Unknown method: " + method);
    }
}

void Bridge::handleClientDisconnect()
{
    QLocalSocket *client = qobject_cast<QLocalSocket*>(sender());
    if (client) {
        mClients.removeAll(client);
        client->deleteLater();
        qWarning() << "Bridge: Client disconnected";
    }
}

void Bridge::handleReplicaStateChanged(QRemoteObjectReplica::State state)
{
    mConnected = (state == QRemoteObjectReplica::Valid);
    broadcast(QJsonObject{
        {"type", "signal"},
        {"name", "hostStateChanged"},
        {"state", (int)state}
    });
}

QJsonObject Bridge::serializeScenes(QList<fScene> scenes)
{
    QJsonObject root;

    QJsonArray scenesArray;
    for (const auto &s : scenes) {
        QJsonObject sceneObj;
        sceneObj["id"] = s.id();
        sceneObj["title"] = s.title();
        sceneObj["width"] = s.width();
        sceneObj["height"] = s.height();
        sceneObj["fps"] = s.fps();
        sceneObj["range"] = QJsonObject{
            {"first", s.range().first()},
            {"last", s.range().last()}
        };
        scenesArray.append(sceneObj);
    }
    root["scenes"] = scenesArray;
    return root;
}

void Bridge::sendResponse(QLocalSocket *client,
                          const QString &id,
                          const QJsonValue &result)
{
    QJsonObject resp;
    resp["type"] = "method_return";
    resp["id"] = id;
    resp["result"] = result;
    client->write(QJsonDocument(resp).toJson(QJsonDocument::Compact) + "\n");
}

void Bridge::sendError(QLocalSocket *client,
                       const QString &id,
                       const QString &message)
{
    QJsonObject resp;
    resp["type"] = "method_return";
    resp["id"] = id;
    resp["error"] = message;
    client->write(QJsonDocument(resp).toJson(QJsonDocument::Compact) + "\n");
}

void Bridge::broadcast(const QJsonObject &json)
{
    if (mClients.isEmpty()) { return; }
    QByteArray data = QJsonDocument(json).toJson(QJsonDocument::Compact) + "\n";
    for (QLocalSocket *client : std::as_const(mClients)) {
        if (client->isValid() &&
            client->state() == QLocalSocket::ConnectedState) {
            client->write(data);
            client->flush();
        }
    }
}
