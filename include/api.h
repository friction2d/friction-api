// Part of Friction <https://friction.graphics>
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
    class FRICTION_API_EXPORT Server : public FrictionApiSimpleSource
    {
        Q_OBJECT

    public:
        explicit Server(QObject *parent = nullptr);

        bool isDBusConnected() const;
        bool isHostConnected() const;
        bool isConnected() const;

        void setScenes(const QList<Scene> &scenes);
        void setCurrentScene(const int id);

    private:
        void setupDBus();
        void setupRemoteObjects();

        bool mDBusConnected;
        bool mHostConnected;
        QRemoteObjectHost *mRemoteHost;
    };
}

#endif // FRICTION_API_H
