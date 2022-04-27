import socket
import select

client_handlers = {}


class Server:
    def __init__(self, host='localhost', port=80):
        self.host, self.port = host, port
        self.socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        self.socket.setblocking(False)
        self.socket.bind((host, port))
        self.socket.listen(5)


class ClientHandler:
    blocksize = 2048

    def __init__(self, server, client_socket, client_address):
        self.response = None
        self.server = server
        self.client_address = client_address
        self.client_socket = client_socket
        self.client_socket.setblocking(0)
        self.host = socket.getfqdn(client_address[0])
        self.incoming = ''  # receives incoming data
        self.writable = False
        self.close_when_done = True

    def handle_error(self):
        self.close()

    def handle_read(self):
        """Reads the data received"""
        try:
            buff = self.client_socket.recv(1024)
            if not buff:  # the connection is closed
                self.close()
            self.incoming += buff  # .write(buff)
            self.process_incoming()
        except socket.error:
            self.close()

    def process_incoming(self):
        """Test if request is complete ; if so, build the response
        and set self.writable to True"""
        if not self.request_complete():
            return
        self.response = self.make_response()
        self.writable = True

    def request_complete(self):
        return True

    def make_response(self):
        return ["xxx"]

    def handle_write(self):
        buff = ''
        while self.response and not buff:
            if isinstance(self.response[0], str):
                buff = self.response.pop(0)
            else:
                buff = self.response[0].read(self.blocksize)
                if not buff:
                    self.response.pop(0)
        if buff:
            try:
                self.client_socket.sendall(buff)
            except socket.error:
                self.close()
            if self.response:
                return
        if self.close_when_done:
            self.close()  # close socket
        else:
            self.writable = False
            self.incoming = ''

    def close(self):
        del client_handlers[self.client_socket]
        self.client_socket.close()


class LengthSepBody(ClientHandler):

    def __init__(self, server, client_socket, client_address):
        super().__init__(server, client_socket, client_address)
        self.msg_body = None

    def request_complete(self):
        recv = self.incoming.split('\n', 1)
        if len(recv) == 1 or len(recv[1]) != int(recv[0]):
            return False
        self.msg_body = recv[1]
        return True

    def make_response(self):
        return [self.msg_body]


def loop(server, handler, timeout=30):
    while True:
        k = client_handlers.keys()
        w = [cl for cl in client_handlers if client_handlers[cl].writable]
        r, w, e = select.select(k + [server.socket], w, k, timeout)
        for e_socket in e:
            client_handlers[e_socket].handle_error()
        for r_socket in r:
            if r_socket is server.socket:
                try:
                    client_socket, client_address = server.socket.accept()
                    client_handlers[client_socket] = handler(server,
                                                             client_socket, client_address)
                except socket.error:
                    pass
            else:
                client_handlers[r_socket].handle_read()
        w = set(w) & set(client_handlers.keys())  # remove deleted sockets
        for w_socket in w:
            client_handlers[w_socket].handle_write()
