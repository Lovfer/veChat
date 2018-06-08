.PHONY:clean all
CC=g++
CFLAGS=-Wall -g
BIN=veServer
all:$(BIN)
%.o:%.cpp
	$(CC) $(CFLAGS) -c $< $@
clean:
	rm -rf *.o $(BIN)	
