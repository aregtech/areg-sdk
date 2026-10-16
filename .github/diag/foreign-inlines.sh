#!/usr/bin/env bash
# TEMPORARY diagnostic of diag-814.yml, removed with it.
# foreign-inlines.sh <elf> <frame-regex> <owner-regex>: functions not matching owner-regex that
# contain an inlined frame matching frame-regex (a body folded in from another instantiation).
objdump -d -C --no-show-raw-insn -l "$1" | awk -v frame="$2" -v owner="$3" '
  /^[0-9a-f]+ <.*>:$/ { fn=$0; sub(/^[0-9a-f]+ </,"",fn); sub(/>:$/,"",fn); next }
  /^[a-zA-Z_].*:$/ && $0 ~ frame && fn !~ owner {
      line=$0; sub(/:$/,"",line); key=fn " <= " line
      if (!(key in seen)) { seen[key]=1; print fn "\n    <= " line }
  }'
