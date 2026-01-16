// Part of Friction <https://friction.graphics>
// SPDX-FileCopyrightText: Copyright (c) Ole-André Rodlie and contributors
// SPDX-License-Identifier: BSD-3-Clause

#include "plugin.h"

#include <QObject>

class DummyPlugin : public QObject, public Friction::Api::Plugin
{
    Q_OBJECT
    Q_INTERFACES(Friction::Api::Plugin)
    Q_PLUGIN_METADATA(IID FRICTION_API_PLUGIN FILE "dummy.json")

public:
    explicit DummyPlugin(QObject *parent = nullptr) : QObject(parent) {}

    QString id() const override { return QStringLiteral("graphics.friction.dummy"); }
    int version() const override { return 100; }
    QString type() const override { return QStringLiteral("Gui"); }

    void initialize(QObject* parent) override;
    void initializeGui(QWidget* parent) override;
    QWidget* createWidget(QWidget* parent = nullptr) override;
    void testMethod() override;
};

