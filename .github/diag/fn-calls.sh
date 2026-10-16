#!/usr/bin/env bash
# TEMPORARY diagnostic of diag-814.yml, removed with it.
# fn-calls.sh <elf> <demangled-function-regex>: prints inline frames and calls of the function.
elf="$1"; fn="$2"
line="$(nm -C -S --defined-only "$elf" | grep -E " [tT] ${fn}$" | head -1)"
[ -z "$line" ] && { echo "not found: $fn"; exit 1; }
set -- $line
start=$((16#$1)); size=$((16#$2))
printf '== %s [0x%x,+0x%x)\n' "$fn" "$start" "$size"
objdump -d -C --no-show-raw-insn -l --start-address="$start" --stop-address="$((start+size))" "$elf" \
  | grep -E "^[a-zA-Z_].*\(.*\)( const)?:$|call" | grep -vE "__tsan_(read|write|func)" \
  | cut -c1-200
