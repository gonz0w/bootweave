#!/bin/sh
# SPDX-License-Identifier: BSD-3-Clause
set -eu
[ "$(uname -s)" = OpenBSD ] || { echo 'Native review run requires OpenBSD.' >&2; exit 77; }
[ "$#" -eq 0 ] || exit 2
cd /root/payload-placement
sha256 -c SHA256 >/dev/null
./review-auto-dynamic
./review-auto-static
sha256 -c SHA256 >/dev/null
[ ! -e work/case ] && [ ! -e work/moved ] && [ ! -e work/source ] && [ ! -e work/core ]
printf '%s\n' 'Native review automated source/fixture disposal passed.'
