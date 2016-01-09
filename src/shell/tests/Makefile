BINARIES = outerr sleepycat cattysleep shell_tests array_tests

CFLAGS = -Wall -std=c99


all: $(BINARIES)


outerr: outerr.o
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

sleepycat: sleepycat.o
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

cattysleep: cattysleep.o
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

shell_tests: shell_tests.o
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

array_tests: array_tests.o array_utils.o cmd_parse.o shell_utils.o
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)


clean:
	rm -f *.o $(BINARIES) *~


.PHONY: all clean

