// Friction API <https://friction.graphics>
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
    struct DBusRange
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

    struct DBusScene
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

    class Server;
    class Adaptor : public QDBusAbstractAdaptor
    {
        Q_OBJECT
        Q_CLASSINFO("D-Bus Interface",
                    FRICTION_API_DBUS_ID)

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

Q_DECLARE_METATYPE(Friction::Api::DBusRange)
Q_DECLARE_METATYPE(Friction::Api::DBusScene)
Q_DECLARE_METATYPE(QList<Friction::Api::DBusScene>)

#endif // FRICTION_HAS_DBUS

#endif // FRICTION_API_DBUS_H
