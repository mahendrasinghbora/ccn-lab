import socket
import select

# set the hostname (WLAN IP Address)
host = "192.168.0.105"

# set the port number
port = 7000

client_socket = socket.socket(family=socket.AF_INET, type=socket.SOCK_DGRAM)
message = ""

print("-" * 60)
print("UDP Client (Connected to {})".format(host))
print("-" * 60)
print("-> Type 'exit' to terminate the connection")
print("-" * 60)

while True:
    message = input("-> UDP Client: ")
    # send a message to the server
    client_socket.sendto(message.encode(), (host, port))

    client_socket.setblocking(False)

    ready = select.select([client_socket], [], [], 2)

    # packet lost (server did not reply)
    if not ready[0]:
        continue

    # receive data from the server
    data, address = client_socket.recvfrom(1024)
    print("-> UDP Server: {}".format(data.decode()))

    # terminate the connection
    if message.lower() == "exit":
        break

print("-" * 60)
