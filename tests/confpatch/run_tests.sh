#!/usr/bin/env bash
set -euo pipefail

ROOT_DIR=`cd "$(dirname "$0")/../.." && pwd`
CONFPATCH="$ROOT_DIR/user-tools/confpatch"

echo "Using confpatch: $CONFPATCH"

# Ensure the confpatch script is present
if [ ! -x "$CONFPATCH" ]; then
  echo "ERROR: confpatch not found or not executable at $CONFPATCH" >&2
  exit 2
fi

TMPDIR=$(mktemp -d)
echo "temporary dir: $TMPDIR" >&2
cleanup() { rm -rf "$TMPDIR"; }
#trap cleanup EXIT

# fixtures directory (this dir)
FIXDIR=`cd "$(dirname "$0")" && pwd`

# newline helper: normalize file content to LF for content comparisons
norm() { tr -d '\r' < "$1"; }

fail_count=0
total_count=0

run_case() {
  local line_cont_set="$1"  # either empty or backslash
  local cfg_le="$2"        # LF or CRLF
  local inp_le="$3"         # LF or CRLF
  local case="$4"          # exists-same | exists-diff | missing

  total_count=$((total_count+1))
  local tag="LC${line_cont_set:-_unset}_CFG${cfg_le}_IN${inp_le}_${case}"
  echo -e "\n=== Test: $tag ==="

  # pick fixture files
  local cfg_fixture="$FIXDIR/fixtures/config_${case}_${cfg_le}.txt"
  local inp_fixture="$FIXDIR/fixtures/input_${case}_${inp_le}.txt"

  # target file
  local target="$TMPDIR/target_${tag}.txt"
  cp "$cfg_fixture" "$target"

  # record checksum before
  local before_md5
  before_md5=$(md5sum "$target" | cut -d' ' -f1)

  # prepare environment
  export CONFPATCH_FORMAT="simple-space"
  if [ -n "$line_cont_set" ]; then
    export CONFPATCH_LINE_CONTINUATION="\\"
  else
    unset CONFPATCH_LINE_CONTINUATION || true
  fi
  unset CONFPATCH_POST_CHANGE || true

  # run confpatch
  if ! ("$CONFPATCH" "$target" < "$inp_fixture"); then
    echo "confpatch exited non-zero for $tag" >&2
    fail_count=$((fail_count+1))
    return
  fi

  local after_md5
  after_md5=$(md5sum "$target" | cut -d' ' -f1)

  # verify
  if [ "$case" = "exists-same" ]; then
    # file should be unchanged
    if [ "$before_md5" != "$after_md5" ]; then
      echo "[FAIL] $tag: file changed but should not have" >&2
      echo "Before:"; norm "$cfg_fixture" | sed -n '1,200p'
      echo "After:"; norm "$target" | sed -n '1,200p'
      fail_count=$((fail_count+1))
    else
      echo "[OK] $tag: file unchanged as expected"
    fi
  elif [ "$case" = "exists-diff" ]; then
    # old value should be commented and new value present
    local normt="$TMPDIR/normt_${tag}.txt"
    norm "$target" > "$normt"
    if ! grep -q -E '^#\s*FOO\s+old$' "$normt"; then
      echo "[FAIL] $tag: old value not commented as expected" >&2
      echo "Result contents:"; cat "$normt"
      fail_count=$((fail_count+1))
    else
      if [ -n "$line_cont_set" ]; then
        # Expect the new definition to be written as continued physical lines
        if ! grep -q -E '^FOO\s+ba\\$' "$normt" || ! grep -q -E '^z$' "$normt"; then
          echo "[FAIL] $tag: new continued value not present as expected" >&2
          echo "Result contents:"; cat "$normt"
          fail_count=$((fail_count+1))
        else
          echo "[OK] $tag: patched as expected (continued lines)"
        fi
      else
        if ! grep -q -E '^FOO\s+baz$' "$normt"; then
          echo "[FAIL] $tag: new value not present as expected" >&2
          echo "Result contents:"; cat "$normt"
          fail_count=$((fail_count+1))
        else
          echo "[OK] $tag: patched as expected"
        fi
      fi
    fi
  elif [ "$case" = "missing" ]; then
    local normt="$TMPDIR/normt_${tag}.txt"
    norm "$target" > "$normt"
    if [ -n "$line_cont_set" ]; then
      if ! grep -q -E '^NEWKEY\s+val\\$' "$normt" || ! grep -q -E '^ue$' "$normt"; then
        echo "[FAIL] $tag: new continued key not added as expected" >&2
        echo "Result contents:"; cat "$normt"
        fail_count=$((fail_count+1))
      else
        echo "[OK] $tag: added key as expected (continued lines)"
      fi
    else
      if ! grep -q -E '^NEWKEY\s+value$' "$normt"; then
        echo "[FAIL] $tag: new key not added as expected" >&2
        echo "Result contents:"; cat "$normt"
        fail_count=$((fail_count+1))
      else
        echo "[OK] $tag: added key as expected"
      fi
    fi
  fi
}

# iterate over combinations
for line_cont in "" "BACKSLASH"; do
  for cfg_le in LF CRLF; do
    for inp_le in LF CRLF; do
      for case in exists-same exists-diff missing; do
        run_case "$line_cont" "$cfg_le" "$inp_le" "$case"
      done
    done
  done
done

echo -e "\nFinished: $((total_count-fail_count)) passed, $fail_count failed out of $total_count tests."
if [ "$fail_count" -ne 0 ]; then
  exit 1
fi

cleanup
