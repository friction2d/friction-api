# Friction API Specification

**This API is currently a Proof of Concept. A stable, first version is scheduled for release in 2026. All code examples work, but the API calls/names are just for testing.**

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

## Client Examples

Check the `examples/` folder for various examples.

### DBus (dbus-send)

```sh
dbus-send --session --type=method_call --print-reply --dest=graphics.friction.api /friction/api graphics.friction.api.getScenes
```

### DBus (Python)

```python
import dbus
bus = dbus.SessionBus()
obj = bus.get_object("graphics.friction.api", "/friction/api")
iface = dbus.Interface(obj, "graphics.friction.api")
print(iface.getScenes())
```

### Qt Remote Objects (C++)

```cpp
#include "rep_api_replica.h"
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

This example uses `examples/friction_api.py`.
