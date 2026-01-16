// Part of Friction <https://friction.graphics>
// SPDX-FileCopyrightText: Copyright (c) Ole-André Rodlie and contributors
// SPDX-License-Identifier: BSD-3-Clause

#include "dummyplugin.h"

#include <QPushButton>
#include <QDebug>

void DummyPlugin::initialize(QObject* parent)
{
    qWarning() << "Dummy Core init with parent (Manager):" << parent;
}

void DummyPlugin::initializeGui(QWidget* parent)
{
    qWarning() << "Dummy GUI init with parent (MainWindow):" << parent;
}

void DummyPlugin::testMethod()
{
    qWarning() << "Dummy Plugin testMethod() was called!";
}

QWidget* DummyPlugin::createWidget(QWidget* parent)
{
    QPushButton* button = new QPushButton(parent);
    button->setText(QStringLiteral("Button from Dummy Plugin"));

    QObject::connect(button, &QPushButton::clicked, []() {
        qWarning() << "Button in plugin was clicked!";
    });

    return button;
}

