#The server thread doesn't like to join back to the main thread.
#I can't figure out why this is the case; the global bool for loop control
#should be stopping the loops.
#This wouldn't be an issue if implemented into 2 separate files, client and server,
#but I chose to implement threads due to the file limit on the Canvas submission.
#
#As always, please be gentle when grading.
#-HC


import socket, time, threading

keep_going = True

server_time = 10
client_time = 0
server_mx = threading.Lock()
client_mx = threading.Lock()

def server_clock():
    global server_time, keep_going, server_mx
    while keep_going:
        server_mx.acquire()
        server_time = server_time + 1
        server_mx.release()
        time.sleep(1)
    

def client_clock():
    global client_time, keep_going, client_mx
    while keep_going:
        client_mx.acquire()
        client_time = client_time + 1
        client_mx.release()
        time.sleep(1)

def server():
    HOST = "127.0.0.1"  # Standard loopback interface address (localhost)
    PORT = 5970  
    global server_time, keep_going, server_mx
    s_clock_thr = threading.Thread(target=server_clock)
    s_clock_thr.start()
    with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
        s.bind((HOST, PORT))
        while keep_going:
            s.listen()
            conn, addr = s.accept()
            with conn:
                print(f"SERVER: Connected by {addr}")
                while keep_going:
                    data = conn.recv(1024)
                    if not data:
                        break
                    #Get and Compare Timestamp
                    secs = int(data.decode())

                    #Lock the timestamp val
                    server_mx.acquire()
                    if server_time - secs > 5 or server_time - secs < 5:
                        conn.send(str("X" + str(server_time)).encode())
                    else:
                        conn.send(str(str(server_time)).encode())
                    #unlock
                    server_mx.release()
                    conn.close()
                    break
    keep_going = False
    s_clock_thr.join()

def client():
    global client_time, keep_going, client_mx
    HOST = "127.0.0.1"  # The server's hostname or IP address
    PORT = 5970  
    c_clock_thr = threading.Thread(target=client_clock)
    c_clock_thr.start()
    input("Enter any value to check clock: ")
    with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
        s.connect((HOST, PORT))

        client_mx.acquire()
        s.sendall(str(client_time).encode())
        client_mx.release()

        data = s.recv(1024)
        
        ts = data.decode()
        if ts[0] == "X":
            print(ts)
            client_mx.acquire()
            client_time = int(ts[1:])
            client_mx.release()
        print("Clock: " + ts)
    keep_going = False
    c_clock_thr.join()

def main():
    
    t1 = threading.Thread(target=server)
    t1.start()
    client()
    t1.join()

main()