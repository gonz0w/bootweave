# Local development

Use the Python version in `.python-version` and run make tools hooks, then make
verify. Bootstrap installs checksum-pinned ShellCheck/Gitleaks and hash-pinned
PyYAML into ignored .tools/. Reviewed prebuilt tool artifacts currently cover
macOS/Linux AMD64/ARM64 development hosts. An unsupported host fails explicitly;
this limitation is not an OpenBSD installer compatibility claim.

check validates all working content, including committed files, required documents,
UTF-8/line endings, local links, configuration parsing, duplicates in YAML and
obvious credential patterns. It prohibits GitHub Actions workflows. lint runs
ShellCheck; test runs safeguards in disposable Git repositories; secrets runs
Gitleaks. make verify aggregates these. No installed services or native disks are
created by these checks.

Hooks are local and bypassable. Configure trusted signer identities deliberately
outside versioned source; never download arbitrary signing keys and trust them
silently. Use git commit -S -s and the [review procedure](review.md).
Python is development-only. Native build instructions will be added after a scoped
media experiment; there is no runnable installer command today.
