import socket
import threading

def server():
    HOST = "127.0.0.1"  # Standard loopback interface address (localhost)
    PORT = 5999  

    with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
        s.bind((HOST, PORT))
        while True:
            s.listen()
            conn, addr = s.accept()
            with conn:
                print("SERVER:")
                print(f"Connected by {addr}")
                while True:
                    data = conn.recv(1024)
                    if not data or data.decode() == "exit":
                        conn.close()
                        return
                    conn.send(data)
                    conn.close()
                    break

def client():
    HOST = "127.0.0.1"  # The server's hostname or IP address
    PORT = 5999  
    msg = "default"
    while msg != "exit":
        print("CLIENT:")
        msg = input("Enter your echo message: ")
        if msg == "exit":
            print("Exiting")
        with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
            s.connect((HOST, PORT))
            s.sendall(msg.encode())
            data = s.recv(1024)
            print("CLIENT:")
            print(data.decode())


def main():
    t1 = threading.Thread(target=server)
    t1.start()
    client()
    t1.join()

print("Enter \"exit\" to break the loop.")
main()