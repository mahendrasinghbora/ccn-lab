import socket

fd_1, fd_2 = socket.socketpair()

server_socket = socket.fromfd(fd_1.fileno(), socket.AF_INET, socket.SOCK_STREAM)
server_socket.sendall("Hey, there!".encode())

server_socket = socket.fromfd(fd_1.fileno(), socket.AF_INET, socket.SOCK_STREAM)
server_socket.sendall("Hey, there!".encode())
