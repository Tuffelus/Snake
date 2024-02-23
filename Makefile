CC = clang
CFLAGS = -std=c17 -g -Wall

COMPONENTS = snake

all: main

main: $(COMPONENTS:=.c) $(COMPONENTS:=.h)
	$(CC) $(CFLAGS) $(COMPONENTS:=.c) -o Snake -lncurses

clean:
	rm -f *.o
	rm -f Snake

format:
	find . -regex '.*\.\(c\|h\)$$' -exec clang-format -i {} \;


