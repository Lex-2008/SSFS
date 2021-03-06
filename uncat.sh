#!/bin/sh

usage() {
	echo "$0 {list} {archive} [file]"
	echo
	echo "Where:"
	echo "  {list} is name of file generated by ´find´ command"
	echo "  {archive} is name of file copied from catfs"
	echo "  [file] is name of file to be extracted from the archive"
	echo "    if [file] is omitted, all files will be extracted"
	exit 1
}

test -z "$1" && usage || list="$1"
test -z "$2" && usage || archive="$2"
test -z "$3" && all=1 || filename="$3"

pos=0
while read line; do
	size="$(expr "$line" : '\([0-9]*\) ')"
	name="$(expr "$line" : '[0-9]* \(.*\)')"
	if [ "$all" = "1" -o "$filename" = "$name" ]; then
		echo "$name"
		mkdir -p "$(dirname "$name")"
		dd if="$archive" of="$name" bs=1 count=$size skip=$pos >/dev/null 2>&1
	fi
	pos=$((pos+size))
done <"$list"
