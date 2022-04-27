import socket

# set the hostname
host = socket.gethostname()

# set the port number
port = 7000

udp_server = socket.socket(family=socket.AF_INET, type=socket.SOCK_DGRAM)

# bind the host address and the port together
udp_server.bind((host, port))

print("-" * 60)
print("UDP Server (Running on {} [{}])".format(host, socket.gethostbyname(host)))
print("-" * 60)

while True:
    # receive data from the client
    message, address = udp_server.recvfrom(1024)

    print("-> Client [{}]: {}".format(address, message.decode()))

    # send reply to the client
    data = input("-> UDP Server: ")

    # if the data to be sent is greater that a byte, fragment it
    fragmented_data = [(data[i:i + 8]) for i in range(0, len(data), 8)]

    # send the fragmented data
    for i in range(len(fragmented_data)):
        udp_server.sendto(fragmented_data[i].encode(), address)

    udp_server.sendto("Message sent.".encode(), address)
    print("-" * 60)
