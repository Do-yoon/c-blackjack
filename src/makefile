all : server client

server : server.c dealer.c
	gcc -o server server.c dealer.c

client : client.c user.c
	gcc -o client client.c user.c
