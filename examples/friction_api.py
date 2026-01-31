# Friction API <https://friction.graphics>
# SPDX-FileCopyrightText: Copyright (c) Ole-André Rodlie and contributors
# SPDX-License-Identifier: BSD-3-Clause

import socket
import json
import uuid
import threading

class FrictionClient:
    def __init__(self, socket_path="/tmp/graphics.friction.api.bridge"):
        self.socket_path = socket_path
        self.sock = None
        self._callbacks = {}
        self._pending_requests = {}

    def connect(self):
        self.sock = socket.socket(socket.AF_UNIX, socket.SOCK_STREAM)
        self.sock.connect(self.socket_path)
        threading.Thread(target=self._listen, daemon=True).start()

    def _listen(self):
        buffer = ""
        while True:
            try:
                data = self.sock.recv(4096).decode('utf-8')
                if not data: break
                buffer += data
                while "\n" in buffer:
                    line, buffer = buffer.split("\n", 1)
                    payload = json.loads(line)

                    if payload.get("type") == "method_return":
                        self._pending_requests[payload.get("id")] = payload
                    elif payload.get("type") == "signal":
                        name = payload.get("name")
                        if name in self._callbacks:
                            self._callbacks[name](payload)
            except: break

    def on(self, signal, callback):
        self._callbacks[signal] = callback

    def call(self, method, params={}):
        req_id = str(uuid.uuid4())[:8]
        self.sock.sendall((json.dumps({"id": req_id, "method": method, "params": params}) + "\n").encode())

        while req_id not in self._pending_requests:
            pass 
        return self._pending_requests.pop(req_id).get("result")
