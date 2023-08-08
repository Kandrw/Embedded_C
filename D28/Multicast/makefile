SRC = client.c server.c
.PHONY: $(SRC)
all: $(SRC)
$(SRC):
	gcc -o $(basename $@) $@ -lpthread -Wall -O0

#	./client 192.168.28.154 239.0.0.1 4567
#	./server 192.168.28.102 239.0.0.1 4567
