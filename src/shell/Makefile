CFLAGS = -Wall -g -std=c11

OBJS = mysh.o shell_utils.o child_process.o string_utils.o

all: mysh

mysh: $(OBJS)
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

clean: 
	rm -f *.o *~ mysh

.PHONY: all clean

