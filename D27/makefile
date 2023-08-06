SRC = server.c client.c server_v2.c
.PHONY: $(SRC)
all: $(SRC)
$(SRC):
#	gcc -o $(basename $@) $@ -lpthread -Wall -O0 -DDEBUG=1
	gcc -o $(basename $@) $@ -lpthread -Wall -O0

