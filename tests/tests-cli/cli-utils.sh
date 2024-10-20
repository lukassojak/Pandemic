#!/bin/false


#   ┏━━━━━━━━━━━━━━━━━━━━━━━━━━━┓
#   ┃   DO NOT EDIT THIS FILE   ┃
#   ┗━━━━━━━━━━━━━━━━━━━━━━━━━━━┛


# This file contains utilities that can help you create directory trees.
# You will not find any tests here, however. Look for files named ‹test-*.sh›
# instead.



if [ -n "${_cli_test_utils+loaded}" ]; then
	return
fi

_cli_test_utils=1

_exe="${cli_script_bin}"/pandemic

pandemic() {
	timeout --kill-after 5 ${CLI_TEST_TIMEOUT:-30} \
	valgrind --log-file="$cli_stash/valgrind.log" --errors-for-leak-kinds=all \
		 --leak-check=full --show-leak-kinds=all --track-fds=yes \
	"${_exe}" "$@"
}

exe="pandemic"

tests_init() {
	if [ ! -x "${_exe}" ]; then
		_cli_msg_fatal "Executable '$_exe' not found"
	fi
}
