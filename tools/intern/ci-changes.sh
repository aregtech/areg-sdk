#!/usr/bin/env bash
# Copyright (c) 2026 Aregtech
# Prints whether a complete Git diff matches the workflow's path expression.
set -euo pipefail

changes="$(git diff --name-only "$1" "$2")"
status=0
grep -E "$3" <<< "${changes}" > /dev/null || status=$?
case "${status}" in
  0) echo true ;;
  1) echo false ;;
  *) exit "${status}" ;;
esac
