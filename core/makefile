# Это комментарий, который говорит, что переменная CC указывает компилятор, используемый для сборки
CC=gcc
# Это еще один комментарий. Он поясняет, что в переменной CFLAGS лежат флаги, которые передаются компилятору
CFLAGS=-c -Wall

all: bin

bin: main.o frame.o io.o header.o utils.o
	$(CC) main.o frame.o io.o header.o utils.o -o bin

main.o: ./src/main.c
	$(CC) $(CFLAGS) ./src/main.c -o main.o

header.o: ./src/header/v1_header.c utils.o
	$(CC) $(CFLAGS) ./src/header/v1_header.c -o header.o

frame.o: ./src/frame/v1_frame.c
	$(CC) $(CFLAGS) ./src/frame/v1_frame.c -o frame.o

io.o: ./src/io/io.c
	$(CC) $(CFLAGS) ./src/io/io.c -o io.o

utils.o: ./src/utils/utils.c
	$(CC) $(CFLAGS) ./src/utils/utils.c -o utils.o

clean:
	rm -rf *.o bin */**/*.gch data.dbin index.idx lockfile
