.PHONY: tools hooks check lint test test-ui test-storage test-handoff test-payload secrets verify

tools:
	python3 tools/bootstrap-tools.py

hooks:
	git config core.hooksPath .githooks

check:
	sh tools/check-repo.sh

lint:
	.tools/bin/shellcheck tools/check-repo.sh .githooks/pre-commit .githooks/commit-msg research/ui/native-smoke.sh research/ramdisk/build.sh research/ramdisk/profile research/ramdisk/smoke.sh research/storage/build.sh research/storage/profile research/handoff/build.sh research/handoff/profile research/payload/native_build.sh research/payload/native_run.sh research/payload/auth_native_build.sh research/payload/auth_native_run.sh research/payload/auth_native_cleanup.sh research/payload/placement_build.sh research/payload/placement_run.sh

test:
	.tools/venv/bin/python3 -m unittest discover -s tools/tests -v

test-ui:
	.tools/venv/bin/python3 -m unittest discover -s research/ui -v

test-storage:
	.tools/venv/bin/python3 -m unittest discover -s research/storage -v

test-handoff:
	.tools/venv/bin/python3 -m unittest discover -s research/handoff -v

test-payload:
	.tools/venv/bin/python3 -m unittest discover -s research/payload -v

secrets:
	.tools/bin/gitleaks dir --redact --no-banner --config .gitleaks.toml .

verify: check lint test test-ui test-storage test-handoff test-payload secrets
