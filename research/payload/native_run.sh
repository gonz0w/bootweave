#!/bin/sh
# SPDX-License-Identifier: BSD-3-Clause
# Only the reviewed fixed inspector and supervisor execute.
set -eu
[ "$(uname -s)" = OpenBSD ] || { echo 'Native payload run requires OpenBSD.' >&2; exit 77; }
[ "$#" -eq 0 ] || exit 2
cd /root/payload
sha256 -c TOOLS.SHA256 >/dev/null
sha256 -c SHA256 >/dev/null
./native-check
sha256 -c SHA256 >/dev/null
printf '%s\n' 'Native payload source and fixture postconditions passed.'
