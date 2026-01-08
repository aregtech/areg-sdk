#!/usr/bin/env bash

# ---------------- Defaults ----------------
: "${AREG_SDK_ROOT:=/path/to/areg-sdk}"   # default, change as needed

echo "AREG_SDK_ROOT = $AREG_SDK_ROOT"

# Script directory
BATCH_ROOT=$(dirname "$(realpath "$0")")
PROJECT_ROOT="$BATCH_ROOT/.."          # one level up for project
CODE_GEN="generated/src"               # relative target folder

# Ensure target folder exists
mkdir -p "$PROJECT_ROOT/$CODE_GEN"

# Set CLASSPATH
export CLASSPATH="$CLASSPATH:$AREG_SDK_ROOT/tools:$AREG_SDK_ROOT/tools/codegen.jar"

echo "......................................................"
echo "Generating class of <ServiceName> Service Interface in folder $CODE_GEN ....."
java -jar "$AREG_SDK_ROOT/tools/codegen.jar" \
    --root="$PROJECT_ROOT" \
    --doc="services/<ServiceName>.siml" \
    --target="$CODE_GEN"

echo "......................................................"
echo
echo "End of code generating"

# Pause equivalent in shell
read -p "Press Enter to continue..."
