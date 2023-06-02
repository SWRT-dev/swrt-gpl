#!/bin/sh

maj=$(cat ./VERSION | cut -f1 -d.)
min=$(cat ./VERSION | cut -f2 -d.)
rev=$(cat ./VERSION | cut -f3 -d.)

gen_version() {
	[ -d ../.git ] && {
		res=$(git rev-parse --verify -q HEAD)
		[ -n "$res" ] && {
			xtra=-$(echo $res | cut -c -8)
			rev=$rev$xtra
		}

		res=$(git diff-index --quiet HEAD)
		[ "$?" != "0" ] && rev=$rev"-dirty"
	}

	echo "-DLIBEASY_MAJOR=$maj -DLIBEASY_MINOR=$min -DLIBEASY_REV=$rev"
}

gen_version
