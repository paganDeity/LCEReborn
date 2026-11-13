"""The LCEReborn debug protocol.

One JSON object per line in each direction. A request names a command and
carries an id; the reply carries the same id and either a result or an error.
This module is the only place that knows the wire format.
"""
import json
import socket

DEFAULT_HOST = "127.0.0.1"
DEFAULT_PORT = 25575


class ProtocolError(Exception):
    """The game refused a request or answered in a way this protocol does not allow."""


class Connection:
    def __init__(self, port=DEFAULT_PORT, host=DEFAULT_HOST, timeout=30.0):
        self._socket = socket.create_connection((host, port), timeout=timeout)
        self._reader = self._socket.makefile("rb")
        self._next_id = 1

    def request(self, command, **parameters):
        request_id = self._next_id
        self._next_id += 1
        message = {"id": request_id, "cmd": command}
        message.update(parameters)
        self._socket.sendall(json.dumps(message).encode("utf-8") + b"\n")
        line = self._reader.readline()
        if not line:
            raise ProtocolError("the game closed the connection")
        reply = json.loads(line.decode("utf-8"))
        if reply.get("id") != request_id:
            raise ProtocolError("reply %r does not answer request %r" % (reply.get("id"), request_id))
        if not reply.get("ok"):
            raise ProtocolError(reply.get("error", "the request failed"))
        return reply["result"]

    def close(self):
        self._reader.close()
        self._socket.close()

    def __enter__(self):
        return self

    def __exit__(self, kind, value, trace):
        self.close()


def connect(port=DEFAULT_PORT, timeout=30.0):
    return Connection(port=port, timeout=timeout)


def is_listening(port=DEFAULT_PORT, timeout=0.5):
    try:
        with Connection(port=port, timeout=timeout):
            return True
    except OSError:
        return False
