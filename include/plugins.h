// Part of Friction <https://friction.graphics>
// SPDX-FileCopyrightText: Copyright (c) Ole-André Rodlie and contributors
// SPDX-License-Identifier: BSD-3-Clause

#ifndef FRICTION_API_PLUGINS_H
#define FRICTION_API_PLUGINS_H

#include "api_global.h"
#include "plugin.h"

#include <QObject>
#include <QMap>
#include <QStringList>
#include <QList>

namespace Friction::Api
{
    struct PluginInfo
    {
        QString id;
        int version = 0;
        QString type;
        QString author;
        QString description;
        QString website;
        QString filePath;
        bool isLoaded = false;
        QString errorString;
    };

    class Plugins : public QObject
    {
        Q_OBJECT
    public:
        explicit FRICTION_API_EXPORT Plugins(QObject *parent = nullptr);
        void scanAndLoad(const QString &path,
                         QWidget* guiParent = nullptr);
        QList<Plugin*> activePlugins() const;
        QList<PluginInfo> allPluginsInfo() const;

    private:
        PluginInfo getPluginMeta(const QString &filePath);

        QMap<QString, QPair<int, QString>> mAvailablePaths;
        QList<Plugin*> mLoadedPlugins;
        QList<PluginInfo> mPluginMetaList;
    };
}

#endif // FRICTION_API_PLUGINS_H
