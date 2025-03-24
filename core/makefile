# Это комментарий, который говорит, что переменная CC указывает компилятор, используемый для сборки
CC=gcc
# Это еще один комментарий. Он поясняет, что в переменной CFLAGS лежат флаги, которые передаются компилятору
CFLAGS=-c -Wall

all: clean compile

compile: main.o frame.o io.o header.o utils.o
	$(CC) ./dist/main.o ./dist/frame.o ./dist/io.o ./dist/header.o ./dist/utils.o -o ./dist/bin

main.o: ./src/main.c
	$(CC) $(CFLAGS) ./src/main.c -o ./dist/main.o

header.o: ./src/header/v1_header.c utils.o
	$(CC) $(CFLAGS) ./src/header/v1_header.c -o ./dist/header.o

frame.o: ./src/frame/v1_frame.c
	$(CC) $(CFLAGS) ./src/frame/v1_frame.c -o ./dist/frame.o

io.o: ./src/io/io.c
	$(CC) $(CFLAGS) ./src/io/io.c -o ./dist/io.o

utils.o: ./src/utils/utils.c
	$(CC) $(CFLAGS) ./src/utils/utils.c -o ./dist/utils.o

clean:
	rm -rf ./dist/*.o ./dist/bin */**/*.gch ./dist/data.dbin ./dist/index.idx ./dist/lockfile
