CC=g++34
DebugFlag=-g

all: server client

server: server.o libcslib.a
	$(CC) $(DebugFlag) -lm -o server server.o -pthread -L. -lcslib

server.o: server.cpp 
	$(CC) $(DebugFlag) -c server.cpp -pthread

client: client.o
	$(CC) $(DebugFlag) -lm -o client client.o -L. -lcslib

client.o: client.cpp
	$(CC) $(DebugFlag) -c client.cpp 

message.o: message.cpp message.h
	$(CC) $(DebugFlag) -c message.cpp

cerrors.o: cerrors.cpp cerrors.h
	$(CC) $(DebugFlag) -c cerrors.cpp

libcslib.a: message.o cerrors.o
	ar rcs libcslib.a message.o cerrors.o

libs: libcslib.a

clean:
	\rm -rf *.o *.a *.gch server client

