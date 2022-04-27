import socket

# get the hostname
host = socket.gethostname()

# set the port number
port = 7000

client_socket = socket.socket(family=socket.AF_INET, type=socket.SOCK_DGRAM)
message = ""

print("-" * 60)
print("Client (Connected to {})".format(host))
print("-" * 60)
print("-> Type 'exit' to terminate the connection")
print("-" * 60)

while True:
    message = input("-> Client: ")
    # send a message to the server
    client_socket.sendto(message.encode(), (host, port))

    # receive data from the server
    data, address = client_socket.recvfrom(1024)
    print("-> Server: {}".format(data.decode()))

    # terminate the connection
    if message.lower() == "exit":
        break

print("-" * 60)
