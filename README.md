CAT filesystem
==============

Shows a list of files as one big file.
It is equal to output of `cat` command, hence the name.

Differs from a similar-named [project](https://sourceforge.net/projects/catfs/)
in that it is optimised for working with a big list of files.

Primary purpose is to make [par](https://en.wikipedia.org/wiki/Parchive)
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

* To extract files if you have `{list file}` and `archive.cat`
	(copied from `{mount point}`):
	
		./uncat {list file} archive.cat [filename]
	
	optionally passing name of the file to be extracted

Example with par2
-----------------

Say, you have a directory with a lot of files in it.
So many that `par2` can't deal with all of them at once
(it has a limit of 32k files).
To work around this limitation, you can either
use tar and create a single file with a copy of all your files in it,
or:

	cd backup/
	find . -type f -printf "%s %P\n" >cat.fs
	mkdir mount && ~/catfs cat.fs mount
	par2create -r1 -n1 par2 mount/archive.cat
	fusermount -u mount && rmdir mount

i.e.:
* go to the directory (we'll be using relative paths)
* make list of files with the given format
* make temporary directory and mount catfs there
* create recovery files for the `archive.cat` file in that directory
* unmount catfs and remove temporary directory

This way you'll save disk space on creating tar file
(if, [for some reason][reason], you prefer to keep your backups as a
spread of files instead of a single tarball),
and still protect them from [bit rot][rot].

[reason]: http://alexey.shpakovsky.ru/en/rsync-backups.html
[rot]: https://en.wikipedia.org/wiki/Data_degradation

You need to keep `cat.fs` and all `par2*` files to be able to restore the files.

In case of damage to any file (say, `important/data.txt`),
you can recover it this way:

	cd backup/
	mkdir mount && ~/catfs cat.fs mount
	cp mount/archive.cat archive.cat
	fusermount -u mount && rmdir mount
	par2recover par2 archive.cat
	~/uncat.sh cat.fs archive.cat important/data.txt

i.e.:
* go to the directory (again)
* make temporary directory and mount catfs there
* copy `archive.cat` out of the virtual file system
* unmount catfs and remove temporary directory
* recover broken `archive.cat` file
* extract recovered file from `archive.cat`

Note that this time, you have to copy whole archive.cat file
out of the virtual filesystem.
That's because catfs is implemented as read-only file system
and under no circumstances will write to your files.

Based on a SSFS by MaaSTaaR: <http://www.maastaar.net/fuse/linux/filesystem/c/2016/05/21/writing-a-simple-filesystem-using-fuse/>

License: GNU GPL.
