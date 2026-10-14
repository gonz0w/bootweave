#!/bin/sh
# SPDX-License-Identifier: BSD-3-Clause
set -eu
umask 077
[ "$(uname -s)" = OpenBSD ] || { echo 'Installed build requires OpenBSD.' >&2; exit 77; }
[ "$#" -eq 0 ] || exit 2
cd /root/installed-build
sha256 -c SHA256 >/dev/null
[ -f fixture_keys.h ] && [ -f FIXTURE.SHA256 ]
sha256 -c FIXTURE.SHA256 >/dev/null
mkdir output
for variant in dynamic static; do
    if [ "$variant" = static ]; then
        set -- -static '-DAUTH_INSPECTOR="/mnt/installed/inspect-static"'
    else
        set -- '-DAUTH_INSPECTOR="/mnt/installed/inspect"'
    fi
    for mode in auto manual; do
        if [ "$mode" = manual ]; then
            set -- "$@" -DREVIEW_MANUAL
        fi
        cc -std=c99 -Os -Wall -Wextra -Werror -pedantic "$@" -DAUTH_TEST -DAUTH_PLACEMENT -DAUTH_REVIEW -DAUTH_INSTALLED -DRV_OUTPUT=15000 '-DAUTH_HOME="/bootweave-research/"' '-DPL_WORK="/bootweave-research/"' -Ipayload -Istorage -Iinstalled -I. "-Wl,-Map=output/$mode-$variant.map" installed/check.c installed/root.c installed/root_native.c storage/validate.c payload/auth_native.c payload/placement_plan.c payload/placement_native.c payload/review.c payload/review_native.c -lutil -o "output/$mode-$variant"
        sha256 "output/$mode-$variant" "output/$mode-$variant.map"
        size "output/$mode-$variant"
        if [ "$variant" = dynamic ]; then
            ldd "output/$mode-$variant"
        else
            readelf -l "output/$mode-$variant"
            readelf -d "output/$mode-$variant"
        fi
    done
done
for mode in KEEP READBACK; do
    cc -std=c99 -Os -static -Wall -Wextra -Werror -pedantic -DAUTH_TEST -DAUTH_PLACEMENT -DAUTH_REVIEW -DAUTH_INSTALLED -DRV_OUTPUT=15000 -DREVIEW_MANUAL "-DIPF_$mode" '-DAUTH_HOME="/bootweave-research/"' '-DPL_WORK="/bootweave-research/"' '-DAUTH_INSPECTOR="/mnt/installed/inspect-static"' -Ipayload -Istorage -Iinstalled -I. "-Wl,-Map=output/$mode.map" installed/check.c installed/root.c installed/root_native.c storage/validate.c payload/auth_native.c payload/placement_plan.c payload/placement_native.c payload/review.c payload/review_native.c -lutil -o "output/$mode"
    sha256 "output/$mode" "output/$mode.map"
    size "output/$mode"
    readelf -d "output/$mode"
done
printf '%s\n' 'Installed native build finished; target tests remain.'
