build:
	clear
	clear
	make main
main: server client
	ls
client:client.o tcp.o login_protocol.o linklist.o queue.o web_protocol.o roomlist.o auction_protocol.o
	gcc -o client client.o tcp.o login_protocol.o linklist.o queue.o roomlist.o web_protocol.o auction_protocol.o
server:server.o tcp.o login_protocol.o linklist.o queue.o web_protocol.o roomlist.o user_database.o auction_protocol.o
	gcc -o server server.o tcp.o login_protocol.o linklist.o queue.o roomlist.o web_protocol.o user_database.o auction_protocol.o

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

web_protocol.o:web_protocol.c
	gcc -c web_protocol.c
auction_protocol.o:auction_protocol.c
	gcc -c auction_protocol.c
user_database.o:user_database.c
	gcc -c user_database.c
roomlist.o:roomlist.c
queue.o: queue.c
	gcc -c queue.c
roomlist.o: roomlist.c
	gcc -c roomlist.c
clear:
	rm *.o 
	rm client server
