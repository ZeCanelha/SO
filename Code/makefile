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
<<<<<<< HEAD
	$(CC) $(OBJECTS)  -o $@ -D_REENTRANT -lpthread
=======
	$(CC) $(OBJECTS) -lpthread -D_REENTRANT -o $@
>>>>>>> config_struct

clean:
	-rm -f $(OBJECTS)
	-rm -f main
