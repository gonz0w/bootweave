#!/bin/sh
# SPDX-License-Identifier: BSD-3-Clause
# Only the separately confirmed disposable native build guest may run this.
set -eu
umask 077
PATH=/bin:/sbin:/usr/bin:/usr/sbin
export PATH
[ "$(uname -s)" = OpenBSD ] && [ "$(uname -r)" = 7.9 ] &&
    [ "$(uname -m)" = amd64 ] && [ "$(id -u)" = 0 ] || exit 1
[ "$#" -eq 0 ] || exit 1
input=/mnt/storage-probe
output=/root/storage-probe
[ ! -e "$output" ] && [ ! -L "$output" ] || exit 1
cd "$input"
# Source identities must equal the independently frozen host review receipt.
sha256 -C SHA256.inputs probe.c validate.c validate.h validator-test.c profile
signify -C -p /etc/signify/openbsd-79-base.pub -x SHA256.sig bsd.rd
[ "$(sha256 -q cdbr)" = 171ab173143bc41dde2cd5944435040614035bd788a40caf34593ffc2332a753 ] || exit 1
[ "$(sha256 -q cdboot)" = beb4e1952b2cb7d40e25fee22d142cc74889204685d1d3f20a40166d56cc45f5 ] || exit 1
work=$(mktemp -d /tmp/bootweave-storage.XXXXXXXX)
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
mkdir "$work/root" "$work/cd" "$work/cd/etc" "$work/cd/7.9" "$work/cd/7.9/amd64"
cc -std=c99 -Os -static -Wall -Wextra -Werror -pedantic \
    -Wl,-s,-Map,"$work/storage-probe.map" probe.c validate.c -o "$work/storage-probe"
cc -std=c99 -Os -static -Wall -Wextra -Werror -pedantic \
    validator-test.c validate.c -o "$work/validator-test"
"$work/validator-test"
# Device checks and RSS measurements run separately in their reviewed fixtures.
gzip -dc bsd.rd >"$work/bsd.rd"
[ "$(rdsetroot -s "$work/bsd.rd")" = 3768320 ] || exit 1
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
install -m 555 "$work/storage-probe" "$work/root/bin/storage-probe"
install -m 444 profile "$work/root/.profile"
# Fixed amd64 sd major 13, unit 0, raw partition c (minor 2); private FFS only.
[ -d "$work/root/dev" ] && [ ! -L "$work/root/dev" ] || exit 1
node=$work/root/dev/rsd0c
if [ ! -e "$node" ] && [ ! -L "$node" ]; then
    mknod "$node" c 13 2
    chmod 600 "$node"
fi
[ -c "$node" ] && [ ! -L "$node" ] || exit 1
ls -ln "$node"
cmp "$work/storage-probe" "$work/root/bin/storage-probe"
cmp profile "$work/root/.profile"
df -k "$work/root"
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
mkhybrid -a -R -T -L -l -d -D -N -o "$work/storage.iso" \
    -A 'Bootweave disposable storage research' -V 'BOOTWEAVE_STORAGE_RESEARCH' \
    -b 7.9/amd64/cdbr -c 7.9/amd64/boot.catalog "$work/cd"
mkdir "$output"
install -m 444 "$work/storage.iso" "$output/storage.iso"
install -m 444 "$work/cd/7.9/amd64/bsd.rd" "$output/bsd.rd"
install -m 444 "$work/storage-probe" "$output/storage-probe"
install -m 444 "$work/storage-probe.map" "$output/storage-probe.map"
sha256 "$output/storage.iso" "$output/bsd.rd" "$output/storage-probe" "$output/storage-probe.map"
wc -c "$output/storage.iso" "$output/bsd.rd" "$output/storage-probe" profile
echo 'Native build finished; read-only ramdisk checks remain.'
