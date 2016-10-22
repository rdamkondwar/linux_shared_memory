CC=gcc
CFLAGS=-Wall -g -pthread

stat_server: stat_server.c
	$(CC) -o stat_server $(CFLAGS) stat_server.c stat_server.h stat_server_util.c stats.c

stat_client: stat_client.c
	$(CC) -o stat_client $(CFLAGS) stat_client.c stat_server.h stat_server_util.c stats.c

clean:
	$(RM) stat_server
