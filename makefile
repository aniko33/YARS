target/yars: target/main.o target/argparse.o
	$(CC) target/*.o -o target/yars

target/argparse.o: src/argparse/argparse.c
	$(CC) -c src/argparse/argparse.c -o target/argparse.o

target/main.o: src/main.c
	$(CC) -c src/main.c -o target/main.o
