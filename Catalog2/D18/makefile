.PHONY: server client client_service
all: client server client_service
server:
	-rm server
	gcc -o server server.c -lrt -lpthread
client:
	-rm client
	gcc -o client client.c -lrt -lpthread
client_service:
	-rm client_service
	gcc -o client_service client_service.c -lrt -lpthread