import logging
import threading
import time
import subprocess
import os
import signal
from queue import Queue
import random
logging.getLogger().setLevel(logging.INFO)


CLIENT_PATH = "cmake-build-debug/pcc_client"
SERVER_PATH = "cmake-build-debug/pcc_server"
PORT = "12345"
IP = "127.0.0.1"

def server_thread_function(e, q):
    logging.info(f"Server Thread is starting {time.ctime()}")
    with subprocess.Popen([SERVER_PATH, PORT], stdout = subprocess.PIPE) as proc:
        while not e.isSet():
            time.sleep(0.1)
        proc.send_signal(signal.SIGINT)
        logging.info(f"Got final data from server")
        q.put(proc.stdout.read().decode("utf-8"))
    logging.info(f"Server stopped running {time.ctime()}")
    return proc

def client_thread_function(file_path, printable):
    #logging.info(f"Client Thread is starting with file {file_path},  {time.ctime()}")
    proc = subprocess.run([CLIENT_PATH, IP, PORT, file_path], stdout=subprocess.PIPE)
    out= proc.stdout.decode("utf-8")
    #logging.info(out)
    client_printable = int(out.split(":")[1][1:])
    if client_printable != printable:
        logging.error(f"Wrong in client size reporting! reported printable: {client_printable}, real printable: {printable}")
    #logging.info(f"Client with file {file_path} stopped running {time.ctime()}")
    return proc

def is_printable(char):
    if 32 <= char <= 126:
        return 1
    return 0

def create_client_dict():
    client_dict = {}
    for c in range(32, 127):
        client_dict[chr(c)] = 0
    return client_dict

def update_client_dict(client_dict, data):
    for c in data:
        if is_printable(c) == 1:
            client_dict[chr(c)] += 1


def get_chr_amount(line):
    s = line.split(" ")
    if s[3] == ":":
        return int(s[4])
    if s[2] == ":":
        return int(s[3])

def get_server_dict(server_data):
    lines = server_data.split("\n")
    chars_dict = { chr(i + 32) : get_chr_amount(line) for i, line in enumerate(lines[:-1])}
    return chars_dict

def main():
    #Creating thread-communication helpers
    e = threading.Event()
    q = Queue()

    #Creating and starting server_thread
    server_thread = threading.Thread(target = server_thread_function, args = (e,q))
    server_thread.start()

    #Creating client files
    client_dict = create_client_dict()
    logging.info("Creating a lot of clients with random file sizes")
    for i in range(1000):
        rand_bytes = os.urandom(random.randint(100, 10000))
        update_client_dict(client_dict, rand_bytes)
        printable_char = sum([is_printable(x) for x in rand_bytes])
        #logging.info(f"Created a randfile of length {len(rand_bytes)} with {printable_char} printable chars")
        with open("randfile0", "wb") as f:
            f.write(rand_bytes)
        client_thread = threading.Thread(target = client_thread_function, args = ("randfile0", printable_char))
        client_thread.start()
        client_thread.join()
    e.set()
    server_thread.join()
    server_dict = get_server_dict(q.get())
    flag = True
    for c in range(32, 127):
        #logging.info(f"{chr(c)}: client_dict - {client_dict[chr(c)]}, server_dict - {server_dict[chr(c)]}")
        if client_dict[chr(c)] != server_dict[chr(c)]:
            flag = False
            logging.error(f"WRONG!!!! char = {chr(c)}, client_dict[{chr(c)}] = {client_dict[chr(c)]}, server_dict[{chr(c)}] = {server_dict[chr(c)]}")
    logging.info(f"Finished running, flag is {flag}")
    if flag:
        logging.info("YAYYYYYY")
    else:
        logging.info("NOOOOO")



if __name__ == '__main__':
    main()
