.PHONY: client server
all: client server
client:
	gcc -o client client.c -lpthread -Wall -O0 -DPORT=54321 -DPORT_SERVER=4567 -DADDRESS='"192.168.56.101"' -DADDRESS_SERVER='"192.168.56.102"'
server:
	gcc -o server server.c -lpthread -Wall -O0 -DPORT=4567 -DADDRESS='"192.168.56.102"'
	