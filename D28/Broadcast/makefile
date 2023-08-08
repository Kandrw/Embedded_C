SRC = client.c server.c
.PHONY: $(SRC)
all: $(SRC)
$(SRC):
	gcc -o $(basename $@) $@ -lpthread -Wall -O0

#	./client 255.255.255.255 4567
#	./server 255.255.255.255 4567
