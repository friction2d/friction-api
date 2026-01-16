# Friction API

Friction API and IPC (DBus/QtRO) library .

```
mkdir build && cd build
cmake -G Ninja -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=/usr ..
cmake --build .
cmake --install . --prefix=/path/to/pkg
```

```
pkg
├── include
│   └── friction
│       └── api
│           ├── api_dbus.h
│           ├── api_global.h
│           ├── api.h
│           ├── rep_api_replica.h
│           └── rep_api_source.h
├── lib
│   └── x86_64-linux-gnu
│       ├── cmake
│       │   └── FrictionApi
│       │       ├── FrictionApiConfig.cmake
│       │       ├── FrictionApiTargets.cmake
│       │       └── FrictionApiTargets-release.cmake
│       ├── libfrictionapi.so -> libfrictionapi.so.1
│       ├── libfrictionapi.so.1 -> libfrictionapi.so.1.0.0
│       └── libfrictionapi.so.1.0.0
└── share
    └── friction
        └── api
            └── api.rep

```
