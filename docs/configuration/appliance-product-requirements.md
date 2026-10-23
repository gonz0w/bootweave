# Consumer-owned appliance configuration

Bootweave owns only reusable installer schemas and extension contracts. It has no
suite-product registry or required appliance repository/runtime. Use the generic
[stock-site and package-entry contract](../design/stock-installer-source.md).
Product-specific requirements, scripts, protected inputs, role transitions,
commissioning/lineage, service policies and Bootweave usage instructions belong in
the consumer's repository, alongside its independently usable package/runtime.

The earlier suite-specific requirements/mapping discussion at this path is
superseded by the2026-10-23 independence correction. Its exact source and reviewed
handoffs remain preserved in historical Git/task evidence, not active dispatcher
policy. Matching fields, filenames or digests never convey runtime authority.
A selected security integration must still refuse on failure; independence does
not permit bypassing it. All extension review, execution authorization, target,
authentication and native qualification gates remain effective.
