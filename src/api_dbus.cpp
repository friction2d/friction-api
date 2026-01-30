// Friction API <https://friction.graphics>
// SPDX-FileCopyrightText: Copyright (c) Ole-André Rodlie and contributors
// SPDX-License-Identifier: BSD-3-Clause

#ifdef FRICTION_HAS_DBUS

#include "api_dbus.h"
#include "api.h"
#include <QtDBus/QDBusMetaType>

using namespace Friction::Api;

Adaptor::Adaptor(Server *parent)
    : QDBusAbstractAdaptor(parent)
{
    qDBusRegisterMetaType<DBusRange>();
    qDBusRegisterMetaType<DBusScene>();
    qDBusRegisterMetaType<QList<DBusScene>>();

    connect(parent, &Server::message,
            this, &Adaptor::message);
    connect(parent, &Server::scenesChanged,
            this, &Adaptor::scenesChanged);
    connect(parent, &Server::currentSceneChanged,
            this, &Adaptor::currentSceneChanged);
}

int Adaptor::getCurrentScene()
{
    Server *srv = qobject_cast<Server*>(parent());
    if (srv) {
        return srv->currentScene();
    }
    return -1;
}

bool Adaptor::setCurrentScene(const int id)
{
    Server *srv = qobject_cast<Server*>(parent());
    if (srv) {
        srv->setCurrentScene(id);
        return srv->currentScene() == id;
    }
    return false;
}

QList<DBusScene> Adaptor::getScenes()
{
    QList<DBusScene> result;
    Server *srv = qobject_cast<Server*>(parent());
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
