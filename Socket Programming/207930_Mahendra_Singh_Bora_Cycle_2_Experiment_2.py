import socket
import re

hostname = "www.google.com"
port = 80

client = socket.socket(family=socket.AF_INET, type=socket.SOCK_STREAM)

# connect to the remote socket (google search)
client.connect((hostname, port))

print("-" * 60)
print("Communicating with Google Search Socket")
print("-" * 60)

# read the search query from the user and format it
query = re.sub(r"\s+", "+", input("Query: ").strip())
print("-" * 60)
print("Message Sent: " + "GET google.com/search?q={} HTTP/1.1".format(query))
print("-" * 60)

# message to be sent
message = bytes("GET google.com/search?q={} HTTP/1.1\n\n".format(query), "utf-8")

# send data to the socket
client.send(message)

# receive data from the socket
received_data = client.recv(65536).decode().split("\r\n")

print("Text Received")
print("-" * 60)

# print the received text on the console
for i in range(5):
    print(received_data[i])

print("-" * 60)
