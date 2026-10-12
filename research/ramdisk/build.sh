#!/bin/sh
# SPDX-License-Identifier: BSD-3-Clause
# Bounded research only: run in the separately confirmed disposable build guest.
set -eu
umask 077
PATH=/bin:/sbin:/usr/bin:/usr/sbin
export PATH
[ "$(uname -s)" = OpenBSD ] && [ "$(uname -r)" = 7.9 ] &&
    [ "$(uname -m)" = amd64 ] && [ "$(id -u)" = 0 ] || {
    echo 'Requires the reviewed OpenBSD 7.9 amd64 build guest.' >&2
    exit 1
}
[ "$#" -eq 0 ] || exit 1
input=/mnt/ramdisk-ui
output=/root/ramdisk-ui
[ ! -e "$output" ] && [ ! -L "$output" ] || {
    echo 'Refusing an existing output path.' >&2
    exit 1
}
cd "$input"
# Frozen original source; changing it requires renewed evidence, not a flag.
[ "$(sha256 -q plain-ui.c)" = \
    8bdeaed352dabc532c46d12fcb9a7b5fa7e3ab2363058ba24196e99b87094dde ] || exit 1
signify -C -p /etc/signify/openbsd-79-base.pub -x SHA256.sig bsd.rd
work=$(mktemp -d /tmp/bootweave-ramdisk.XXXXXXXX)
vnd=
mounted=false
cleanup_safe=true
cleanup() {
    result=$?
    trap - EXIT HUP INT TERM
    if ! "$cleanup_safe"; then
        echo 'Uncertain vnode identity; owned build directory retained.' >&2
        exit 3
    fi
    if "$mounted" && ! umount "$work/root"; then
        echo 'Unmount failed; owned build directory retained.' >&2
        exit 3
    fi
    if [ -n "$vnd" ] && ! vnconfig -u "$vnd"; then
        echo 'Vnode detach failed; owned build directory retained.' >&2
        exit 3
    fi
    rm -rf "$work"
    exit "$result"
}
trap cleanup EXIT
trap 'exit 2' HUP INT TERM
mkdir "$work/root" "$work/cd" "$work/cd/etc" "$work/cd/7.9" \
    "$work/cd/7.9/amd64"
cc -std=c99 -Os -static -Wall -Wextra -Werror -pedantic \
    -Wl,-s,-Map,"$work/plain-ui.map" plain-ui.c -o "$work/plain-ui"
cc -std=c99 -Os -static -Wall -Wextra -Werror -pedantic \
    native-pty.c -lutil -o "$work/native-pty"
sh native-smoke.sh "$work/plain-ui"
"$work/native-pty" "$work/plain-ui"
# The signed release bsd.rd is gzip-compressed; operate only on our private copy.
gzip -dc bsd.rd >"$work/bsd.rd"
rdsetroot -s "$work/bsd.rd"
rdsetroot -x "$work/bsd.rd" "$work/root.fs"
vnd=$(vnconfig "$work/root.fs")
case "$vnd" in
    vnd[0-9]) ;;
    *) cleanup_safe=false; exit 3 ;;
esac
vnconfig -l "$vnd"
disklabel "$vnd"
mount -t ffs "/dev/${vnd}a" "$work/root"
mounted=true
df -k "$work/root"
install -m 555 "$work/plain-ui" "$work/root/bin/plain-ui"
install -m 444 profile "$work/root/.profile"
install -m 555 smoke.sh "$work/root/ui-smoke.sh"
# Copies must match before the private filesystem is unmounted and re-embedded.
cmp "$work/plain-ui" "$work/root/bin/plain-ui"
cmp profile "$work/root/.profile"
cmp smoke.sh "$work/root/ui-smoke.sh"
sync
umount "$work/root"
mounted=false
vnconfig -u "$vnd"
vnd=
rdsetroot "$work/bsd.rd" "$work/root.fs"
gzip -9cn "$work/bsd.rd" >"$work/cd/7.9/amd64/bsd.rd"
install -m 444 cdbr "$work/cd/7.9/amd64/cdbr"
install -m 444 cdboot "$work/cd/7.9/amd64/cdboot"
printf '%s\n' 'set image /7.9/amd64/bsd.rd' >"$work/cd/etc/boot.conf"
# BIOS only. No EFI/hybrid disk image, unattended installer or install target.
mkhybrid -a -R -T -L -l -d -D -N -o "$work/ui.iso" \
    -A 'Bootweave disposable UI research' -V 'BOOTWEAVE_UI_RESEARCH' \
    -b 7.9/amd64/cdbr -c 7.9/amd64/boot.catalog "$work/cd"
# The UI ceiling includes its new profile; ISO growth is measured against control.
ui_bytes=$(stat -f %z "$work/plain-ui")
profile_bytes=$(stat -f %z profile)
smoke_bytes=$(stat -f %z smoke.sh)
[ "$((ui_bytes + profile_bytes + smoke_bytes))" -le 16777216 ] || exit 1
mkdir "$output"
install -m 444 "$work/ui.iso" "$output/ui.iso"
install -m 444 "$work/cd/7.9/amd64/bsd.rd" "$output/bsd.rd"
install -m 444 "$work/plain-ui" "$output/plain-ui"
install -m 444 "$work/plain-ui.map" "$output/plain-ui.map"
sha256 "$output/ui.iso" "$output/bsd.rd" "$output/plain-ui"
wc -c "$output/ui.iso" "$output/bsd.rd" "$output/plain-ui" profile smoke.sh
printf '%s\n' 'Native build finished; boot and ramdisk checks are still required.'
