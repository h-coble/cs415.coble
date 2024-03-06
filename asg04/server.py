#I could not for the life of me understand how to get the C++ sockets working.
#The code crashed my computer 4 separate times so I pivoted to python.
#I think I managed to fork-bomb myself at least two of those times.
import socket, fileinput
from random import randrange
fortunes = []
for line in fileinput.input('fortune.txt'):
    if line != '\n':
        fortunes.append(line)


HOST = "127.0.0.1"  # Standard loopback interface address (localhost)
PORT = 9877  #9876 was taken

with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
    s.bind((HOST, PORT))
    while True:
        s.listen()
        conn, addr = s.accept()
        with conn:
            print(f"Connected by {addr}")
            while True:
                data = conn.recv(1024)
                if not data:
                    break
                conn.send(str.encode(fortunes[randrange(len(fortunes))]))
                conn.close()
                break