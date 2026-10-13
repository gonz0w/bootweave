#!/bin/sh
# SPDX-License-Identifier: BSD-3-Clause
set -eu
umask 077
[ "$(uname -s)" = OpenBSD ] || { echo 'Native authentication build requires OpenBSD.' >&2; exit 77; }
[ "$#" -eq 0 ] || exit 2
cd /root/payload-auth
sha256 -c SHA256 >/dev/null
mkdir work
printf %s 'owned sentinel' > work/canary
uname -srvm
cc --version
cc -std=c99 -Os -Wall -Wextra -Werror -pedantic -Wl,-Map=inspect.map inspect.c -o inspect
cc -std=c99 -Os -Wall -Wextra -Werror -pedantic -static -Wl,-Map=inspect-static.map inspect.c -o inspect-static
cc -std=c99 -Os -Wall -Wextra -Werror -pedantic -DAUTH_STANDALONE -Wl,-Map=auth.map auth_native.c -o auth
cc -std=c99 -Os -Wall -Wextra -Werror -pedantic -static -DAUTH_STANDALONE -Wl,-Map=auth-static.map auth_native.c -o auth-static
cc -std=c99 -Os -Wall -Wextra -Werror -pedantic -DAUTH_TEST -Wl,-Map=auth-check.map auth_native.c auth_native_check.c -o auth-check
cc -std=c99 -Os -Wall -Wextra -Werror -pedantic -static -DAUTH_TEST '-DAUTH_INSPECTOR="/root/payload-auth/inspect-static"' -Wl,-Map=auth-check-static.map auth_native.c auth_native_check.c -o auth-check-static
sha256 /usr/bin/signify inspect inspect-static auth auth-static auth-check auth-check-static
size inspect inspect-static auth auth-static auth-check auth-check-static
ldd auth auth-check /usr/bin/signify
readelf -l auth-static
readelf -d auth-static
printf '%s\n' 'Native authentication build finished.'
