import socket

# get the hostname
host = socket.gethostname()

# set the port number
port = 7000

server = socket.socket(family=socket.AF_INET, type=socket.SOCK_DGRAM)

# bind the host address and the port together
server.bind((host, port))

print("-" * 60)
print("Server (Running on {} [{}])".format(host, socket.gethostbyname(host)))
print("-" * 60)

while True:
    # receive data from the client (maximum size 1024 bytes)
    message, address = server.recvfrom(1024)

    print("-> Client [{}]: {}".format(address, message.decode()))

    if message.decode().lower() == "exit":
        server.sendto("Goodbye! Hope to see you again.".encode(), address)
        print("-" * 60)
        continue

    # send reply to the client
    data = input("-> Server: ")
    server.sendto(data.encode(), address)
