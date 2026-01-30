# Friction API <https://friction.graphics>
# SPDX-FileCopyrightText: Copyright (c) Ole-André Rodlie and contributors
# SPDX-License-Identifier: BSD-3-Clause

cmake_minimum_required(VERSION 3.16)

project(frictionapi VERSION 1.0.0 LANGUAGES CXX)

set(FRICTION_API_DBUS_ID "graphics.friction.api" CACHE STRING "Friction Api Identifier")
set(FRICTION_API_DBUS_PATH "/friction/api" CACHE STRING "Friction Api D-Bus Path")
set(FRICTION_API_SOCKET "local:graphics.friction.api" CACHE STRING "Friction Api Socket")

add_definitions(-DFRICTION_API_DBUS_ID="${FRICTION_API_DBUS_ID}")
add_definitions(-DFRICTION_API_DBUS_PATH="${FRICTION_API_DBUS_PATH}")
add_definitions(-DFRICTION_API_SOCKET="${FRICTION_API_SOCKET}")

option(USE_QT6 "Build using Qt 6" OFF) # untested

if(USE_QT6)
    set(QT_VERSION_MAJOR 6)
else()
    set(QT_VERSION_MAJOR 5)
endif()
message(STATUS "Friction API using Qt ${QT_VERSION_MAJOR}")

set(CMAKE_AUTOMOC ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Core RemoteObjects)

if(UNIX AND NOT APPLE)
    find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS DBus)
    set(HAS_DBUS TRUE)
    add_definitions(-DFRICTION_HAS_DBUS)
endif()

if(CMAKE_BUILD_TYPE MATCHES "^(release|Release|RELEASE)$")
    add_definitions(-DQT_NO_DEBUG_OUTPUT)
else()
    add_definitions(-DQT_MESSAGELOGCONTEXT)
endif()

set(REP_FILE "${CMAKE_CURRENT_SOURCE_DIR}/include/api.rep")

if(USE_QT6)
    qt6_re_generate_source(${REP_FILE})
    qt6_re_generate_replica("${REP_FILE}")
else()
    qt5_generate_repc(API_SOURCES "${REP_FILE}" SOURCE)
    qt5_generate_repc(API_REPLICA "${REP_FILE}" REPLICA)
endif()

set(API_HEADERS include/api_global.h include/api_dbus.h include/api.h)

add_library(${PROJECT_NAME} SHARED src/api.cpp src/api_dbus.cpp ${API_HEADERS} ${API_SOURCES} ${REP_FILE})

target_include_directories(${PROJECT_NAME} PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/friction/api>
)

set_target_properties(${PROJECT_NAME} PROPERTIES VERSION ${PROJECT_VERSION})
set_target_properties(${PROJECT_NAME} PROPERTIES SOVERSION ${PROJECT_VERSION_MAJOR})

target_compile_definitions(${PROJECT_NAME} PRIVATE FRICTION_API_LIBRARY)
target_link_libraries(${PROJECT_NAME} PRIVATE
    Qt${QT_VERSION_MAJOR}::Core
    Qt${QT_VERSION_MAJOR}::RemoteObjects
)

if(HAS_DBUS)
    target_link_libraries(${PROJECT_NAME} PRIVATE Qt${QT_VERSION_MAJOR}::DBus)
endif()

add_executable(${PROJECT_NAME}-server examples/server.cpp)
target_link_libraries(${PROJECT_NAME}-server PRIVATE
    ${PROJECT_NAME}
    Qt${QT_VERSION_MAJOR}::Core
    Qt${QT_VERSION_MAJOR}::RemoteObjects
)

if(HAS_DBUS)
    target_link_libraries(${PROJECT_NAME}-server PRIVATE Qt${QT_VERSION_MAJOR}::DBus)
endif()

add_executable(${PROJECT_NAME}-client examples/client.cpp ${API_REPLICA})
target_link_libraries(${PROJECT_NAME}-client PRIVATE
    Qt${QT_VERSION_MAJOR}::Core
    Qt${QT_VERSION_MAJOR}::RemoteObjects
)

target_include_directories(${PROJECT_NAME}-client PRIVATE ${CMAKE_CURRENT_BINARY_DIR})

if(UNIX AND NOT APPLE)
    include(GNUInstallDirs)

    install(TARGETS ${PROJECT_NAME} EXPORT FrictionApiTargets
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    )

    install(FILES
        include/api.h
        include/api_global.h
        DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/friction/api
    )

    if(HAS_DBUS)
        install(FILES include/api_dbus.h DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/friction/api)
    endif()

    install(FILES
        ${CMAKE_CURRENT_BINARY_DIR}/rep_api_source.h
        ${CMAKE_CURRENT_BINARY_DIR}/rep_api_replica.h
        DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/friction/api
    )

    install(FILES include/api.rep DESTINATION ${CMAKE_INSTALL_DATADIR}/friction/api)

    file(WRITE "${CMAKE_CURRENT_BINARY_DIR}/FrictionApiConfig.cmake"
    "include(CMakeFindDependencyMacro)
    find_dependency(Qt${QT_VERSION_MAJOR}RemoteObjects)
    if(UNIX AND NOT APPLE)
        find_dependency(Qt${QT_VERSION_MAJOR}DBus)
    endif()
    include(\"\${CMAKE_CURRENT_LIST_DIR}/FrictionApiTargets.cmake\")
    ")

    install(FILES "${CMAKE_CURRENT_BINARY_DIR}/FrictionApiConfig.cmake"
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/FrictionApi
    )

    install(EXPORT FrictionApiTargets
        FILE FrictionApiTargets.cmake
        NAMESPACE Friction::
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/FrictionApi
    )
endif()
