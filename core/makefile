# Это комментарий, который говорит, что переменная CC указывает компилятор, используемый для сборки
CC=gcc
# Это еще один комментарий. Он поясняет, что в переменной CFLAGS лежат флаги, которые передаются компилятору
CFLAGS=-c -Wall

OUT_PREFIX=./dist

all: build

build: compile clean
	make compile && make clean

compile: main.o frame.o io.o header.o utils.o
	$(CC) \
		$(OUT_PREFIX)/main.o \
		$(OUT_PREFIX)/frame.o \
		$(OUT_PREFIX)/io.o \
		$(OUT_PREFIX)/header.o \
		$(OUT_PREFIX)/utils.o \
		-o $(OUT_PREFIX)/bin

main.o: ./src/main.c
	$(CC) $(CFLAGS) ./src/main.c -o $(OUT_PREFIX)/main.o

header.o: ./src/header/v1_header.c utils.o
	$(CC) $(CFLAGS) ./src/header/v1_header.c -o $(OUT_PREFIX)/header.o

frame.o: ./src/frame/v1_frame.c
	$(CC) $(CFLAGS) ./src/frame/v1_frame.c -o $(OUT_PREFIX)/frame.o

io.o: ./src/io/io.c
	$(CC) $(CFLAGS) ./src/io/io.c -o $(OUT_PREFIX)/io.o

utils.o: ./src/utils/utils.c
	$(CC) $(CFLAGS) ./src/utils/utils.c -o $(OUT_PREFIX)/utils.o

del:
	rm -rf *.o bin */**/*.gch data.dbin index.idx lockfile

clean:
	rm -rf $(OUT_PREFIX)/*.o */**/*.gch
