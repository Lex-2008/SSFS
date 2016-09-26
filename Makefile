COMPILER = gcc
FILESYSTEM_FILES = catfs.c
OUTPUT = catfs

build: $(FILESYSTEM_FILES)
	$(COMPILER) $(FILESYSTEM_FILES) -o $(OUTPUT) `pkg-config fuse --cflags --libs`

clean:
	rm $(OUTPUT)
