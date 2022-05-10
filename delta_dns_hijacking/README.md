Basically, we sniff all dns packes with pcap, choosing the right interface.

Then, we forge a packet sending a dns packet with our own fake ip "FALSE IP" :
	For each question in the dns packet, we fill a answer containing a fake ip.

On the screenshot, we can see that we answered the dns request (packet 200) requesting www.google.com
with ip 1.2.3.4 (packet 205) that we created ourselves

We put the our own ip adress, open our port 53, and modify our dns forward zone in /etc/bind/named.conf.local so that facebook.com is redirected to ip 0.0.0.0


(Here we will use dig to prove that, thus putting in /etc/resolv.conf : nameserver 192.168.0.10)

Our (temporary) iptables rules : 
Chain INPUT (policy DROP)
target     prot opt source               destination         
ACCEPT     tcp  --  anywhere             anywhere             tcp dpt:http
ACCEPT     tcp  --  anywhere             anywhere             tcp dpt:ssh
ACCEPT     tcp  --  anywhere             anywhere             tcp dpt:imap2
ACCEPT     icmp --  anywhere             anywhere            
ACCEPT     all  --  anywhere             anywhere            
ACCEPT     all  --  anywhere             anywhere             state RELATED,ESTABLISHED
ACCEPT     udp  --  anywhere             anywhere             udp dpt:domain
ACCEPT     udp  --  anywhere             anywhere             udp dpt:domain

Chain FORWARD (policy DROP)
target     prot opt source               destination         

Chain OUTPUT (policy DROP)
target     prot opt source               destination         
ACCEPT     all  --  anywhere             anywhere             ! ctstate INVALID
ACCEPT     udp  --  anywhere             anywhere             udp spt:domain


We opened port 53 and allowed our own dns server to send data on port 53


Then we modify /etc/bind/named.conf.local to add :

zone "facebook.com" {
  type master;
  file "/etc/bind/forward.facebook.local.db";
};

and we create file : /etc/bind/forward.facebook.local.db with contents :

;
; BIND data file for local loopback interface
;
$TTL	604800
@	IN	SOA	facebook.com. root.facebook.com. (
			      2		; Serial
			 604800		; Refresh
			  86400		; Retry
			2419200		; Expire
			 604800 )	; Negative Cache TTL
;
@	IN	NS	localhost.
@	IN	A	0.0.0.0
;@	IN	AAAA	::1


In the end, after restarting the server, when we do a "dig facebook.com" :

; <<>> DiG 9.16.1-Ubuntu <<>> facebook.com
;; global options: +cmd
;; Got answer:
;; ->>HEADER<<- opcode: QUERY, status: NOERROR, id: 43504
;; flags: qr aa rd ra; QUERY: 1, ANSWER: 1, AUTHORITY: 0, ADDITIONAL: 1

;; OPT PSEUDOSECTION:
; EDNS: version: 0, flags:; udp: 4096
; COOKIE: 2654f951c469f60e01000000627a6d12aeee8499698c8ea0 (good)
;; QUESTION SECTION:
;facebook.com.			IN	A

;; ANSWER SECTION:
facebook.com.		604800	IN	A	0.0.0.0

;; Query time: 4 msec
;; SERVER: 127.0.0.1#53(127.0.0.1)
;; WHEN: mar. mai 10 15:48:02 CEST 2022
;; MSG SIZE  rcvd: 85


We are redirected to adress 0.0.0.0
