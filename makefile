main: server client
	ls
client:client.o tcp.o login_protocol.o linklist.o
	gcc -o client client.o tcp.o login_protocol.o linklist.o
server:server.o tcp.o login_protocol.o linklist.o user_database.o
	gcc -o server server.o tcp.o login_protocol.o linklist.o user_database.o
tcp.o:tcp.c tcp.h
	gcc -c tcp.c
server.o:server.c
	gcc -c server.c
client.o:client.c
	gcc -c client.c
linklist.o:linklist.c
	gcc -c linklist.c
login_protocol.o:login_protocol.c
	gcc -c login_protocol.c
user_database.o:user_database.c
	gcc -c user_database.c
clear:
	rm *.o 
	rm client server
