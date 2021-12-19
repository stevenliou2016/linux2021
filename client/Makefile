client: client.o rio.o
	gcc -g client.o rio.o -o client

client.o: client.c
	gcc -g -c client.c -g -o client.o

rio.o: rio.c
	gcc -g -c rio.c -o rio.o

.PHONY: clean

clean:
	rm client client.o rio.o
