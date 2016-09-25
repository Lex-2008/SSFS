#define FUSE_USE_VERSION 30

#include <fuse.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <time.h>
#include <string.h>
#include <stdlib.h>

#include <inttypes.h>
#include <linux/limits.h>

struct entry {
	char *name;
	off_t start;
	off_t size;
};

struct entry *entries;
int entries_len;
// properties of the file in fake FS
char *filename;
off_t filelen;
//struct stat stat;

static int do_getattr( const char *path, struct stat *st )
{
	//printf("getattr [%s]\n", path);
	st->st_uid = getuid(); // The owner of the file/directory is the user who mounted the filesystem
	st->st_gid = getgid(); // The group of the file/directory is the same as the group of the user who mounted the filesystem
	st->st_atime = time( NULL ); // The last "a"ccess of the file/directory is right now
	st->st_mtime = time( NULL ); // The last "m"odification of the file/directory is right now
	
	if ( strcmp( path, "/" ) == 0 )
	{
		st->st_mode = S_IFDIR | 0755;
		st->st_nlink = 2; // Why "two" hardlinks instead of "one"? The answer is here: http://unix.stackexchange.com/a/101536
	}
	else
	{
		st->st_mode = S_IFREG | 0444;
		st->st_nlink = 1;
		st->st_size = filelen;
	}
	return 0;
}

static int do_readdir( const char *path, void *buffer, fuse_fill_dir_t filler, off_t offset, struct fuse_file_info *fi )
{
	filler( buffer, ".", NULL, 0 ); // Current Directory
	filler( buffer, "..", NULL, 0 ); // Parent Directory
	
	if ( strcmp( path, "/" ) == 0 ) // If the user is trying to show the files/directories of the root directory show the following
	{
		filler( buffer, "archive.cat", NULL, 0 );
	}
	
	return 0;
}

int BinarySearch(struct entry *array, int number_of_elements, off_t key) {
	int low = 0, high = number_of_elements-1, mid;
	int iter =0;
	while(low < high) {
		iter++;
		mid = (low + high)/2;
		if(mid==low) mid++;
		if(array[mid].start < key) low = mid;
		else if(array[mid].start > key) high = mid-1;
		//printf("%d %d %d\n", iter, low, high);
		if(iter==number_of_elements) return mid;
	}
	return low;
}

off_t min(off_t a, off_t b) {
	if(a<b) return a;
	return b;
}

static int do_read( const char *path, char *buffer, size_t size, off_t offset, struct fuse_file_info *fi )
{
	memset(buffer, 0, size);
	//printf( "--> Trying to read %s, %u, %u\n", path, offset, size );
	off_t bytes_written=0;
	off_t bytes_left=size;
	if(offset>filelen) offset=filelen;
	if(bytes_left+offset>filelen) bytes_left=filelen-offset;
	//printf( "--> Trying to read %s, %u, %u\n", path, offset, bytes_left );
	int i=BinarySearch(entries, entries_len, offset);
	//printf( "start entry %d\n", i);
	
	while(bytes_left>0 && i<entries_len) {
		off_t pos_in_file=offset-entries[i].start;
		//printf( "entry %d, %d\n", i, pos_in_file);
		int fd = open(entries[i].name, O_RDONLY);
		// TODO: cache fd
		if (fd != -1) {
			//printf( "reading %u bytes starting at %u to %u \n", bytes_left, pos_in_file, bytes_written);
			pread(fd, buffer+bytes_written, bytes_left, pos_in_file);
			close(fd);
		//} else {
			// NOTE: even if we didn't actually read the file,
			// we still consider it "ok" result
			// (relevant part in buffer will be zero-filled)
		}
		off_t this_result = min(entries[i].size-pos_in_file, bytes_left);
		bytes_written += this_result;
		offset += this_result;
		bytes_left -= this_result;
		//printf( "%u bytes written, offset=%u, %u bytes left\n", bytes_written, offset, bytes_left);
		//printf( "uoutput: [%s]\n", buffer);
		i++;
	}
	return bytes_written;
}

static struct fuse_operations operations = {
    .getattr	= do_getattr,
    .readdir	= do_readdir,
    .read		= do_read,
};

int main( int argc, char *argv[] )
{
	char line[PATH_MAX];
	FILE *fr = fopen (argv[1], "rt");
	int i=0;
	while(fgets(line, PATH_MAX, fr) != NULL) {
		i++;
	}
	entries_len = i;
	entries = malloc(sizeof(struct entry) * entries_len);
	rewind(fr);
	i=0;
	off_t filepos=0;
	while(fgets(line, sizeof line, fr) != NULL) {
		size_t len = strlen(line);
		if (len > 0 && line[len-1] == '\n') {
			line[--len] = '\0';
		}
		sscanf (line, "%" SCNd64, &(entries[i].size));
		entries[i].start=filepos;
		filepos=entries[i].start+entries[i].size;
		char *name = line;
		while (*name != ' ') name++;
		name++;
		entries[i].name = malloc(strlen(name)+1);
		strcpy(entries[i].name, name);
		i++;
	}
	filelen=filepos;
	fclose(fr);  /* close the file prior to exiting the routine */
	return fuse_main( argc-1, argv+1, &operations, NULL );
	/*
	off_t value;
	sscanf (argv[2], "%" SCNd64, &(value));
	int ret=BinarySearch(entries, entries_len, value);
	printf("%" PRId64 "\n", value);
	printf("%d\n", ret);
	i=ret;
	printf ("[%s] [%" PRId64 "][%" PRId64 "]\n", entries[i].name, entries[i].size, entries[i].start);
	i++;
	printf ("[%s] [%" PRId64 "][%" PRId64 "]\n", entries[i].name, entries[i].size, entries[i].start);
	*/
}
