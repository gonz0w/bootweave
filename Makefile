.PHONY: tools hooks check lint test test-ui test-storage test-handoff test-payload test-plan build secrets verify

CC = cc
CFLAGS = -O2 -std=c99 -Wall -Wextra -Werror -pedantic
PLAN_SOURCES = src/main.c src/plan.c src/plan_io.c src/native.c

build: bin/bootweave

bin/bootweave: $(PLAN_SOURCES) src/plan.h
	mkdir -p bin
	$(CC) $(CFLAGS) -Isrc $(PLAN_SOURCES) -o $@

test-plan:
	.tools/venv/bin/python3 -m unittest discover -s tests -v

tools:
	python3 tools/bootstrap-tools.py

hooks:
	git config core.hooksPath .githooks

check:
	sh tools/check-repo.sh

lint:
	.tools/bin/shellcheck tools/check-repo.sh .githooks/pre-commit .githooks/commit-msg research/ui/native-smoke.sh research/ramdisk/build.sh research/ramdisk/profile research/ramdisk/smoke.sh research/storage/build.sh research/storage/profile research/handoff/build.sh research/handoff/profile research/payload/native_build.sh research/payload/native_run.sh research/payload/auth_native_build.sh research/payload/auth_native_run.sh research/payload/auth_native_cleanup.sh research/payload/placement_build.sh research/payload/placement_run.sh research/payload/review_build.sh research/payload/review_run.sh research/installed/build.sh tests/native_build.sh tests/native_run.sh

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

verify: check lint test test-ui test-storage test-handoff test-payload test-plan secrets
