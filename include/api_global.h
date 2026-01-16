// Part of Friction <https://friction.graphics>
// SPDX-FileCopyrightText: Copyright (c) Ole-André Rodlie and contributors
// SPDX-License-Identifier: BSD-3-Clause

#ifndef FRICTION_API_GLOBAL_H
#define FRICTION_API_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(FRICTION_API_LIBRARY)
#define FRICTION_API_EXPORT Q_DECL_EXPORT
#else
#define FRICTION_API_EXPORT Q_DECL_IMPORT
#endif

#endif // FRICTION_API_GLOBAL_H
