import socket

print("-" * 60)
print("Resolving Hostname into IP Address")
print("-" * 60)

# read the hostname from the user
hostname = input("Hostname: ")
print("-" * 60)

try:
    # get IP address from the hostname
    ip_address = socket.gethostbyname(hostname)

    # getfqdn(): to get the fully qualified domain name
    print("IP Address: {} ({})".format(ip_address, socket.getfqdn(ip_address)))
except socket.error:
    # hostname is not valid
    print("Please enter a valid hostname!")

print("-" * 60)
