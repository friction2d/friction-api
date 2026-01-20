#!/bin/bash
dbus-send --session --type=method_call --print-reply \
    --dest=graphics.friction.api \
    /friction/api \
    graphics.friction.api.getScenes

