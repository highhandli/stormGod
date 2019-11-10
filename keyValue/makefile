server:server.o
	gcc -o server server.o -pthread
	cp -f server ../keyTest/
server.o:server.c
	gcc -c server.c

client:client.o
	gcc -o client client.o -pthread
	cp -f client ../keyTest/
client.o:client.c
	gcc -c client.c

clientBat:clientBat.o
	gcc -o clientBat clientBat.o -pthread
	cp -f clientBat ../keyTest/
clientBat.o:clientBat.c
	gcc -c clientBat.c
