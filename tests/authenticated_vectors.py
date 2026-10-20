# SPDX-License-Identifier: BSD-3-Clause
"""Independent data oracles for the closed authenticated-review composition.

These are requirements, not observed candidate results.  This module calls only
the existing independent content stimulus constructors; it imports no product,
driver, filesystem, process, clock, signer or verifier.  The signature below is
inert record-format data.  A portable fixed-exec double is not cryptographic or
OpenBSD confinement evidence.

Driver phase names describe a semantic boundary, never a particular clock-call
ordinal.  Instrumented wrappers must delegate to the real preparation, signature
state machine and content pipeline.  There is no supplied successful verdict.
"""
import base64
import copy
import itertools

import content_vectors as content


KEY = content.KEY
SIGNATURE = (b"untrusted comment: independent inert composition signature\n" +
             base64.b64encode(b"Ed" + bytes(72)) + b"\n")

# Exactly two literal replacements; all other bytes stay the independent content
# oracle's bytes, including its three distinct zero-operation statements.
REPORT_TEMPLATE = content.REPORT_TEMPLATE.replace(
    "UNAUTHENTICATED CONTENT REVIEW - NO OPERATIONS AUTHORIZED\n",
    "AUTHENTICATED CONTENT REVIEW - NO OPERATIONS AUTHORIZED\n", 1).replace(
        "Signature authentication: not established\n",
        "Signature authentication: accepted by fixed verifier\n", 1)

RETURNS = {"NOT_TAKEN": -2, "UNCERTAIN": -1, "REJECTED": 0, "COMPLETE": 1}
SPAN_ROLES = ("material", "snapshot", "output", "result", "policy", "cancelled")
ALIAS_PAIRS = tuple(itertools.combinations(SPAN_ROLES, 2))
ZERO_EFFECTS = {"target_identity_collected": 0, "operations_authorized": 0,
                "operations_performed": 0, "operations_independently_observed": 0,
                "extraction_calls": 0, "operation_calls": 0}


def _calls(prepare=0, signature=0, content_call=0, policy=0, hashes=0,
           inspect=0, content_format=0, authenticated_format=0):
    return {"prepare": prepare, "signature": signature, "content": content_call,
            "policy": policy, "hash": hashes, "inspect": inspect,
            "content_format": content_format,
            "authenticated_format": authenticated_format}


NO_WRITE_EXPECTATION = {
    "return": -2, "cause": "unchanged", "state": "unchanged",
    "report": b"", "output_disposition": "unchanged",
    "result_disposition": "unchanged", "material_disposition": "unchanged",
    "calls": _calls(), "ownership_taken": 0, "material_terminal_consumptions": 0,
    "fused_release_calls": 0, "signature_release_calls": 0,
    "final_boundary_calls": 0, "boundary_owner": "caller",
    "effects": ZERO_EFFECTS,
}


def _failure(cause, *, result="REJECTED", material_cause="NONE",
             signature_cause="NONE", content_cause="NONE", policy_cause="NONE",
             calls=None, fused_release=0, signature_release=0,
             final_boundary=1, boundary_owner="fused", final_boundary_failed=0,
             output="zeroed", quarantine=False, live_child=False):
    return {"return": RETURNS[result], "state": "FINISHED", "cause": cause,
            "material_cause": material_cause, "signature_cause": signature_cause,
            "content_cause": content_cause, "policy_cause": policy_cause,
            "report": b"", "length": 0, "model": None, "summary": None,
            "output_disposition": output,
            "result_disposition": "zero_model_except_fixed_causes_and_private_signature",
            "calls": calls or _calls(), "ownership_taken": 1,
            "material_terminal_consumptions": 1,
            "fused_release_calls": fused_release,
            "signature_release_calls": signature_release,
            "final_boundary_calls": final_boundary,
            "boundary_owner": boundary_owner,
            "final_boundary_failed": final_boundary_failed,
            "path_or_process_calls_after_final_boundary": 0, "close_retries": 0,
            "quarantine_required": quarantine, "live_child": live_child,
            "effects": dict(ZERO_EFFECTS)}


def valid_cases():
    """Exact viewer, DNS and firewall reports with explicit inherited models."""
    cases = []
    for original in content.valid_cases()[:3]:
        case = copy.deepcopy(original)
        case["signature"] = SIGNATURE
        report = original["expected"]["report"].replace(
            b"UNAUTHENTICATED CONTENT REVIEW - NO OPERATIONS AUTHORIZED\n",
            b"AUTHENTICATED CONTENT REVIEW - NO OPERATIONS AUTHORIZED\n", 1).replace(
                b"Signature authentication: not established\n",
                b"Signature authentication: accepted by fixed verifier\n", 1)
        case["expected"] = {
            "return": 1, "state": "FINISHED", "cause": "NONE",
            "material_cause": "NONE", "signature_cause": "NONE",
            "content_cause": "NONE", "policy_cause": "NONE",
            "report": report, "length": len(report),
            "model": copy.deepcopy(original["expected"]["model"]),
            "summary": dict(original["expected"]["summary"]),
            "output_disposition": "complete_report_then_NUL_then_zero_tail",
            "result_disposition": "complete_model",
            "calls": _calls(1, 1, 1, 1, 2, 1, 1, 1),
            "ownership_taken": 1, "material_terminal_consumptions": 1,
            "fused_release_calls": 0, "signature_release_calls": 1,
            "final_boundary_calls": 1, "boundary_owner": "signature",
            "final_boundary_failed": 0,
            "path_or_process_calls_after_final_boundary": 0,
            "close_retries": 0, "quarantine_required": False,
            "live_child": True, "effects": dict(ZERO_EFFECTS),
        }
        cases.append(case)
    return tuple(cases)


def no_write_cases():
    """All15 relationships precede reads, including invalid-capacity requests.

    The fixture retains a genuine reservation separately when passing a bogus
    argument pointer.  Caller-owned cleanup AFTER the call is separately labeled
    and must not be counted as an effect of the refused entry.
    """
    result = [{"name": "alias-" + "-".join(pair), "fault": "-".join(pair),
               "expected": copy.deepcopy(NO_WRITE_EXPECTATION)}
              for pair in ALIAS_PAIRS]
    for kind in ("null", "overflow"):
        for role in ("material", "snapshot", "output", "result", "cancelled"):
            result.append({"name": kind + "-" + role, "fault": kind + "-" + role,
                           "expected": copy.deepcopy(NO_WRITE_EXPECTATION)})
    for fault in ("material-empty", "material-ready", "material-closed",
                  "material-uncertain", "result-working", "result-finished"):
        result.append({"name": fault, "fault": fault,
                       "expected": copy.deepcopy(NO_WRITE_EXPECTATION)})
    for fault in ("material-result", "output-policy", "overflow-cancelled"):
        result.append({"name": fault + "-invalid-capacity", "fault": fault,
                       "capacity": 65537,
                       "expected": copy.deepcopy(NO_WRITE_EXPECTATION)})
    return tuple(result)


def argument_cases():
    """Ordinary taken refusals consume only the existing RESERVED capability."""
    result = []
    for role, cap in (("definition", 16384), ("signature", 16384),
                      ("payload", 1048576)):
        for length in (0, cap + 1):
            result.append({"name": role + "-length-" + str(length),
                           "fault": role + "-length", "length": length,
                           "expected": _failure("ARGUMENT", fused_release=1)})
    for fault, capacity in (("normal", 0), ("normal", 65537), ("null-zero", 0)):
        result.append({"name": fault + "-capacity-" + str(capacity),
                       "fault": fault, "capacity": capacity,
                       "expected": _failure("ARGUMENT", fused_release=1,
                                            output="unchanged")})
    return tuple(result)


def policy_cases():
    """A linked trust fixture is input to this test, never a runtime selector."""
    result = []
    for field, value in (("profile", 2), ("key_length", 0), ("key_length", 1025)):
        result.append({"name": "compiled-" + field + "-" + str(value),
                       "policy_change": {field: value},
                       "expected": _failure("POLICY", fused_release=1)})
    result.append({"name": "compiled-key-malformed", "policy_change": {
        "key": b"not a public key\n", "key_length": 17},
        "expected": _failure("MATERIAL", material_cause="RECORD",
                             calls=_calls(prepare=1))})
    result.append({"name": "snapshot-signature-malformed", "signature": b"invalid\n",
                   "expected": _failure("MATERIAL", material_cause="RECORD",
                                        calls=_calls(prepare=1))})
    return tuple(result)


def content_cases():
    """Real modeled signature acceptance cannot bypass policy or full structure.

    The selected content stimuli already freeze independent syntax, tuple,
    digest and archive outcomes.  The wrappers must verify the exact SAME
    retained definition/payload address and length, not merely equal copies.
    """
    selected_policy = {"definition-whitespace-old-pin", "payload-byte-before-inspection",
                       "payload-truncated-suffix-before-inspection",
                       "payload-zero-extension-before-inspection", "different-project-tuple",
                       "metadata-escape", "hash-adapter-first-refusal",
                       "hash-adapter-second-refusal"}
    selected_structure = {"late-checksum", "symbolic-link", "directory-wire-alias"}
    result = []
    for original in (*content.policy_cases(), *content.structure_cases()):
        if original["name"] not in selected_policy | selected_structure:
            continue
        case = copy.deepcopy(original)
        case["signature"] = SIGNATURE
        e = original["expected"]
        c = e["calls"]
        case["expected"] = _failure(
            "CONTENT", content_cause=e["cause"], policy_cause=e["policy_cause"],
            calls=_calls(1, 1, 1, c["policy"], c["hash"], c["inspect"], c["format"], 0),
            signature_release=1, boundary_owner="signature", live_child=True)
        result.append(case)
    return tuple(result)


def mutation_cases():
    """Changed bytes BEFORE a real later consumer must fail its own binding.

    These deliberately violate the caller's immutable-lifetime precondition in
    the linked test double.  They prove recomputation at a subsequent consumer,
    not general mutation detection, synchronization or post-consumption proof.
    No vector promises to detect a signature or payload change after its final
    consumption.  Key mutation changes the private copy, never const policy.
    """
    result = []
    for name, policy_cause, hashes in (
            ("mutate-definition-after-prepare", "DEFINITION", 1),
            ("mutate-definition-after-signature", "DEFINITION", 1),
            ("mutate-payload-after-signature", "PAYLOAD", 2),
            ("mutate-key-after-prepare", "KEY", 0)):
        result.append({"name": name, "fault": name,
                       "expected": _failure("CONTENT", content_cause="POLICY",
                           policy_cause=policy_cause,
                           calls=_calls(1, 1, 1, 1, hashes), signature_release=1,
                           boundary_owner="signature", live_child=True)})
    return tuple(result)


def phase_cases():
    """Semantic boundary faults, with no clock sleeps or candidate-derived data."""
    before = _calls()
    prepared = _calls(prepare=1)
    signed = _calls(1, 1)
    reviewed = _calls(1, 1, 1, 1, 2, 1, 1, 0)
    formatted = _calls(1, 1, 1, 1, 2, 1, 1, 1)
    stages = (("before-prepare", before, 1, 0, "fused", False),
              ("after-prepare", prepared, 1, 0, "fused", False),
              ("after-signature", signed, 0, 1, "signature", True),
              ("after-content", reviewed, 0, 1, "signature", True),
              ("after-format", formatted, 0, 1, "signature", True),
              ("after-publication", formatted, 0, 1, "signature", True))
    result = []
    for phase, calls, fused, sv_release, boundary, child in stages:
        for fault, cause in (("cancel", "CANCEL"), ("clock-error", "CLOCK"),
                             ("clock-backward", "CLOCK")):
            result.append({"name": fault + "-" + phase, "fault": fault,
                           "phase": phase,
                           "expected": _failure(cause, calls=dict(calls),
                               fused_release=fused, signature_release=sv_release,
                               boundary_owner=boundary, live_child=child)})
    for fault in ("clock-negative-seconds", "clock-negative-nanoseconds",
                  "clock-nanoseconds-billion"):
        result.append({"name": fault, "fault": fault, "phase": "before-prepare",
                       "expected": _failure("CLOCK", fused_release=1)})
    # Strict original2s, original11s and post-SV pure2s boundaries, not fresh
    # per-operation allowances.  Nanoseconds are offsets from the named origin.
    for label, phase, origin, nanos, calls, fused, sv_release, boundary, child in (
            ("original-two-before-prepare", "before-prepare", "reservation", 2000000000,
             before, 1, 0, "fused", False),
            ("original-two-before-signature", "after-prepare", "reservation", 2000000000,
             prepared, 1, 0, "fused", False),
            ("original-eleven-after-signature", "after-signature", "reservation", 11000000000,
             signed, 0, 1, "signature", True),
            ("pure-two-after-content", "after-content", "pure-phase", 2000000000,
             reviewed, 0, 1, "signature", True),
            ("pure-two-after-publication", "after-publication", "pure-phase", 2000000000,
             formatted, 0, 1, "signature", True),
            ("original-eleven-dominates-pure", "after-format", "reservation", 11000000000,
             formatted, 0, 1, "signature", True)):
        result.append({"name": label, "fault": "clock-at-offset", "phase": phase,
                       "origin": origin, "offset_nanoseconds": nanos,
                       "pure_origin_offset_nanoseconds": 10000000000 if label ==
                           "original-eleven-dominates-pure" else None,
                       "expected": _failure("DEADLINE", calls=dict(calls),
                           fused_release=fused, signature_release=sv_release,
                           boundary_owner=boundary, live_child=child)})
    return tuple(result)


def ownership_cases():
    """One owner per phase; observation and failures never authorize retries."""
    return (
        {"name": "prepare-known-refusal", "fault": "prepare-record-refusal",
         "expected": _failure("MATERIAL", material_cause="RECORD",
                              calls=_calls(prepare=1))},
        {"name": "prepare-quarantine", "fault": "prepare-unlink-error",
         "expected": _failure("MATERIAL", result="UNCERTAIN", material_cause="IO",
                              calls=_calls(prepare=1), quarantine=True)},
        {"name": "early-close-uncertainty", "fault": "release-close-uncertain",
         "capacity": 0,
         "expected": _failure("ARGUMENT", result="UNCERTAIN", material_cause="CLOSE",
                              fused_release=1, output="unchanged", quarantine=True)},
        {"name": "early-final-boundary-failure", "fault": "early-final-boundary-failure",
         "capacity": 0,
         "expected": _failure("ARGUMENT", fused_release=1, final_boundary_failed=1,
                              output="unchanged")},
        {"name": "normal-host-unsupported", "adapter": "normal-unsupported",
         "expected": _failure("SIGNATURE", signature_cause="UNSUPPORTED",
                              calls=_calls(1, 1), signature_release=1,
                              final_boundary=0, boundary_owner="none")},
        {"name": "signature-child-refusal", "fault": "child-reject",
         "expected": _failure("SIGNATURE", signature_cause="CHILD", calls=_calls(1, 1),
                              signature_release=1, boundary_owner="signature", live_child=True)},
        {"name": "signature-close-uncertainty", "fault": "material-close-0",
         "expected": _failure("SIGNATURE", result="UNCERTAIN", signature_cause="CLOSE",
                              material_cause="CLOSE", calls=_calls(1, 1),
                              signature_release=1, boundary_owner="signature", live_child=True)},
        {"name": "signature-final-boundary-failure", "fault": "final-boundary",
         "expected": _failure("SIGNATURE", signature_cause="BOUNDARY", calls=_calls(1, 1),
                              signature_release=1, boundary_owner="signature", live_child=True)},
        {"name": "nonspawning-observer-loss", "fault": "modeled-observer-loss",
         "nonspawning": True,
         "expected": _failure("SIGNATURE", result="UNCERTAIN", signature_cause="WAIT",
                              calls=_calls(1, 1), signature_release=1,
                              boundary_owner="signature")},
    )


def formatting_cases():
    """Complete publication only, retaining old unauthenticated output bytes."""
    viewer = valid_cases()[0]
    n = viewer["expected"]["length"]
    full = _calls(1, 1, 1, 1, 2, 1, 1, 1)
    fail = _failure("FORMAT", calls=full, signature_release=1,
                    boundary_owner="signature", live_child=True)
    return (
        {"name": "exact-authenticated-capacity", "capacity": n + 1,
         "expected": copy.deepcopy(viewer["expected"])},
        {"name": "one-short-authenticated-capacity", "capacity": n,
         "expected": copy.deepcopy(fail)},
        {"name": "authenticated-format-negative", "fault": "authenticated-format-negative",
         "expected": copy.deepcopy(fail)},
        {"name": "authenticated-format-truncated", "fault": "authenticated-format-truncated",
         "expected": copy.deepcopy(fail)},
    )


# A repeat is deliberately the SAME consumed material and SAME finished result.
# It is NOT_TAKEN without mutation, not a new grant from remembered acceptance.
REUSE_EXPECTATION = copy.deepcopy(NO_WRITE_EXPECTATION)
BYTE_IDENTITY_REQUIREMENTS = {
    "key": "one disjoint private copy of exact compiled key; same copy at prepare/content",
    "definition": "original retained snapshot pointer/length at prepare and real binder",
    "signature": "original retained snapshot pointer/length at prepare and fixed child fd4",
    "payload": "original retained snapshot pointer/length at real hash and full inspector",
    "ready_fds": "anonymous read-only initial-offset-zero exact key/signature/definition",
    "raw_diagnostics_publication": 0,
    "supplied_verdict_or_cached_result": 0,
}
