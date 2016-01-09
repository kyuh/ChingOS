BINARIES = outerr sleepycat cattysleep shell_tests

CFLAGS = -Wall


all: $(BINARIES)


outerr: outerr.o
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

sleepycat: sleepycat.o
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

cattysleep: cattysleep.o
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

shell_tests: shell_tests.o
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)


clean:
	rm -f *.o $(BINARIES) *~


.PHONY: all clean

