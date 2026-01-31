// Friction API <https://friction.graphics>
// SPDX-FileCopyrightText: Copyright (c) Ole-André Rodlie and contributors
// SPDX-License-Identifier: BSD-3-Clause

#ifdef FRICTION_HAS_DBUS

#include "adaptor.h"
#include "host.h"

#include <QtDBus/QDBusMetaType>

using namespace Friction::Api;

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
