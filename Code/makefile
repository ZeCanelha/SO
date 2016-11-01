# Makefile 
# Complier
CC	=	gcc
# Flags used ( to be completed along )
CFLAGS	=	-Wall	-g

HEADERS = config.h	ServerHttp.h
OBJECTS = config.o	ServerHttp.o functions.o

default:	main

%.o: %.c $(HEADERS)
	$(CC) $(CFLAGS) -c $< -o $@

main: $(OBJECTS)
	$(CC) $(OBJECTS) -o $@

clean:
	-rm -f $(OBJECTS)
	-rm -f main