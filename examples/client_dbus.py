#!/bin/env python
# Part of Friction <https://friction.graphics>
# SPDX-FileCopyrightText: Copyright (c) Ole-André Rodlie and contributors
# SPDX-License-Identifier: BSD-3-Clause

import dbus
bus = dbus.SessionBus()
obj = bus.get_object("graphics.friction.api", "/friction/api")
iface = dbus.Interface(obj, "graphics.friction.api")
print(iface.getScenes())

