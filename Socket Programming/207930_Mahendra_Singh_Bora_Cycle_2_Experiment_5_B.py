import socket
import select

# set the hostname
host = socket.gethostname()

# set the port number
port = 7000

client_socket = socket.socket(family=socket.AF_INET, type=socket.SOCK_DGRAM)
message = "Requesting for Data."

print("-" * 60)
print("UDP Client (Connected to {})".format(host))
print("-" * 60)

# send a message to the server
client_socket.sendto(message.encode(), (host, port))

while True:
    client_socket.setblocking(False)
    ready = select.select([client_socket], [], [], 10)

    # packet lost (server did not reply)
    if not ready[0]:
        continue

    # receive data from the server
    data, address = client_socket.recvfrom(1024)

    if data.decode() == "Message sent.":
        break

    print("-> UDP Server: {}".format(data.decode()))

print("-" * 60)
