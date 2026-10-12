#!/bin/sh
# SPDX-License-Identifier: BSD-3-Clause
# Fixed disposable research build; no extraction or installation backend.
set -eu
umask 077
[ "$(uname -s)" = OpenBSD ] || { echo 'Native payload build requires OpenBSD.' >&2; exit 77; }
[ "$#" -eq 0 ] || exit 2
cd /root/payload
sha256 -c TOOLS.SHA256 >/dev/null
sha256 -c SHA256
mkdir work
printf %s 'owned sentinel' > work/canary
uname -srvm
cc --version
cc -std=c99 -Os -Wall -Wextra -Werror -pedantic -Wl,-Map=inspect.map inspect.c -o inspect
cc -std=c99 -Os -Wall -Wextra -Werror -pedantic -static -Wl,-Map=inspect-static.map inspect.c -o inspect-static
cc -std=c99 -Os -Wall -Wextra -Werror -pedantic native_check.c -o native-check
sha256 inspect inspect-static native-check
size inspect inspect-static native-check
ldd inspect inspect-static native-check
printf '%s\n' 'Native payload build finished.'
