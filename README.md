CAT filesystem
==============

Merges a list of files into one. Should produce equivalent of `cat` command.

to generate a list of files:

	find . -type f -printf "%s %P\n" >{list file}

to mount:

	./catfs {list file} {mount point}

Based on a SSFS by MaaSTaaR: <http://www.maastaar.net/fuse/linux/filesystem/c/2016/05/21/writing-a-simple-filesystem-using-fuse/>

License: GNU GPL.
