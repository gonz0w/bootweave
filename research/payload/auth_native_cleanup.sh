#!/bin/sh
# SPDX-License-Identifier: BSD-3-Clause
# Fixed trusted scope-end cleanup, never an input-selected operation.
set -eu
[ "$(uname -s)" = OpenBSD ] || { echo 'Native authentication cleanup requires OpenBSD.' >&2; exit 77; }
[ "$#" -eq 0 ] || exit 2
cd /root/payload-auth
sha256 -c SHA256 >/dev/null
[ -d keys ] && [ -f key-lifetime.private ]
rm keys/selected.pub keys/selected.sec keys/other.pub keys/other.sec
rmdir keys
rm key-lifetime.private
[ ! -e keys ] && [ ! -e key-lifetime.private ]
printf '%s\n' 'Native synthetic two-pair key lifetime disposed.'
