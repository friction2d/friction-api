// Part of Friction <https://friction.graphics>
// SPDX-FileCopyrightText: Copyright (c) Ole-André Rodlie and contributors
// SPDX-License-Identifier: BSD-3-Clause

#ifndef FRICTION_API_DBUS_H
#define FRICTION_API_DBUS_H

#ifdef FRICTION_HAS_DBUS
#include <QtDBus/QDBusAbstractAdaptor>
#include <QtDBus/QDBusArgument>
#include <QList>
#include <QString>

namespace Friction::Api
{
    struct DBusScene
    {
        int id;
        QString title;

        friend QDBusArgument &operator<<(QDBusArgument &arg,
                                         const DBusScene &scene)
        {
            arg.beginStructure();
            arg << scene.id << scene.title;
            arg.endStructure();
            return arg;
        }

        friend const QDBusArgument &operator>>(const QDBusArgument &arg,
                                               DBusScene &scene)
        {
            arg.beginStructure();
            arg >> scene.id >> scene.title;
            arg.endStructure();
            return arg;
        }
    };

    class Server;
    class Adaptor : public QDBusAbstractAdaptor
    {
        Q_OBJECT
        Q_CLASSINFO("D-Bus Interface", FRICTION_API_ID)

    public:
        explicit Adaptor(Server *parent);

    public Q_SLOTS:
        QList<Friction::Api::DBusScene> getScenes();
        int getCurrentScene();
        bool setCurrentScene(const int id);

    Q_SIGNALS:
        void message(const QString &text);
        void scenesChanged();
        void currentSceneChanged(const int id);
    };
}

Q_DECLARE_METATYPE(Friction::Api::DBusScene)
Q_DECLARE_METATYPE(QList<Friction::Api::DBusScene>)

#endif // FRICTION_HAS_DBUS
#endif // FRICTION_API_DBUS_H
