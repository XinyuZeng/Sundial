CC=g++
CFLAGS=-Wall -g -std=c++11

all: server client

server: message.o server.o 
	$(CXX) $^ $(CFLAGS) -o $@

client: message.o client.o 
	$(CXX) $^ $(CFLAGS) -o $@
clean:
	rm server client *.o
