// Friction API <https://friction.graphics>
// SPDX-FileCopyrightText: Copyright (c) Ole-André Rodlie and contributors
// SPDX-License-Identifier: BSD-3-Clause

#include "bridge.h"

#include <QCoreApplication>

using namespace Friction::Api;

int main(int argc, char *argv[]) {
    QCoreApplication app(argc, argv);

    Friction::Api::Bridge bridge;
    if (!bridge.start(FRICTION_API_BRIDGE)) { return -1; }

    return app.exec();
}
