// Part of Friction <https://friction.graphics>
// SPDX-FileCopyrightText: Copyright (c) Ole-André Rodlie and contributors
// SPDX-License-Identifier: BSD-3-Clause

#include <QCoreApplication>
#include "api.h"

using namespace Friction::Api;

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);
    ApiServer server(&app);

    qDebug() << "--- Friction API Server Running ---";
    qDebug() << "Socket:" << FRICTION_API_SOCKET;

    return app.exec();
}

