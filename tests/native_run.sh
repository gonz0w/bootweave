#!/bin/sh
# SPDX-License-Identifier: BSD-3-Clause
set -eu
[ "$#" -eq 0 ] || exit 2
[ "$(uname -s)" = OpenBSD ] || exit 77
[ "$(uname -r)" = 7.9 ] && [ "$(machine)" = amd64 ] || exit 77
[ "$(id -u)" -eq 0 ] && [ ! -L /planner ] || exit 4
cd /planner
sha256 -c SHA256 >/dev/null
sha256 -c ARTIFACTS.SHA256 >/dev/null
for suite in --corpus --edges --faults --probes --unit; do
    ./native-check "$suite"
done
sha256 -c SHA256 >/dev/null
sha256 -c ARTIFACTS.SHA256 >/dev/null
printf '%s\n' 'Planner native suites passed; actual consoles/readback/disposal remain.'
