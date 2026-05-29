# Coding conventions

- indentation by tabs

## Shell script conventions

- shell `then`, `else`, `do` keywords NOT in same line as preceding flow control keyword
- prefer `[` for most tests in `if` statements and `-a`/`-o` for logical operators
- use `[[` for regexp tests (`=~`) in `if` statements
- shebang is `#!/bin/bash` (or `#!/bin/sh` for POSIX-only scripts)
- `set -e`, `set -o pipefail`, `set -u` each on separate lines, at the top
- `case` patterns use parenthesis-only form: `(pattern)` NOT `pattern)`
- `case` closing `;;` indented at the same level as the pattern body, not at the pattern's indentation level
- function body opening brace `{` on its own line
- local variables declared with `local varname=...` at the top of functions
- boolean-like variables use empty string for false, non-empty (`yes` or `1`) for true; tested with `[ "$var" ]`
- arithmetic uses `$[expr]` syntax
- `n=$[n+1]` instead of `let n++`
- for CLI option and argument parsing pattern, see `boilerplate/script.sh`
- POD documentation embedded via `true <<'EOF' ... EOF` block
- error, diagnostic messages and progress indication go to stderr

## Perl script conventions

- shebang is `#!/usr/bin/env perl`
- `GetOptions` used for option parsing with long and short forms
- global variables controlled by CLI options named `$OptFoo` (CamelCase with `Opt` prefix)
- opening brace `{` on its own line for subroutines
