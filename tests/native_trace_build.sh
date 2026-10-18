#!/bin/sh
# SPDX-License-Identifier: BSD-3-Clause
# Fixed trusted evidence build; production artifacts in /planner stay unchanged.
set -eu
umask 077
[ "$#" -eq 0 ] || exit 2
[ "$(uname -s)" = OpenBSD ] || exit 77
[ "$(uname -r)" = 7.9 ] && [ "$(machine)" = amd64 ] || exit 77
[ "$(id -u)" -eq 0 ] && [ ! -L /planner-trace ] || exit 4
cd /planner-trace
sha256 -c SHA256
[ "$(sha256 -q /planner/native-compile)" = 4f04fa2158c2095fc3dc36c44066f8bebf192025fb71d990fbc75da769ab0018 ] || exit 4
(cd /planner && sha256 -c SHA256 && sha256 -c ARTIFACTS.SHA256)
mkdir -m 0700 compiler-tmp work
TMPDIR=/planner-trace/compiler-tmp
export TMPDIR
printf '%s\n' 'planner outside canary' > work/canary
printf '%s\n' 'planner outside canary' > outside-canary
: > work/input
chown 32767:32767 work work/canary work/input outside-canary
chmod 0644 work/canary work/input outside-canary
chmod 0755 /planner-trace
/planner/native-compile -std=c99 -O2 -Wall -Wextra -Werror -pedantic -I. -Dsetrlimit=bw_trace_setrlimit -c native.c -o trace-native.o
/planner/native-compile -std=c99 -O2 -Wall -Wextra -Werror -pedantic -DBW_TRACE_POLICY -c native_trace.c -o trace-policy.o
for variant in 0 1; do
    if [ "$variant" -eq 0 ]; then
        set --
        name=trace-bootweave
        limits=trace-limits-dynamic
    else
        set -- -static
        name=trace-bootweave-static
        limits=trace-limits-static
    fi
    /planner/native-compile -std=c99 -O2 -Wall -Wextra -Werror -pedantic "$@" -I. "-Wl,-Map=$name.map" main.c plan.c plan_io.c trace-native.o trace-policy.o -o "$name"
    /planner/native-compile -std=c99 -O2 -Wall -Wextra -Werror -pedantic "$@" trace_limits.c trace-policy.o -o "$limits"
    cp "$name" "bootweave-suid-$variant"
    cp "$name" "bootweave-sgid-$variant"
    chown 0:0 "$name" "$limits" "bootweave-suid-$variant" "bootweave-sgid-$variant"
    chmod 0755 "$name" "$limits"
    chmod 4755 "bootweave-suid-$variant"
    chmod 2755 "bootweave-sgid-$variant"
done
/planner/native-compile -std=c99 -O2 -static -Wall -Wextra -Werror -pedantic -DBW_NATIVE_TRACE -I. native_check.c native_trace.c -lutil -o trace-check
chmod 0755 trace-check
sha256 trace-bootweave trace-bootweave-static bootweave-suid-0 bootweave-suid-1 bootweave-sgid-0 bootweave-sgid-1 trace-limits-dynamic trace-limits-static trace-check trace-native.o trace-policy.o > TRACE_ARTIFACTS.SHA256
cat TRACE_ARTIFACTS.SHA256
sha256 ./*.map
ldd trace-bootweave trace-limits-dynamic
for binary in trace-bootweave-static trace-limits-static trace-check; do
    readelf -l "$binary"
    readelf -d "$binary"
done
rmdir compiler-tmp
sha256 -c SHA256
(cd /planner && sha256 -c SHA256 && sha256 -c ARTIFACTS.SHA256)
printf '%s\n' 'Planner trace-only build complete; original production artifacts unchanged.'
