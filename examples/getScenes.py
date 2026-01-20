#!/bin/env python
import dbus
bus = dbus.SessionBus()
obj = bus.get_object("graphics.friction.api", "/friction/api")
iface = dbus.Interface(obj, "graphics.friction.api")
print(iface.getScenes())

