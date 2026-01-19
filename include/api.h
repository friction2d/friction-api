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
    class FRICTION_API_EXPORT Server : public FrictionApiSource
    {
        Q_OBJECT
        Q_CLASSINFO("D-Bus Interface", FRICTION_API_ID)

    public:
        explicit Server(QObject *parent = nullptr);

        bool isDBusConnected() const;
        bool isHostConnected() const;

    public Q_SLOTS:
        Q_SCRIPTABLE void testMethod() override;
        Q_SCRIPTABLE void newProject(const int width,
                                     const int height,
                                     const double fps,
                                     const int start,
                                     const int end,
                                     const double r,
                                     const double g,
                                     const double b,
                                     const double a) override;
        Q_SCRIPTABLE void loadProject(const QString &path) override;

    private:
        void setupDBus();
        void setupRemoteObjects();

        bool mDBusConnected;
        bool mHostConnected;
        QRemoteObjectHost *mRemoteHost;
    };
}

#endif // FRICTION_API_H
