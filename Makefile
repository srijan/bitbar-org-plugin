CFLAGS = -O2 -Wall -Wextra

all: plugin
.PHONY: clean debug

debug: clean
debug: CFLAGS += -DDEBUG -g -fsanitize=address -fno-omit-frame-pointer
debug: plugin

plugin: plugin.c config.h
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm -rf plugin
