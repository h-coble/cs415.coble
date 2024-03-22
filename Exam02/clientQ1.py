import socket

HOST = "127.0.0.1"  # The server's hostname or IP address
PORT = 5999  


while True:
    msg = input("Enter your echo message: ")
    with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
        s.connect((HOST, PORT))
        s.sendall(msg.encode())
        data = s.recv(1024)
        print(data.decode())