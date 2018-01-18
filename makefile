CFLAGS := -Wall -Wextra -pedantic -std=c99
LIBS := $(shell pkg-config --cflags --libs x11)

all: xundec

xundec: xundec.c
	gcc $(CFLAGS) xundec.c $(LIBS) -o xundec

.PHONY: xundec
