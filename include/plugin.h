// Part of Friction <https://friction.graphics>
// SPDX-FileCopyrightText: Copyright (c) Ole-André Rodlie and contributors
// SPDX-License-Identifier: BSD-3-Clause

#ifndef FRICTION_API_PLUGIN_H
#define FRICTION_API_PLUGIN_H

#include "api_global.h"

#include <QString>
#include <QObject>
#include <QWidget>
#include <QtPlugin>

namespace Friction::Api
{
    class FRICTION_API_EXPORT Plugin
    {
    public:
        virtual ~Plugin() = default;

        virtual QString id() const = 0;
        virtual int version() const = 0;
        virtual QString type() const = 0; // "Core" or "Gui"

        virtual void initialize(QObject* parent) = 0;
        virtual void initializeGui(QWidget* parent) = 0;

        virtual QWidget* createWidget(QWidget* parent = nullptr) = 0;
        virtual void testMethod() = 0;
    };
}

Q_DECLARE_INTERFACE(Friction::Api::Plugin, FRICTION_API_PLUGIN)

#endif // FRICTION_API_PLUGIN_H
