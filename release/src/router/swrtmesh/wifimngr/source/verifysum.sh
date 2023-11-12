#!/bin/sh

verify_md5() {
	[ -f files.md5 ] && {
		res=$(md5sum -c --status --quiet files.md5 2>/dev/null)
		[ "$?" != "0" ] && echo "-dirty"
	}
}

verify_md5
