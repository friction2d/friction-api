// Friction API <https://friction.graphics>
// SPDX-FileCopyrightText: Copyright (c) Ole-André Rodlie and contributors
// SPDX-License-Identifier: BSD-3-Clause

#ifndef FRICTION_API_H
#define FRICTION_API_H

#include "api_global.h"
#include "rep_api_source.h"

#include <QObject>
#include <QRemoteObjectHost>

namespace Friction::Api
{
    class FRICTION_API_EXPORT Host : public FrictionApiSimpleSource
    {
        Q_OBJECT

    public:
        explicit Host(QObject *parent = nullptr,
                      const QString &socketName = FRICTION_API_SOCKET,
                      const QString &sessionName = FRICTION_API_DBUS_ID,
                      const QString &sessionPath = FRICTION_API_DBUS_PATH);

        bool isDBusConnected() const;
        bool isHostConnected() const;
        bool isConnected() const;

        void setScenes(QList<fScene> scenes) override;
        void setCurrentScene(const int id) override;

    private:
        void setupDBus(const QString &sessionName,
                       const QString &sessionPath);
        void setupRemoteObjects(const QString &socketName);

        bool mDBusConnected;
        bool mHostConnected;
        QRemoteObjectHost *mRemoteHost;
    };
}

#endif // FRICTION_API_H
