###################################################################################################
# Copyright (C) 2020 Michele Bertoni - All Rights Reserved                                        #
# You may use, distribute and modify this code under the terms of the CC BY-NC-SA 3.0 license.    #
# You can find a copy of the license inside the LICENSE file you received with this code          #
# (https://github.com/michele-bertoni/Printy-McPrintface/blob/master/LICENSE)                     #
# or on the website of CreativeCommons (https://creativecommons.org/licenses/by-nc-sa/3.0/)       #
###################################################################################################

import socket
import time

class SocketLineProtocol:
    def __init__(self, soc):
        self.socket = soc
        self.buffer = b''
        self.NEWLINE = b'\r'

    def write(self, msg):
        msg = msg.strip()
        msg += '\r'
        self.socket.sendall(msg.encode())

    def read_lines(self):
        lines = []
        while self.NEWLINE not in self.buffer:
            try:
                d = self.socket.recv(1024)
            except OSError as exc:
                if str(exc)!='timed out' and '[Errno 115]' not in str(exc):
                    raise exc
                return lines
            if not d:
                return lines
            self.buffer = self.buffer + d

        while self.NEWLINE in self.buffer:
            i = self.buffer.find(self.NEWLINE)
            line = self.buffer[:i]
            self.buffer = self.buffer[i:].lstrip()
            lines.append(line.decode("utf-8"))
        return lines


class Connection:
    __DISCONNECTED = 0
    __IGNORE = 1
    __CONNECTED = 2

    def __connect(self):
        try:
            print("Trying to connect to {}:{}".format(self.address, self.port), flush=True)
            self.__socket = socket.create_connection((self.address, self.port), self.timeout)
            self.__state = self.__IGNORE
            self.__connected_time = time.time()
            print("Connected, starting ignore period ({} seconds)".format(self.ignore_period), flush=True)
        except OSError as exc:
            print(exc, flush=True)
            self.__connected_time = 0.0
        return []

    def __ignore(self):
        if self.__connected_time>0.0 and time.time()>=self.__connected_time+self.ignore_period:
            self.__lineProtocol = SocketLineProtocol(self.__socket)
            self.__state = self.__CONNECTED
            print("Connection ready, starting read/write process", flush=True)
        return []

    def __read_write(self):
        try:
            while len(self.__out_queue)>0:
                self.__lineProtocol.write(self.__out_queue[0])
                self.__out_queue.pop(0)
            return self.__lineProtocol.read_lines()
        except OSError as exc:
            print(exc, flush=True)
            self.__state = self.__DISCONNECTED
            self.__connected_time = 0.0
            self.__socket = None
            self.__lineProtocol = None
            print("Socket disconnected. Attempting to reconnect...", flush=True)
            return []

    __operations = {
        __DISCONNECTED: __connect,
        __IGNORE:       __ignore,
        __CONNECTED:    __read_write
    }

    __out_message_format = {
        'DUET': 'M117 {}'
    }

    def __init__(self, address, port, host_type='DUET', timeout=0.1, ignore_period=0.0):
        self.address = address
        self.port = port
        self.host_type = host_type
        self.timeout = timeout
        self.ignore_period = ignore_period

        self.__state = self.__DISCONNECTED
        self.__socket = None
        self.__lineProtocol = None
        self.__out_queue = []
        self.__connected_time = 0.0

    def handle_connection(self):
        return self.__operations[self.__state](self)

    def queue_message(self, out_message=''):
        if len(out_message) > 0:
            self.__out_queue.append(self.__out_message_format.get(self.host_type, "{}").format(out_message))

    def set_timeout(self, timeout=.1):
        self.timeout = timeout
        self.__socket.settimeout(timeout)
