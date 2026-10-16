# SPDX-License-Identifier: BSD-3-Clause
"""Read-only preflight and fixed argument preview; cannot launch/create targets."""

import hashlib
import json
import os
from pathlib import Path
import stat
import sys

ROOT = Path(__file__).resolve().parents[1]
FIXTURE = ROOT / ".tools/project-planner-native"
TARGET = FIXTURE / "build.raw"
PINS = {
    "/opt/homebrew/bin/qemu-system-x86_64": "25afcb2986e4a0f2c567de17273f55de3c99792b78a07c99e923bb62024c088f",
    "/opt/homebrew/share/qemu/bios-256k.bin": "ae6f6aa973aaccc143f57aa960fb035fd9de4daee4ad0cd713322f8c259e7650",
    "/opt/homebrew/share/qemu/vgabios-stdvga.bin": "e8fc9e55790dbe3cb31f019a3deb57206ba6c54f5e581adb2ab2677a9d391472",
    "/opt/homebrew/bin/xorriso": "d217ca8e759699dcaf36953822bd508f13587c57a890d2c1a71b19f6d0e8da86",
    str(ROOT / ".tools/signify-review/signify-osx-1.4/signify"): "4b9b2069c5e5acda9d35ca898ecd5afc050094170cf3c73fa7cf9eb615530946",
    str(ROOT / ".tools/release-key-review/mirror.pub"): "b7ee8e7981549a1f707a4ed05115a99959a5f9e34b9bed9f9a098be17dd7e4b1",
    str(ROOT / ".tools/openbsd-79-amd64/install79.iso"): "7a4a92e953618035097c796a90b54424a0f3ae775552e1e7d102cf8a5130449f",
    str(ROOT / ".tools/openbsd-79-amd64/SHA256.sig"): "99db2ba3d63cddeeb9a1166319e48bf4b22224c48bbdc4deff8ecadc0f7b2786",
}
PHASES = ("provision", "serial", "vga", "readback")


def arguments(phase):
    if phase not in PHASES:
        raise ValueError("Unknown fixed native phase")
    argv = ["/opt/homebrew/bin/qemu-system-x86_64", "-S", "-no-user-config",
            "-nodefaults", "-machine", "pc-i440fx-11.1", "-accel", "tcg,thread=single",
            "-cpu", "qemu64-v1", "-smp", "1", "-m", "1024", "-no-reboot",
            "-display", "none", "-vga", "none", "-monitor", "none", "-parallel", "none",
            "-nic", "none", "-net", "none",
            "-bios", "/opt/homebrew/share/qemu/bios-256k.bin",
            "-device", "VGA,id=video,bus=pci.0,addr=0x2,romfile=/opt/homebrew/share/qemu/vgabios-stdvga.bin",
            "-device", "ich9-ahci,id=ahci,bus=pci.0,addr=0x4",
            "-drive", f"file={TARGET},if=none,id=target,format=raw,readonly=off",
            "-device", "virtio-blk-pci,id=planner-disk,drive=target,bus=pci.0,addr=0x3",
            "-drive", f"file={FIXTURE / 'data.iso'},if=none,id=data,format=raw,media=cdrom,readonly=on",
            "-device", "ide-cd,id=data-cd,drive=data,bus=ahci.1,unit=0",
            "-qmp", f"unix:{FIXTURE / 'qmp.sock'},server=on,wait=off",
            "-chardev", f"socket,id=console,path={FIXTURE / 'serial.sock'},server=on,wait=off",
            "-device", "isa-serial,chardev=console,index=0"]
    if phase == "provision":
        argv += ["-drive", f"file={ROOT / '.tools/openbsd-79-amd64/install79.iso'},if=none,id=installer,format=raw,media=cdrom,readonly=on",
                 "-device", "ide-cd,id=installer-cd,drive=installer,bus=ahci.0,unit=0",
                 "-boot", "once=d,order=c,menu=off,strict=on"]
    else:
        argv += ["-boot", "order=c,menu=off,strict=on"]
    return argv


def preflight():
    if os.path.lexists(FIXTURE):
        raise RuntimeError("Fresh fixture namespace is not absent")
    observed = {}
    for name, expected in PINS.items():
        path = Path(name).resolve(strict=True)
        fd = os.open(path, os.O_RDONLY | os.O_NOFOLLOW)
        with os.fdopen(fd, "rb") as stream:
            before = os.fstat(stream.fileno())
            if not stat.S_ISREG(before.st_mode):
                raise RuntimeError("Pinned input is not regular")
            digest = hashlib.file_digest(stream, "sha256").hexdigest()
            after = os.fstat(stream.fileno())
        def identity(item):
            return (item.st_dev, item.st_ino, item.st_mode, item.st_uid, item.st_gid,
                    item.st_nlink, item.st_size, item.st_mtime_ns, item.st_ctime_ns)
        if (identity(before) != identity(after) or
                identity(after) != identity(path.lstat()) or digest != expected):
            raise RuntimeError("Pinned input changed or mismatched")
        observed[name] = {"bytes": before.st_size, "sha256": digest}
    return {"guest_started": False, "target_created": False, "write_authorized": False,
            "target": str(TARGET), "expected_bytes": 8589934592,
            "expected_zero_sha256": "ebfb4ef19ae410f190327b5ebd312711263bc7579970e87d9c1e2d84e06b3c25",
            "pins": observed, "arguments": {phase: arguments(phase) for phase in PHASES}}


if __name__ == "__main__":
    if len(sys.argv) != 1:
        raise SystemExit("Preflight accepts no arguments and never creates/launches a fixture.")
    print(json.dumps(preflight(), indent=2))
