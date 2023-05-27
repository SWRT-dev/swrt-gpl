#!/bin/sh

TESTS_DIR="./tests"
CONFIG_DIR=${TESTS_DIR}"/config"
CHANGES_DIR="/tmp/.uci"
TMP_DIR=${TESTS_DIR}"/tmp"
FULL_SUITE=${TESTS_DIR}"/full_suite.sh"

UCI_BIN=${UCI_BIN:-"../uci"}
[ -x "$UCI_BIN" ] || {
	echo "uci is not present." >&2
	return 1
}
VALGRIND="valgrind --quiet --show-leak-kinds=all --leak-check=full --track-origins=yes"
UCI_BIN="${VALGRIND} ${UCI_BIN}"
UCI="${UCI_BIN} -c ${CONFIG_DIR} -p ${CHANGES_DIR}"
UCI_Q="${UCI_BIN} -c ${CONFIG_DIR} -p ${CHANGES_DIR} -q"

REF_DIR="./references"
SCRIPTS_DIR="./tests.d"
DO_TEST="./shunit2/shunit2"

rm -rf ${TESTS_DIR}
mkdir -p ${TESTS_DIR}

cat << 'EOF' > ${FULL_SUITE}
setUp() {
	mkdir -p ${CONFIG_DIR} ${CHANGES_DIR} ${TMP_DIR}
}
tearDown() {
	rm -rf ${CONFIG_DIR} ${CHANGES_DIR} ${TMP_DIR}
}
assertSameFile() {
	local ref=$1
	local test=$2
	diff -qr $ref $test
	assertTrue $? || {
		echo "REF:"
		cat $ref
		echo "----"
		echo "TEST:"
		cat $test
		echo "----"
	}
}
assertNotSegFault()
{
	[ $1 -eq 139 ] && fail "Returned with 139: segmentation fault (SIGSEGV)!!!"
}
assertNotIllegal()
{
	[ $1 -eq 132 ] && fail "Returned with 132: Illegal instruction (SIGILL)!!!"
}
assertFailWithNoReturn() {
	local test="$1"
	value=$( $test )
	rv=$?
	assertFalse "'$test' does not fail" $rv
	assertNotSegFault $rv
	assertNotIllegal $rv
	assertNull "'$test' returns '$value'" "$value"
}
EOF

for suite in "${SCRIPTS_DIR}"/*
do
	cat "${suite}" >> ${FULL_SUITE}
done

echo ". ${DO_TEST}" >> ${FULL_SUITE}

REF_DIR="${REF_DIR}" \
CONFIG_DIR="${CONFIG_DIR}" \
CHANGES_DIR="${CHANGES_DIR}" \
TMP_DIR="${TMP_DIR}" \
UCI="${UCI}" \
UCI_Q="${UCI_Q}" \
/bin/sh ${FULL_SUITE}

rm -rf ${TESTS_DIR}
