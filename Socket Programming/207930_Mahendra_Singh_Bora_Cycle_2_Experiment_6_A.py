import socket

TCP_IP = socket.gethostname()
TCP_PORT = 5005

server = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
server.bind((TCP_IP, TCP_PORT))

# only one connection request can queue up
server.listen(1)

print("-" * 60)
print("TCP Server (Running on {} [{}])".format(TCP_IP, socket.gethostbyname(TCP_IP)))
print("-" * 60)

# set up the connection
client_socket, address = server.accept()

while True:
    # receive the data from the client
    data = client_socket.recv(1024)

    if not data:
        break

    print("Client [{}]:".format(address), data.decode())
    print("-" * 60)

    client_socket.send(data)  # echo (send back the client's message)

# terminate the connection
client_socket.close()
