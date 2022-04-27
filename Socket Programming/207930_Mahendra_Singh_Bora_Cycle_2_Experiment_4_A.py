import socket

# set the hostname (WLAN IP Address)
host = "192.168.0.105"

# set the port number
port = 7000

udp_server = socket.socket(family=socket.AF_INET, type=socket.SOCK_DGRAM)

# bind the host address and the port together
udp_server.bind((host, port))

print("-" * 60)
print("UDP Server (Running on {} [{}])".format(host, socket.gethostbyname(host)))
print("-" * 60)

while True:
    # receive data from the client (maximum size 1024 bytes)
    message, address = udp_server.recvfrom(1024)

    print("-> Client [{}]: {}".format(address, message.decode()))

    if message.decode().lower() == "exit":
        udp_server.sendto("Goodbye! Hope to see you again.".encode(), address)
        print("-" * 60)
        continue

    # send reply to the client
    data = input("-> UDP Server: ")
    udp_server.sendto(data.encode(), address)
