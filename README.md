CAT filesystem
==============

Merges a list of files into one. Should produce equivalent of `cat` command.

Differs from a similar-named [project](https://sourceforge.net/projects/catfs/)
in that is optimised for working with a big list of files.

Primary purpose - to make [par](https://en.wikipedia.org/wiki/Parchive)
work with many files at once (by making par think that it's only one file).

Usage
-----

* To generate a list of files:

		find . -type f -printf "%s %P\n" >{list file}

* To mount, from the same directory:

		./catfs {list file} {mount point}

	You will see `{mount point}/archive.cat` file.
	It should be identical to the one produced by this command:

		find . -type f -print0 | xargs -0 cat >{reference file}

* To unmount:

		fusermount -u {mount point}

Based on a SSFS by MaaSTaaR: <http://www.maastaar.net/fuse/linux/filesystem/c/2016/05/21/writing-a-simple-filesystem-using-fuse/>

License: GNU GPL.
