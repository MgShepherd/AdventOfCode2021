CC=clang
CFLAGS=-Wall -Werror -I./include
CFILES=src/main.c src/problem1.c src/response.c
OUTDIR=build

main: $(CFILES) | build
	$(CC) $(CFLAGS) $(CFILES) -o $(OUTDIR)/main

build:
	mkdir -p $(OUTDIR)

clean:
	rm -rf $(OUTDIR)/
