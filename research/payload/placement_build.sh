#!/bin/sh
# SPDX-License-Identifier: BSD-3-Clause
set -eu
umask 077
[ "$(uname -s)" = OpenBSD ] || { echo 'Native placement build requires OpenBSD.' >&2; exit 77; }
[ "$#" -eq 0 ] || exit 2
cd /root/payload-placement
sha256 -c SHA256 >/dev/null
mkdir work
printf %s 'owned sentinel' > work/canary
uname -srvm
cc --version
cc -std=c99 -Os -Wall -Wextra -Werror -pedantic -DAUTH_PLACEMENT -DAUTH_TEST -Wl,-Map=placement-check.map auth_native.c placement_plan.c placement_native.c placement_check.c -o placement-check
cc -std=c99 -Os -Wall -Wextra -Werror -pedantic -static -DAUTH_PLACEMENT -DAUTH_TEST '-DAUTH_INSPECTOR="/root/payload-auth/inspect-static"' -Wl,-Map=placement-check-static.map auth_native.c placement_plan.c placement_native.c placement_check.c -o placement-check-static
sha256 placement-check placement-check-static placement-check.map placement-check-static.map
size placement-check placement-check-static
ldd placement-check
readelf -l placement-check-static
readelf -d placement-check-static
printf '%s\n' 'Native placement build finished.'
