CC=clang
CFLAGS=-Wall -Werror -I./include
CFILES=src/main.c src/problem1.c src/problem2.c src/problem3.c src/problem4.c src/problem5.c src/problem6.c src/problem7.c src/problem8.c src/problem9.c src/problem10.c src/response.c src/utils.c
OUTDIR=build

main: $(CFILES) | build
	$(CC) $(CFLAGS) $(CFILES) -o $(OUTDIR)/main

build:
	mkdir -p $(OUTDIR)

clean:
	rm -rf $(OUTDIR)/
