# Friction API Specification

**This API is currently a Proof of Concept. A stable, first version is scheduled for release in 2026. All code examples work, but are just for testing.**

The Friction API is built on a "Single Source, Multiple Channels" architecture. It utilizes Qt Remote Objects (QtRO) as its core engine, with specialized adapters for DBus and an integrated JSON-RPC Bridge via QLocalServer.

By leveraging QtRO as the primary source of truth, Friction ensures that state synchronization and method calls are handled atomically.

### Why three different interfaces?

To support a diverse ecosystem the API provides protocol-specific access points:

* **Qt Remote Objects (Native Qt IPC)**
  * High-performance, low-latency communication between Qt-based applications
  * Native support for Plain Old Data (POD) types, automated signal/slot synchronization, and seamless cross-platform performance (Linux, macOS, Windows)
* **DBus (System Integration)**
  * Standardized Inter-Process Communication (IPC) for Linux and BSD environments
  * Allows to integrate deeply with desktop environments, system services, and existing automation tools
* **JSON Bridge via QLocalServer (Universal/Failsafe)**
  * Language-agnostic, universal access
  * Enables tools to control Friction via standard Unix Domain Sockets (or Named Pipes on Windows). The use of JSON-RPC 2.0 eliminates the need for complex library dependencies or specific Qt runtimes

## Host Example

in `example/server.cpp` you can find a simple "server" implementation, it uses `Friction::Api::Host` that takes care of everything. At the moment the host is very generic, mostly just for testing the API.

## Client Examples (get scenes from a project)

Check the `examples/` folder for various examples.

### DBus (dbus-send)

```sh
dbus-send --session --type=method_call --print-reply --dest=graphics.friction.api /friction/api graphics.friction.api.getScenes
```
```
method return time=1769897343.494107 sender=:1.583 -> destination=:1.584 serial=6 reply_serial=2
   array [
      struct {
         int32 0
         string "Scene 0"
         int32 1920
         int32 1080
         double 30
         struct {
            int32 0
            int32 299
         }
      }
      struct {
         int32 1
         string "Scene 1"
         int32 1920
         int32 1080
         double 30
         struct {
            int32 0
            int32 299
         }
      }
      struct {
         int32 2
         string "Scene 2"
         int32 1920
         int32 1080
         double 30
         struct {
            int32 0
            int32 299
         }
      }
   ]
```

### DBus (Python)

```python
import dbus
bus = dbus.SessionBus()
obj = bus.get_object("graphics.friction.api", "/friction/api")
iface = dbus.Interface(obj, "graphics.friction.api")
print(iface.getScenes())
```
```
dbus.Array([dbus.Struct((dbus.Int32(0), dbus.String('Scene 0'), dbus.Int32(1920), dbus.Int32(1080), dbus.Double(30.0), dbus.Struct((dbus.Int32(0), dbus.Int32(299)), signature=None)), signature=None), dbus.Struct((dbus.Int32(1), dbus.String('Scene 1'), dbus.Int32(1920), dbus.Int32(1080), dbus.Double(30.0), dbus.Struct((dbus.Int32(0), dbus.Int32(299)), signature=None)), signature=None), dbus.Struct((dbus.Int32(2), dbus.String('Scene 2'), dbus.Int32(1920), dbus.Int32(1080), dbus.Double(30.0), dbus.Struct((dbus.Int32(0), dbus.Int32(299)), signature=None)), signature=None)], signature=dbus.Signature('(isiid(ii))'))
```

### Qt Remote Objects (C++)

```cpp
#include "rep_api_replica.h" // generated from api.rep
#include <QCoreApplication>
#include <QRemoteObjectNode>
#include <QDebug>

int main(int argc, char *argv[]) {
    QCoreApplication app(argc, argv);
    QRemoteObjectNode node;
    node.connectToNode(QUrl(QStringLiteral(FRICTION_API_SOCKET)));
    auto replica = node.acquire<FrictionApiReplica>();
    QObject::connect(replica, &FrictionApiReplica::initialized,
                     [replica, &app]() {
        for (const auto &s : replica->scenes()) {
            qWarning() << "--> ID:" << s.id()
                       << "Title:" << s.title()
                       << "Width:" << s.width()
                       << "Height:" << s.height()
                       << "Fps:" << s.fps()
                       << "Start Frame:" << s.range().first()
                       << "End Frame:" << s.range().last();
        }
    });
    return app.exec();
}
```
```
--> ID: 0 Title: "Scene 0" Width: 1920 Height: 1080 Fps: 30 Start Frame: 0 End Frame: 299
--> ID: 1 Title: "Scene 1" Width: 1920 Height: 1080 Fps: 30 Start Frame: 0 End Frame: 299
--> ID: 2 Title: "Scene 2" Width: 1920 Height: 1080 Fps: 30 Start Frame: 0 End Frame: 299
```

### JSON-RPC Bridge (Python)

```python
from friction_api import FrictionClient
import time

client = FrictionClient()

client.on("message", lambda data: print(f"Message from Friction: {data['data']}"))
client.on("scenesChanged", lambda data: print(f"Friction Scenes Changed: {data['data']['scenes']}"))

try:
    client.connect()

    scenes = client.call("getScenes")
    for s in scenes:
            print(f" - {s['id']}: {s['title']} ({s['width']}x{s['height']})")

    while True:
        time.sleep(1)

except Exception as e:
    print(f"Error: {e}")
```
```
 - 0: Scene 0 (1920x1080)
 - 1: Scene 1 (1920x1080)
 - 2: Scene 2 (1920x1080)
```

This example uses `examples/friction_api.py`.
