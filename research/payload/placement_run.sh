#!/bin/sh
# SPDX-License-Identifier: BSD-3-Clause
set -eu
[ "$(uname -s)" = OpenBSD ] || { echo 'Native placement run requires OpenBSD.' >&2; exit 77; }
[ "$#" -eq 0 ] || exit 2
cd /root/payload-placement
sha256 -c SHA256 >/dev/null
./placement-check
./placement-check-static
sha256 -c SHA256 >/dev/null
[ ! -e work/case ] && [ ! -e work/moved ] && [ ! -e work/source ] && [ ! -e work/core ]
printf '%s\n' 'Native placement source, fixtures and case disposal passed.'
