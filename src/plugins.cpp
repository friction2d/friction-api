// Part of Friction <https://friction.graphics>
// SPDX-FileCopyrightText: Copyright (c) Ole-André Rodlie and contributors
// SPDX-License-Identifier: BSD-3-Clause

#include "plugins.h"

#include <QDir>
#include <QLibrary>
#include <QPluginLoader>
#include <QJsonObject>
#include <QDebug>

using namespace Friction::Api;

Plugins::Plugins(QObject *parent)
    : QObject(parent)
{

}

PluginInfo Plugins::getPluginMeta(const QString &filePath)
{
    PluginInfo info;
    info.filePath = filePath;

    QPluginLoader loader(filePath);
    QJsonObject meta = loader.metaData().value("MetaData").toObject();

    info.id = meta.value("id").toString();
    info.version = meta.value("version").toInt();
    info.type = meta.value("type").toString();
    info.author = meta.value("author").toString("Unknown");
    info.description = meta.value("description").toString("No description.");
    info.website = meta.value("website").toString();

    return info;
}

void Plugins::scanAndLoad(const QString &path,
                          QWidget* guiParent)
{
    QDir dir(path);
    if (!dir.exists()) { return; }

    mPluginMetaList.clear();
    const auto entryList = dir.entryInfoList(QDir::Files |
                                             QDir::NoDotAndDotDot);

    for (const QFileInfo &info : entryList) {
        if (!QLibrary::isLibrary(info.absoluteFilePath())) { continue; }

        PluginInfo meta = getPluginMeta(info.absoluteFilePath());
        if (meta.id.isEmpty()) { continue; }

        if (!mAvailablePaths.contains(meta.id) ||
            meta.version > mAvailablePaths[meta.id].first) {
            mAvailablePaths[meta.id] = {meta.version, info.absoluteFilePath()};
        }
    }

    for (auto it = mAvailablePaths.begin(); it != mAvailablePaths.end(); ++it) {
        QString fullPath = it.value().second;
        PluginInfo currentMeta = getPluginMeta(fullPath);

        QPluginLoader* loader = new QPluginLoader(fullPath, this);
        QObject *instance = loader->instance();

        if (instance) {
            if (Plugin *plugin = qobject_cast<Plugin*>(instance)) {
                plugin->initialize(this);
                if (plugin->type() == "Gui" && guiParent) {
                    plugin->initializeGui(guiParent);
                }
                mLoadedPlugins.append(plugin);
                currentMeta.isLoaded = true;
                qWarning() << "Loaded:" << currentMeta.id << "by" << currentMeta.author;
            } else {
                currentMeta.errorString = "Invalid Plugin interface";
                loader->unload();
            }
        } else {
            currentMeta.errorString = loader->errorString();
        }

        mPluginMetaList.append(currentMeta);
    }
}

QList<Plugin*> Plugins::activePlugins() const
{
    return mLoadedPlugins;
}

QList<PluginInfo> Plugins::allPluginsInfo() const
{
    return mPluginMetaList;
}
