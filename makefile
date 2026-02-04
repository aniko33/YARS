CFLAGS = -Wall -Wextra

all: target/yars

target/yars: target/main.o target/argparse.o target/socket.o target/term.o
	$(CC) $(CFLAGS) -o target/yars target/*.o

target/argparse.o: src/argparse/argparse.c
	$(CC) -c $(CFLAGS) -o target/argparse.o src/argparse/argparse.c

target/socket.o: src/socket/socket.c
	$(CC) -c $(CFLAGS) -o target/socket.o src/socket/socket.c	

target/term.o: src/term/term.c
	$(CC) -c $(CFLAGS) -o target/term.o src/term/term.c

target/main.o: src/main.c
	$(CC) -c $(CFLAGS) -o target/main.o src/main.c
