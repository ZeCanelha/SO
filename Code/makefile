# Makefile 
# Complier
CC	=	gcc
# Flags used ( to be completed along )
CFLAGS	=	-Wall		-g 

HEADERS = config.h	ServerHttp.h statistics.h
OBJECTS = config.o	ServerHttp.o functions.o statistics.c

default:	main

%.o: %.c $(HEADERS)
	$(CC) $(CFLAGS) -c $< -o $@

main: $(OBJECTS)
	$(CC) $(OBJECTS)  -o $@ -D_REENTRANT -lpthread

clean:
	-rm -f $(OBJECTS)
	-rm -f main
