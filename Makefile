COMPILER = gcc
FILESYSTEM_FILES = catfs.c

build: $(FILESYSTEM_FILES)
	$(COMPILER) $(FILESYSTEM_FILES) -o ssfs `pkg-config fuse --cflags --libs`

clean:
	rm ssfs
