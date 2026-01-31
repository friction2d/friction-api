#!/usr/bin/env python3

# Friction API <https://friction.graphics>
# SPDX-FileCopyrightText: Copyright (c) Ole-André Rodlie and contributors
# SPDX-License-Identifier: BSD-3-Clause

from friction_api import FrictionClient
import time

client = FrictionClient()

client.on("message", lambda data: print(f"Message from Friction: {data['data']}"))
client.on("scenesChanged", lambda data: print(f"Friction Scenes Changed: {data['data']['scenes']}"))
client.on("hostStateChanged", lambda data: print(f"Friction Host Changed: {data['state']}"))

try:
    client.connect()

    scenes = client.call("getScenes")
    for s in scenes:
            print(f" - {s['id']}: {s['title']} ({s['width']}x{s['height']})")

    client.call("setCurrentScene", {"id": 1})

    while True:
        time.sleep(1)

except Exception as e:
    print(f"Error: {e}")
