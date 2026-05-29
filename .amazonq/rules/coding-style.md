# Coding conventions

- indentation by tabs
- also indent empty lines
- no line length limit
- for readability, long lines may be broken at the lowest precedence operator of that line; eg. `|`, `||`, `&&` for shell scripts; `and`, `or` for Perl
- text is broken up at semantic boundaries
- the more logically separeted code sections are, the more blank lines are between them
- the narrower a variable's scope is, the shorter and the less descriptive its name is allowed to be
- global variables: CamelCase; ALL_CAPS for env vars and constants
- local/loop variables: snake_case or mixedCase

## Shell script conventions

- `set -e`, `set -o pipefail`, `set -u` each on separate lines
- `then`, `else`, `do` keywords NOT on same line as preceding flow control keyword, except if the whole `if ...; then ...; [else ...;] fi`, `for`/`while ...; do ...; done` is on a single line
- function body opening brace `{` on its own line
- local variables declared with `local varname=...` at the top of functions
- boolean-like variables use empty string for false, non-empty (`yes` or `1`) for true; tested with `[ "$var" ]`
- prefer `[` for most tests in `if` statements and `-a`/`-o` for logical operators
- use `[[` for regexp tests (`=~`) in `if` statements
- `case` patterns use parenthesis-only form: `(pattern)` NOT `pattern)`
- `case` closing `;;` indented at the same level as the pattern body, not at the pattern's indentation level
- command substitution by backticks `` `cmd` `` for shorter commands; `$(...)` for longer ones, especially when multiline
- arithmetics by `$[expr]` syntax
- `n=$[n+1]` instead of `let n++`
- for CLI option and argument parsing pattern, see `boilerplate/script.sh`
- POD documentation embedded via `true <<'EOF' ... EOF` block
- error, diagnostic messages and progress indication go to stderr
- do not hardcode non-standard FD in redirection: use `exec {fd}>>...` for dynamic file descriptors

## Perl script conventions

- `GetOptions` used for option parsing with long and short forms
- global variables from CLI options with `Opt` prefix (eg. `OptVerbose`, `OptOutputFile`)
- subroutine arguments unpacked via `my ($arg1, $arg2) = @_;` or `my $arg = shift;` for single args
- opening brace `{` of multi-line blocks is on the next line than preceding `if`, `else`, `while`, `do`, `for`, `sub` keywords
- opening and closing braces of an `if`, `else`, `while`, `do`, `for`, `sub` block is allowed only if the whole block (disregarding the parenthesized condition blocks of `if`, `while`, and `for`) is in its own line
- `warn`/`die "$0: $filename: $!\n"` for (fatal) errors resulted from a syscall
