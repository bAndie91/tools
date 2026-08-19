#!/usr/bin/env bash
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "$0")/../.." && pwd)"
CONFPATCH="$ROOT_DIR/user-tools/confpatch"

echo "Using confpatch: $CONFPATCH"

# Ensure the confpatch script is present
if [ ! -x "$CONFPATCH" ]; then
  echo "ERROR: confpatch not found or not executable at $CONFPATCH" >&2
  exit 2
fi

TMPDIR=$(mktemp -d)
cleanup() { rm -rf "$TMPDIR"; }
trap cleanup EXIT

# fixtures directory (this dir)
FIXDIR="$(cd "$(dirname "$0")" && pwd)"

# newline helper: normalize file content to LF for content comparisons
norm() { tr -d '\r' < "$1"; }

fail_count=0
total_count=0

run_case() {
  local line_cont_set="$1"  # either empty or backslash
  local cfg_nl="$2"        # LF or CRLF
  local in_nl="$3"         # LF or CRLF
  local case="$4"          # exists-equal | exists-different | missing

  total_count=$((total_count+1))
  local tag="LC${line_cont_set:-_unset}_CFG${cfg_nl}_IN${in_nl}_${case}"
  echo "\n=== Test: $tag ==="

  # pick fixture files
  local cfg_fixture="$FIXDIR/fixtures/config_${case}_${cfg_nl}.txt"
  local in_fixture  ="$FIXDIR/fixtures/input_${case}_${in_nl}.txt"
  # Note: the input fixture name mapping: for exists cases input may be same or different
  if [ "$case" = "exists-equal" ]; then
    in_fixture="$FIXDIR/fixtures/input_same_${in_nl}.txt"
  elif [ "$case" = "exists-different" ]; then
    in_fixture="$FIXDIR/fixtures/input_diff_${in_nl}.txt"
  else
    in_fixture="$FIXDIR/fixtures/input_missing_${in_nl}.txt"
  fi

  # target file
  local target="$TMPDIR/target_${tag}.txt"
  cp "$cfg_fixture" "$target"

  # record checksum before
  local before_md5
  before_md5=$(md5sum "$target" | cut -d' ' -f1)

  # prepare environment
  export CONFPATCH_TYPE="simple-space"
  # Also set CONFPATCH_FORMAT for compatibility with the test request (script uses CONFPATCH_TYPE)
  export CONFPATCH_FORMAT="simple-space"
  if [ -n "$line_cont_set" ]; then
    export CONFPATCH_LINE_CONTINUATION="\\"
  else
    unset CONFPATCH_LINE_CONTINUATION || true
  fi
  unset CONFPATCH_POST_CHANGE || true

  # run confpatch
  if ! ("$CONFPATCH" "$target" < "$in_fixture"); then
    echo "confpatch exited non-zero for $tag" >&2
    fail_count=$((fail_count+1))
    return
  fi

  local after_md5
  after_md5=$(md5sum "$target" | cut -d' ' -f1)

  # verify
  if [ "$case" = "exists-equal" ]; then
    # file should be unchanged
    if [ "$before_md5" != "$after_md5" ]; then
      echo "[FAIL] $tag: file changed but should not have" >&2
      echo "Before:"; norm "$cfg_fixture" | sed -n '1,200p'
      echo "After:"; norm "$target" | sed -n '1,200p'
      fail_count=$((fail_count+1))
    else
      echo "[OK] $tag: file unchanged as expected"
    fi
  elif [ "$case" = "exists-different" ]; then
    # old value should be commented and new value present
    local normt="$TMPDIR/normt_${tag}.txt"
    norm "$target" > "$normt"
    if ! grep -q -E '^#\s*FOO\s+old$' "$normt"; then
      echo "[FAIL] $tag: old value not commented as expected" >&2
      echo "Result contents:"; cat "$normt"
      fail_count=$((fail_count+1))
    elif ! grep -q -E '^FOO\s+baz$' "$normt"; then
      echo "[FAIL] $tag: new value not present as expected" >&2
      echo "Result contents:"; cat "$normt"
      fail_count=$((fail_count+1))
    else
      echo "[OK] $tag: patched as expected"
    fi
  elif [ "$case" = "missing" ]; then
    local normt="$TMPDIR/normt_${tag}.txt"
    norm "$target" > "$normt"
    if ! grep -q -E '^NEWKEY\s+value$' "$normt"; then
      echo "[FAIL] $tag: new key not added as expected" >&2
      echo "Result contents:"; cat "$normt"
      fail_count=$((fail_count+1))
    else
      echo "[OK] $tag: added key as expected"
    fi
  fi
}

# iterate over combinations
for line_cont in "" "BACKSLASH"; do
  for cfg_nl in LF CRLF; do
    for in_nl in LF CRLF; do
      for case in exists-equal exists-different missing; do
        run_case "$line_cont" "$cfg_nl" "$in_nl" "$case"
      done
    done
  done
done

echo "\nFinished: $((total_count-fail_count)) passed, $fail_count failed out of $total_count tests."
if [ "$fail_count" -ne 0 ]; then
  exit 1
fi
