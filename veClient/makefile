.PHONY:clean all
CC=g++
CFLAGS=-Wall -g
BIN=veClient
all:$(BIN)
%.o:%.cpp
	$(CC) $(CFLAGS) -c $< $@
clean:
	rm -rf *.o $(BIN)	
