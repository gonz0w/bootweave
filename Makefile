.PHONY: tools hooks check lint test test-ui test-storage secrets verify

tools:
	python3 tools/bootstrap-tools.py

hooks:
	git config core.hooksPath .githooks

check:
	sh tools/check-repo.sh

lint:
	.tools/bin/shellcheck tools/check-repo.sh .githooks/pre-commit .githooks/commit-msg research/ui/native-smoke.sh research/ramdisk/build.sh research/ramdisk/profile research/ramdisk/smoke.sh research/storage/build.sh research/storage/profile

test:
	.tools/venv/bin/python3 -m unittest discover -s tools/tests -v

test-ui:
	.tools/venv/bin/python3 -m unittest discover -s research/ui -v

test-storage:
	.tools/venv/bin/python3 -m unittest discover -s research/storage -v

secrets:
	.tools/bin/gitleaks dir --redact --no-banner --config .gitleaks.toml .

verify: check lint test test-ui test-storage secrets
