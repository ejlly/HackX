First, we code our dns poisoning (inside the poison function). The code is multithreaded so that we can both poison a DHCP server and send spoofed packets to users.

The dns poisoning simply spams a DHCP server with bogus requests (as shown in the screenshot_poison.png)

We recover DHCPOFFER messages sent from this server as answer to our DHCPDISCOVER poisons, and we remember the IP adresses taht we can use in a stack of ips (in a struct ips that I coded myself)



In the mean time, we sniff all DHCP packets.

We answer DHCPDISCOVER with our own DHCPOFFER and DHCPREQUEST with our own DHCPACK

We are then able to put the ip address of a controlled dns server, any kind we want.

Then, on this dns server that we control, we configure bind9, so that a request to facebook.com returns the IP adress of the machine that hosts our hacker_page (on my machine, it is 0.0.0.0, in the python script)
