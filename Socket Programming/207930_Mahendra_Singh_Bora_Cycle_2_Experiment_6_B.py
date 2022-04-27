import socket

TCP_IP = socket.gethostname()
TCP_PORT = 5005

print("-" * 60)
print("TCP Client (Connected to {})".format(TCP_IP))
print("-" * 60)

MESSAGE = input("-> TCP Client: ")
client = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
client.connect((TCP_IP, TCP_PORT))

# send the message
client.send(MESSAGE.encode())

data = client.recv(1024)
client.close()

print("-> TCP Server:", data.decode())
print("-" * 60)