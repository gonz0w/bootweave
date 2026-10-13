#!/bin/sh
# SPDX-License-Identifier: BSD-3-Clause
set -eu
umask 077
[ "$(uname -s)" = OpenBSD ] || { echo 'Native review build requires OpenBSD.' >&2; exit 77; }
[ "$#" -eq 0 ] || exit 2
cd /root/payload-placement
sha256 -c SHA256 >/dev/null
for variant in dynamic static; do
    if [ "$variant" = static ]; then
        set -- -static '-DAUTH_INSPECTOR="/root/payload-auth/inspect-static"'
    else
        set --
    fi
    for mode in auto manual; do
        if [ "$mode" = manual ]; then
            cc -std=c99 -Os -Wall -Wextra -Werror -pedantic "$@" -DAUTH_TEST -DAUTH_PLACEMENT -DAUTH_REVIEW -DREVIEW_MANUAL "-Wl,-Map=review-$mode-$variant.map" auth_native.c placement_plan.c placement_native.c review.c review_native.c review_check.c -lutil -o "review-$mode-$variant"
        else
            cc -std=c99 -Os -Wall -Wextra -Werror -pedantic "$@" -DAUTH_TEST -DAUTH_PLACEMENT -DAUTH_REVIEW "-Wl,-Map=review-$mode-$variant.map" auth_native.c placement_plan.c placement_native.c review.c review_native.c review_check.c -lutil -o "review-$mode-$variant"
        fi
        sha256 "review-$mode-$variant" "review-$mode-$variant.map"
        size "review-$mode-$variant"
        if [ "$variant" = dynamic ]; then
            ldd "review-$mode-$variant"
        else
            readelf -l "review-$mode-$variant"
            readelf -d "review-$mode-$variant"
        fi
    done
done
printf '%s\n' 'Native review build finished.'
