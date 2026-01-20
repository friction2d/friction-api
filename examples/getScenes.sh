#!/bin/bash
# Part of Friction <https://friction.graphics>
# SPDX-FileCopyrightText: Copyright (c) Ole-André Rodlie and contributors
# SPDX-License-Identifier: BSD-3-Clause

dbus-send --session --type=method_call --print-reply --dest=graphics.friction.api /friction/api graphics.friction.api.getScenes
