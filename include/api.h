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
    class FRICTION_API_EXPORT ApiServer : public FrictionApiSource
    {
        Q_OBJECT
        Q_CLASSINFO("D-Bus Interface", FRICTION_API_ID)

    public:
        explicit ApiServer(QObject *parent = nullptr);

        bool isDBusConnected() const;
        bool isHostConnected() const;

    public Q_SLOTS:
        Q_SCRIPTABLE void testMethod() override;

    private:
        void setupDBus();
        void setupRemoteObjects();

        bool mDBusConnected;
        bool mHostConnected;
        QRemoteObjectHost *mRemoteHost;
    };
}

#endif // FRICTION_API_H
