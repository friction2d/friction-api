# Part of Friction <https://friction.graphics>
# SPDX-FileCopyrightText: Copyright (c) Ole-André Rodlie and contributors
# SPDX-License-Identifier: BSD-3-Clause

cmake_minimum_required(VERSION 3.16)

project(frictionapi VERSION 1.0.0 LANGUAGES CXX)

set(FRICTION_API_ID "graphics.friction.api" CACHE STRING "Friction Api identifier")
set(FRICTION_API_DBUS_PATH "/friction/api" CACHE STRING "Friction Api DBus path")
set(FRICTION_API_SOCKET "local:friction_api" CACHE STRING "Friction Api Host")

add_definitions(-DFRICTION_API_ID="${FRICTION_API_ID}")
add_definitions(-DFRICTION_API_DBUS_PATH="${FRICTION_API_DBUS_PATH}")
add_definitions(-DFRICTION_API_SOCKET="${FRICTION_API_HOST}")

option(BUILD_TESTS "Build the test server and client" ON)
option(USE_QT6 "Build using Qt 6" OFF)

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

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)
include_directories(${CMAKE_CURRENT_BINARY_DIR})

set(REP_FILE "${CMAKE_CURRENT_SOURCE_DIR}/include/api.rep")

if(USE_QT6)
    qt6_re_generate_source(${REP_FILE})
else()
    qt5_generate_repc(API_SOURCES "${REP_FILE}" SOURCE)
endif()

set(API_HEADERS include/api_global.h include/api.h)

if(HAS_DBUS)
    list(APPEND API_HEADERS include/api_dbus.h)
endif()

add_library(${PROJECT_NAME} SHARED
    ${API_HEADERS}
    src/api.cpp
    ${API_SOURCES}
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

if(BUILD_TESTS)
    if(USE_QT6)
        qt6_re_generate_replica("${REP_FILE}")
    else()
        qt5_generate_repc(API_TEST_REPLICA "${REP_FILE}" REPLICA)
    endif()

    add_executable(${PROJECT_NAME}-server tests/server_main.cpp)
    target_link_libraries(${PROJECT_NAME}-server PRIVATE
        ${PROJECT_NAME}
        Qt${QT_VERSION_MAJOR}::Core
        Qt${QT_VERSION_MAJOR}::RemoteObjects
    )

    if(HAS_DBUS)
        target_link_libraries(${PROJECT_NAME}-server PRIVATE Qt${QT_VERSION_MAJOR}::DBus)
    endif()

    add_executable(${PROJECT_NAME}-client tests/client_main.cpp ${API_TEST_REPLICA})
    target_link_libraries(${PROJECT_NAME}-client PRIVATE
        Qt${QT_VERSION_MAJOR}::Core
        Qt${QT_VERSION_MAJOR}::RemoteObjects
    )

    target_include_directories(${PROJECT_NAME}-client PRIVATE ${CMAKE_CURRENT_BINARY_DIR})
endif()
