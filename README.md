# Friction API *(draft mode)*

Friction API using DBus, QtRO (RemoteObjects) and Socket (with JSON response).

## Examples

Client examples, check the `examples/` folder.

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

### QtRO (RemoteObjects)

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

### Socket (Python)

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
