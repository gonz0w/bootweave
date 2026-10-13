#!/bin/sh
# SPDX-License-Identifier: BSD-3-Clause
set -eu
[ "$(uname -s)" = OpenBSD ] || { echo 'Native authentication run requires OpenBSD.' >&2; exit 77; }
[ "$#" -eq 0 ] || exit 2
cd /root/payload-auth
sha256 -c SHA256 >/dev/null
./auth-check
./auth-check-static
sha256 -c SHA256 >/dev/null
printf '%s\n' 'Native authentication source and fixture postconditions passed.'
