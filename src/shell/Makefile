CFLAGS = -Wall -g -std=c99

OBJS = mysh.o array_utils.o child_process.o shell_utils.o

all: mysh

mysh: $(OBJS)
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

clean: 
	rm -f *.o *~ mysh

.PHONY: all clean

