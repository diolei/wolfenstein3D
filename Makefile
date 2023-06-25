CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -pedantic
LDFLAGS = -lSDL2 -lm

SRCS = ray.c

all: ray

ray: $(SRCS)
	 $(CC) $(CFLAGS) $(SRCS) -o ray $(LDFLAGS)

clean:
	rm -f ray

