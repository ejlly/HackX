Basically, we sniff all dns packes with pcap, choosing the right interface.

Then, we forge a packet sending a dns packet with our own fake ip "FALSE IP" :
	For each question in the dns packet, we fill a answer containing a fake ip.

On the screenshot, we can see that we answered the dns request (packet 200) requesting www.google.com
with ip 1.2.3.4 (packet 205) that we created ourselves
