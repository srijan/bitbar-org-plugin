CFLAGS = -O2 -Wall -Wextra

all: plugin
.PHONY: clean watch debug

debug: clean
debug: CFLAGS += -DDEBUG -g -fsanitize=address -fno-omit-frame-pointer
debug: plugin

plugin: plugin.c config.h
	$(CC) $(CFLAGS) $< -o $@

watch:
	fswatch -o plugin.c config.h | xargs -n1 -I{} make

clean:
	rm -rf plugin
