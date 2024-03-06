import socket

HOST = "127.0.0.1"  # The server's hostname or IP address
PORT = 9877  #9876 was taken.

with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
    s.connect((HOST, PORT))
    s.sendall(b"Hello, world")
    data = s.recv(1024)

print('\nZoltar says:\n' + data.decode())