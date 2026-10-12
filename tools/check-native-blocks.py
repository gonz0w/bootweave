# SPDX-License-Identifier: BSD-3-Clause
"""Check one saved QMP query-block reply; no guest, socket or disk access."""
import argparse
import json
import re
import sys

MAX_INPUT_BYTES = 65536
MAX_ATTACHMENTS = 8
DEVICE = re.compile(r"[A-Za-z0-9][A-Za-z0-9._-]{0,63}\Z")


class Rejected(ValueError):
    """Only fixed diagnostic categories may leave this boundary."""


class SafeParser(argparse.ArgumentParser):
    def error(self, message):
        # argparse normally echoes unknown arguments and their private contents.
        self.exit(2, "Native block observation rejected: arguments.\n")


def unique_object(pairs):
    result = {}
    for key, value in pairs:
        if key in result:
            raise Rejected("duplicate JSON field")
        result[key] = value
    return result


def reject_constant(value):
    raise Rejected("nonstandard JSON number")


def expectations(read_only, writable):
    result = {}
    paths = set()
    for pairs, readonly in ((read_only, True), (writable, False)):
        for device, path in pairs:
            if (not DEVICE.fullmatch(device) or device in result or path in paths
                    or not path.startswith("/") or not 1 < len(path) <= 4096
                    or any(not 32 <= ord(char) <= 126 for char in path)):
                raise Rejected("expected inventory")
            result[device] = (path, readonly)
            paths.add(path)
    if not 1 <= len(result) <= MAX_ATTACHMENTS:
        raise Rejected("expected inventory")
    return result


def check_reply(data, expected):
    if len(data) > MAX_INPUT_BYTES:
        raise Rejected("input size")
    try:
        reply = json.loads(data.decode("utf-8"), object_pairs_hook=unique_object,
                           parse_constant=reject_constant)
    except Rejected:
        raise
    except (ValueError, UnicodeError, RecursionError):
        raise Rejected("JSON input") from None
    if (not isinstance(reply, dict) or set(reply) - {"return", "id"}
            or not isinstance(reply.get("return"), list)):
        raise Rejected("QMP reply")
    blocks = reply["return"]
    if len(blocks) != len(expected):
        raise Rejected("attachment inventory")
    seen = set()
    for block in blocks:
        if not isinstance(block, dict):
            raise Rejected("attachment inventory")
        device = block.get("device")
        if not isinstance(device, str) or device not in expected or device in seen:
            raise Rejected("attachment inventory")
        seen.add(device)
        item = block.get("inserted")
        if not isinstance(item, dict):
            raise Rejected("inserted media")
        path, readonly = expected[device]
        if item.get("file") != path:
            raise Rejected("attachment path")
        if type(item.get("ro")) is not bool or item["ro"] is not readonly:
            raise Rejected("attachment access")
        if item.get("drv") != "raw":
            raise Rejected("raw format")
        if (type(item.get("backing_file_depth")) is not int
                or item["backing_file_depth"] != 0 or "backing_file" in item):
            raise Rejected("backing chain")
        children = item.get("children", [])
        if (not isinstance(children, list)
                or any(not isinstance(child, dict)
                       or not isinstance(child.get("child"), str)
                       or child["child"] == "backing" for child in children)):
            raise Rejected("block children")
        image = item.get("image")
        if (not isinstance(image, dict) or image.get("filename") != path
                or image.get("format") != "raw"):
            raise Rejected("image identity")
        if any(key in image for key in ("backing-filename", "full-backing-filename",
                                       "backing-filename-format", "backing-image")):
            raise Rejected("backing chain")
    return len(blocks)


def main(argv=None):
    parser = SafeParser(prog="check-native-blocks", allow_abbrev=False,
                        description="Check a saved QMP query-block reply on stdin.")
    parser.add_argument("--expect-read-only", action="append", nargs=2, default=[],
                        metavar=("DEVICE", "ABSOLUTE_PATH"))
    parser.add_argument("--expect-writable", action="append", nargs=2, default=[],
                        metavar=("DEVICE", "ABSOLUTE_PATH"),
                        help="match observed access; does not authorize writes")
    args = parser.parse_args(argv)
    try:
        expected = expectations(args.expect_read_only, args.expect_writable)
        data = sys.stdin.buffer.read(MAX_INPUT_BYTES + 1)
        count = check_reply(data, expected)
    except Rejected as error:
        print(f"Native block observation rejected: {error}.", file=sys.stderr)
        return 2
    except OSError:
        print("Native block observation rejected: input read.", file=sys.stderr)
        return 2
    print(f"Block observation matches {count} expected raw attachments.")
    return 0


if __name__ == "__main__":
    sys.exit(main())
