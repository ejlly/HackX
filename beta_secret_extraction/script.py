import sys
import requests
import time
import numpy as np

import random

#def main():
#    site = "https://superxwebdeveloper.tk/login"
#
#    requests.get(site)
#
#    username = "" #len = 6
#    password = ""
#    
#    N_test = 1000
#
#    all_chars = []
#
#    for i in range(1, 15):
#        username += "a"
#
#        chrono = []
#    
#        for _ in range(N_test):
#            chrono.append(time.perf_counter())
#            response = requests.post(site, data = {'username' : username, 'password' : password})
#            chrono[-1] = time.perf_counter() - chrono[-1]
#        
#        #print("**********************************\n", i+1, "\n", chrono, "\n")
#        
#        chrono.sort()
#
#        all_chars.append(chrono[len(chrono)//2])
#
#        print("FOR :", i+1, ", MEDIAN IS : ", chrono[len(chrono)//2])
#    
#    
#    print(np.argmax(all_chars) + 1, '\n', all_chars)
#
#
#    print(response.text)

def main():
    id = 643732
    
    time_to_send = int(time.time())
    
    for i in range(5):
        random.seed(int(time.time()) + i + id)
        solution = random.sample(list(range(1,50)), 5)
        solution.sort()

        print(solution)

    i = 13
    
    random.seed(int(time.time()) + i + id)
    solution = random.sample(list(range(1,50)), 5)
    solution.sort()

    print(solution)

    while time_to_send + i - int(time.time()) >= 0:
        print(time_to_send + i - int(time.time()), solution)
    

main()
