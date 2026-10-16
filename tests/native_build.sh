#!/bin/sh
# SPDX-License-Identifier: BSD-3-Clause
# Fixed trusted test build, only inside the separately approved disposable guest.
# OpenBSD base sh supplies these ulimit options after the platform guard below.
# shellcheck disable=SC3045
set -eu
umask 077
[ "$#" -eq 0 ] || exit 2
[ "$(uname -s)" = OpenBSD ] || exit 77
[ "$(uname -r)" = 7.9 ] && [ "$(machine)" = amd64 ] || exit 77
[ "$(id -u)" -eq 0 ] && [ ! -L /planner ] || exit 4
cd /planner
sha256 -c SHA256
uname -srvm
cc --version
ulimit -c 0
ulimit -d 524288
ulimit -t 120
mkdir -m 0700 compiler-tmp
TMPDIR=/planner/compiler-tmp
export TMPDIR
# This bootstrap compiler is externally timed/bounded by the approved host monitor.
/usr/bin/time -lp cc -std=c99 -O2 -Wall -Wextra -Werror -pedantic native_compile.c -o native-compile
chmod 0755 native-compile
printf '%s\n' 'Planner compiler guard ready.'
chmod 0755 /planner
mkdir -m 0700 work
printf '%s\n' 'planner outside canary' > work/canary
printf '%s\n' 'planner outside canary' > outside-canary
: > work/input
chown 32767:32767 work work/canary work/input outside-canary
chmod 0644 work/canary work/input outside-canary
for variant in 0 1; do
    if [ "$variant" -eq 0 ]; then
        set --
        name=bootweave
    else
        set -- -static
        name=bootweave-static
    fi
    ./native-compile -std=c99 -O2 -Wall -Wextra -Werror -pedantic "$@" -I. "-Wl,-Map=$name.map" main.c plan.c plan_io.c native.c -o "$name"
    cp "$name" "bootweave-suid-$variant"
    cp "$name" "bootweave-sgid-$variant"
    chown 0:0 "$name" "bootweave-suid-$variant" "bootweave-sgid-$variant"
    chmod 0755 "$name"
    chmod 4755 "bootweave-suid-$variant"
    chmod 2755 "bootweave-sgid-$variant"
    ./native-compile -std=c99 -O2 -Wall -Wextra -Werror -pedantic -I. -Dread=bw_test_read -Dwrite=bw_test_write -Dfstat=bw_test_fstat -c plan_io.c -o fault-io.o
    ./native-compile -std=c99 -O2 -Wall -Wextra -Werror -pedantic "$@" -I. "-Wl,-Map=io-fault-$variant.map" main.c plan.c native.c fault-io.o plan_faults.c -o "io-fault-$variant"
    ./native-compile -std=c99 -O2 -Wall -Wextra -Werror -pedantic -I. -Dunveil=bw_test_unveil -Dpledge=bw_test_pledge -Dsetrlimit=bw_test_setrlimit -Dissetugid=bw_test_issetugid -c native.c -o fault-native.o
    ./native-compile -std=c99 -O2 -Wall -Wextra -Werror -pedantic "$@" -I. "-Wl,-Map=gate-fault-$variant.map" main.c plan.c plan_io.c fault-native.o native_faults.c -o "gate-fault-$variant"
    ./native-compile -std=c99 -O2 -Wall -Wextra -Werror -pedantic "$@" -I. "-Wl,-Map=probe-$variant.map" native_probe.c native.c plan_io.c plan.c -o "probe-$variant"
    ./native-compile -std=c99 -O2 -Wall -Wextra -Werror -pedantic "$@" -I. "-Wl,-Map=unit-$variant.map" plan_unit.c plan.c -o "unit-$variant"
    chmod 0755 "io-fault-$variant" "gate-fault-$variant" "probe-$variant" "unit-$variant"
done
./native-compile -std=c99 -O2 -static -Wall -Wextra -Werror -pedantic native_check.c -lutil -o native-check
chmod 0755 native-check
sha256 bootweave bootweave-static bootweave-suid-0 bootweave-suid-1 bootweave-sgid-0 bootweave-sgid-1 io-fault-0 io-fault-1 gate-fault-0 gate-fault-1 probe-0 probe-1 unit-0 unit-1 native-check native-compile > ARTIFACTS.SHA256
cat ARTIFACTS.SHA256
sha256 ./*.map
size bootweave bootweave-static native-check
ldd bootweave io-fault-0 gate-fault-0 probe-0 unit-0 native-compile
for binary in bootweave-static io-fault-1 gate-fault-1 probe-1 unit-1 native-check; do
    readelf -l "$binary"
    readelf -d "$binary"
done
rmdir compiler-tmp
sha256 -c SHA256 >/dev/null
printf '%s\n' 'Planner native build complete; execution remains separately gated.'
