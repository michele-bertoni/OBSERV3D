###################################################################################################
# Copyright (C) 2020 Michele Bertoni - All Rights Reserved                                        #
# You may use, distribute and modify this code under the terms of the CC BY-NC-SA 3.0 license.    #
# You can find a copy of the license inside the LICENSE file you received with this code          #
# (https://github.com/michele-bertoni/Printy-McPrintface/blob/master/LICENSE)                     #
# or on the website of CreativeCommons (https://creativecommons.org/licenses/by-nc-sa/3.0/)       #
###################################################################################################

import socket

class SimpleLineProtocol:
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
            except Exception as sockException:
                if str(sockException) != 'timed out':
                    print(sockException, flush=True)
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
