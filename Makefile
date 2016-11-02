CC=gcc
CFLAGS=-Wall -Werror -g -pthread
default: clean libstats.so stats_server stats_client

stats_server: stats_server.c
	$(CC) -o stats_server $(CFLAGS) stats_server.c libstats.so -lrt -lstats -L.

stats_client: stats_client.c
	$(CC) -o stats_client $(CFLAGS) stats_client.c libstats.so -lrt -lstats -L.

libstats.so: stats_server_util.o
	$(CC) -shared -o libstats.so stats_server_util.o

#stats.o: stats.c 
#	$(CC) -c -fpic $(CFLAGS) stats.c -o stats.o

stats_server_util.o: stats_server_util.c stats_server.h
	$(CC) -c -fpic $(CFLAGS) stats_server_util.c -o stats_server_util.o

clean:
	$(RM) stats_server stats_client statss.o stats_server_util.o libstats.so
