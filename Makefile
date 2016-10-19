CC=gcc
CFLAGS=-Wall -g

stat_server: stat_server.c
	$(CC) -o stat_server $(CFLAGS) stat_server.c stat_server.h stat_server_util.c

clean:
	$(RM) stat_server
