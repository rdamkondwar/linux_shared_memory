CC=gcc
CFLAGS=-Wall -Werror -g -pthread
default: clean stat_server stat_client

stat_server: stat_server.c
	$(CC) -o stat_server $(CFLAGS) stat_server.c stat_server.h stat_server_util.c 

stat_client: stat_client.c
	$(CC) -o stat_client $(CFLAGS) stat_client.c stat_server.h stat_server_util.c

# libstats.so: stat_server_util.o
# 	$(CC) -shared -o libstats.so stat_server_util.o

# #stats.o: stats.c 
# #	$(CC) -c -fpic $(CFLAGS) stats.c -o stats.o

# stat_server_util.o: stat_server_util.c stat_server.h#
# 	$(CC) -c -fpic $(CFLAGS) stat_server_util.c -o stat_server_util.o

clean:
	$(RM) stat_server stat_client stats.o stat_server_util.o libstats.so
