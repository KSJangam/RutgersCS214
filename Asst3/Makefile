all: client serve

client: DUMBclient.c
	gcc DUMBclient.c -o DUMBclient

serve: DUMBserver.c
	gcc -lpthread -pthread DUMBserver.c -o DUMBserver
