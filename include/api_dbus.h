// Part of Friction <https://friction.graphics>
// SPDX-FileCopyrightText: Copyright (c) Ole-André Rodlie and contributors
// SPDX-License-Identifier: BSD-3-Clause

#ifndef FRICTION_API_DBUS_H
#define FRICTION_API_DBUS_H

#ifdef FRICTION_HAS_DBUS
#include <QtDBus/QDBusAbstractAdaptor>

namespace Friction::Api
{
    class Server;
    class Adaptor : public QDBusAbstractAdaptor
    {
        Q_OBJECT
        Q_CLASSINFO("D-Bus Interface", FRICTION_API_ID)

    public:
        explicit Adaptor(Server *parent);

    public Q_SLOTS:
        void testMethod();

    Q_SIGNALS:
        void message(const QString &text);
    };
}
#endif

#endif // FRICTION_API_DBUS_H
