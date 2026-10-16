# SPDX-License-Identifier: BSD-3-Clause
"""Independent synthetic definitions and plaintext oracle, never production code."""

import json

BASE = {
    "schema_version": 1,
    "project": {"id": "offline-viewer", "name": "Offline Viewer"},
    "system": {"release": "7.9", "architecture": "amd64"},
    "payload": {"file": "absent.tar", "sha256": "a" * 64},
}


def encode(model):
    return json.dumps(model, separators=(",", ":")).encode("ascii")


def expected(model):
    return (
        "UNVERIFIED PROJECT PLAN - NO OPERATIONS AUTHORIZED\n"
        "Definition syntax: validated\n"
        f"Project ID (supplied): {model['project']['id']}\n"
        f"Project name (supplied): {model['project']['name']}\n"
        f"Requested release (supplied): {model['system']['release']}\n"
        f"Requested architecture (supplied): {model['system']['architecture']}\n"
        f"Payload file (unverified): {model['payload']['file']}\n"
        f"Claimed SHA-256 (unverified): {model['payload']['sha256']}\n"
        "Payload access: not opened\n"
        "Target: unselected; no device identity collected\n"
        "Operations authorized: none\n"
        "Operations performed: none\n"
        "Operations independently observed: none\n"
    ).encode("ascii")
