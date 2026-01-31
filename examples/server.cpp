// Friction API <https://friction.graphics>
// SPDX-FileCopyrightText: Copyright (c) Ole-André Rodlie and contributors
// SPDX-License-Identifier: BSD-3-Clause

#include "host.h"

#include <QCoreApplication>

using namespace Friction::Api;

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);
    Host host(&app);

    qWarning() << "Adding dummy data";
    QList<fScene> initialScenes;
    initialScenes << fScene(0, "Scene 0", 1920, 1080, 30, fRange{0, 299})
                  << fScene(1, "Scene 1", 1920, 1080, 30, fRange{0, 299})
                  << fScene(2, "Scene 2", 1920, 1080, 30, fRange{0, 299});
    host.setScenes(initialScenes);
    host.setCurrentScene(0);

    return app.exec();
}

