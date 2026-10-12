#!/bin/sh
# SPDX-License-Identifier: BSD-3-Clause
# Run only inside the reviewed disposable OpenBSD build guest.
set -eu
umask 077
if [ "$(uname -s)" != OpenBSD ]; then
    echo 'Native UI checks require OpenBSD.' >&2
    exit 1
fi
if [ "$#" -ne 1 ] || [ ! -f "$1" ] || [ ! -x "$1" ]; then
    echo 'Supply the reviewed native simulation binary.' >&2
    exit 1
fi
binary=$1
case $binary in
    /*) ;;
    *) echo 'Use an absolute binary path.' >&2; exit 1 ;;
esac
fixture=$(mktemp -d /tmp/bootweave-ui.XXXXXXXX)
trap 'rm -f "$fixture/output"; rmdir "$fixture"' EXIT
trap 'exit 2' HUP INT TERM
run_case() {
    name=$1 input=$2 expected=$3 marker=$4
    result=0
    printf '%b' "$input" | "$binary" >"$fixture/output" || result=$?
    if [ "$result" -ne "$expected" ]; then
        echo "FAIL $name: exit $result, expected $expected" >&2
        exit 1
    fi
    if ! grep -Fqx "$marker" "$fixture/output"; then
        echo "FAIL $name: missing expected output" >&2
        exit 1
    fi
    if [ "$expected" -ne 0 ] && grep -Fq 'SIMULATION COMPLETE' "$fixture/output"; then
        echo "FAIL $name: unexpected completion" >&2
        exit 1
    fi
    echo "PASS $name"
}
run_case valid 'Example\nalpha\ncomplete\n' 0 'SIMULATION COMPLETE'
run_case crlf 'Example\r\nbeta\r\ncomplete\r\n' 0 'SIMULATION COMPLETE'
run_case cancel 'q\n' 2 'Label (ASCII, 1-64 bytes):'
run_case incomplete 'Example\nalpha\n\n' 2 'Input rejected.'
run_case escape '\033Example\n' 2 'Input rejected.'
run_case nul 'Example\000suffix\n' 2 'Input rejected.'
run_case help_back 'Example\nalpha\nb\nh\nkeep\ncomplete\n' 0 'SIMULATION COMPLETE'
echo 'Seven native pipe cases passed; tty, resources and ramdisk remain separate.'
