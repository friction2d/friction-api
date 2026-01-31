// Friction API <https://friction.graphics>
// SPDX-FileCopyrightText: Copyright (c) Ole-André Rodlie and contributors
// SPDX-License-Identifier: BSD-3-Clause

#include "host.h"

#include <QDebug>
#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonArray>

#ifdef FRICTION_HAS_DBUS
#include <QtDBus/QDBusConnection>
#include <QtDBus/QDBusMetaType>
#endif

using namespace Friction::Api;

Host::Host(QObject *parent,
           const QString &socketName,
           const QString &socketBridge,
           const QString &sessionName,
           const QString &sessionPath)
    : FrictionApiSimpleSource(parent)
    , mDBusConnected(false)
    , mHostConnected(false)
    , mBridgeConnected(false)
    , mRemoteHost(nullptr)
    , mBridgeServer(nullptr)
{
    qWarning() << "--- Friction Host Running ---";
    setupDBus(sessionName,
              sessionPath);
    setupRemoteObjects(socketName);
    setupBridge(socketBridge);
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
#else
    Q_UNUSED(sessionName)
    Q_UNUSED(sessionPath)
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

void Host::setupBridge(const QString &socketName)
{
    mBridgeServer = new QLocalServer(this);
    QLocalServer::removeServer(socketName);
    mBridgeConnected = mBridgeServer->listen(socketName);

    qWarning() << "Host: Bridge server is"
               << (mBridgeConnected ? "Online" : "Offline")
               << socketName;

    connect(mBridgeServer, &QLocalServer::newConnection,
            this, &Host::handleNewBridgeConnection);

    connect(this, &FrictionApiSimpleSource::message,
            this, [this](const QString &text) {
        broadcastToBridge(QJsonObject{
            {"type", "signal"},
            {"name", "message"},
            {"data", text}
        });
    });

    connect(this, &FrictionApiSimpleSource::scenesChanged,
            this, [this](const QList<fScene> &scenes){
        broadcastToBridge(QJsonObject{
            {"type", "signal"},
            {"name", "scenesChanged"},
            {"data", serializeScenes(scenes)}
        });
    });
}

void Host::handleNewBridgeConnection()
{
    QLocalSocket *client = mBridgeServer->nextPendingConnection();
    if (!client) { return; }

    connect(client, &QLocalSocket::readyRead,
            this, &Host::processBridgeRequest);
    connect(client, &QLocalSocket::disconnected,
            this, &Host::handleBridgeDisconnect);

    mBridgeClients.append(client);

    qWarning() << "Bridge: Added new client";
}

void Host::processBridgeRequest()
{
    QLocalSocket *client = qobject_cast<QLocalSocket*>(sender());
    if (!client) return;

    while (client->canReadLine()) {
        QByteArray line = client->readLine().trimmed();
        if (line.isEmpty()) continue;

        QJsonDocument doc = QJsonDocument::fromJson(line);
        if (doc.isNull()) {
            qWarning() << "Bridge: Got invalid JSON from client";
            continue;
        }

        QJsonObject req = doc.object();
        QString id = req["id"].toString();
        QString method = req["method"].toString();
        QJsonObject params = req["params"].toObject();

        qDebug() << "Bridge: Request" << method << "ID" << id;

        if (method == "getScenes") {
            sendBridgeResponse(client, id, serializeScenes(scenes())["scenes"]);
        }
        else if (method == "setCurrentScene") {
            if (params.contains("id")) {
                int sceneId = params["id"].toInt();
                pushCurrentScene(sceneId);
                sendBridgeResponse(client, id, true);
            } else {
                sendBridgeError(client, id, "Parameter 'id' missing");
            }
        } else {
            sendBridgeError(client, id, "Unknown method: " + method);
        }
    }
}

void Host::handleBridgeDisconnect()
{
    QLocalSocket *client = qobject_cast<QLocalSocket*>(sender());
    if (client) {
        mBridgeClients.removeAll(client);
        client->deleteLater();

        qWarning() << "Bridge: Client disconnected";
    }
}

void Host::broadcastToBridge(const QJsonObject &json)
{
    if (mBridgeClients.isEmpty()) { return; }

    QByteArray data = QJsonDocument(json).toJson(QJsonDocument::Compact) + "\n";
    for (QLocalSocket *client : std::as_const(mBridgeClients)) {
        if (client->isValid() &&
            client->state() == QLocalSocket::ConnectedState) {
            client->write(data);
            client->flush();
        }
    }
}

void Host::sendBridgeResponse(QLocalSocket *client,
                              const QString &id,
                              const QJsonValue &result)
{
    if (!client ||
        client->state() != QLocalSocket::ConnectedState) { return; }

    QJsonObject resp;
    resp["type"] = "method_return";
    resp["id"] = id;
    resp["result"] = result;

    client->write(QJsonDocument(resp).toJson(QJsonDocument::Compact) + "\n");
    client->flush();
}

void Host::sendBridgeError(QLocalSocket *client,
                           const QString &id,
                           const QString &message)
{
    if (!client ||
        client->state() != QLocalSocket::ConnectedState) { return; }

    QJsonObject resp;
    resp["type"] = "method_return";
    resp["id"] = id;
    resp["error"] = message;

    client->write(QJsonDocument(resp).toJson(QJsonDocument::Compact) + "\n");
    client->flush();
}

QJsonObject Host::serializeScenes(const QList<fScene> &scenes)
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

bool Host::isDBusConnected() const
{
    return mDBusConnected;
}

bool Host::isHostConnected() const
{
    return mHostConnected;
}

bool Host::isBridgeConnected() const
{
    return mBridgeConnected;
}

bool Host::isConnected() const
{
    return mHostConnected && (mDBusConnected || mBridgeConnected);
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

#ifdef FRICTION_HAS_DBUS
Adaptor::Adaptor(Host *parent)
    : QDBusAbstractAdaptor(parent)
{
    qDBusRegisterMetaType<DBusRange>();
    qDBusRegisterMetaType<DBusScene>();
    qDBusRegisterMetaType<QList<DBusScene>>();

    connect(parent, &Host::message,
            this, &Adaptor::message);
    connect(parent, &Host::scenesChanged,
            this, &Adaptor::scenesChanged);
    connect(parent, &Host::currentSceneChanged,
            this, &Adaptor::currentSceneChanged);
}

int Adaptor::getCurrentScene()
{
    Host *srv = qobject_cast<Host*>(parent());
    if (srv) {
        return srv->currentScene();
    }
    return -1;
}

bool Adaptor::setCurrentScene(const int id)
{
    Host *srv = qobject_cast<Host*>(parent());
    if (srv) {
        srv->setCurrentScene(id);
        return srv->currentScene() == id;
    }
    return false;
}

QList<DBusScene> Adaptor::getScenes()
{
    QList<DBusScene> result;
    Host *srv = qobject_cast<Host*>(parent());
    if (srv) {
        const auto scenesList = srv->scenes();
        for (const auto &s : scenesList) {
            DBusRange dr{s.range().first(),
                         s.range().last()};
            DBusScene ds;
            ds.id = s.id();
            ds.title = s.title();
            ds.width = s.width();
            ds.height = s.height();
            ds.fps = s.fps();
            ds.range = dr;

            result.append(ds);
        }
    }
    return result;
}
#endif // FRICTION_HAS_DBUS
