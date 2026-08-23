#!/usr/bin/env bash
# ===========================================================================
# Template to generate code from an Areg document on Linux and macOS. Copy it
# into your project, set AREG_SDK_ROOT and DOCUMENT below, and run it.
#
# The generator takes three paths:
#   --root=<path>   the project root. Absolute.
#   --doc=<path>    the .siml, .fsml or .dtml document to generate from.
#                   Absolute, or relative to --root.
#   --target=<path> the generate root: the one folder the project's generated
#                   sources go under. Absolute, or relative to --root. Each
#                   document gets the folder its own path names inside it.
#
# A document that includes another one is generated together with it. Name the
# .siml or the .fsml here and its includes come with it.
# ===========================================================================

# The Areg SDK root. Set it here, or in the environment before running.
: "${AREG_SDK_ROOT:=/path/to/areg-sdk}"

# The document to generate from, relative to the project root.
DOCUMENT="services/ServiceName.siml"

# The folder of this file, and the project root one level above it.
BATCH_ROOT=$(dirname "$(realpath "$0")")
PROJECT_ROOT=$(realpath "$BATCH_ROOT/..")

# The generate root, relative to the project root.
CODE_GEN="generated/src"

echo "AREG_SDK_ROOT = $AREG_SDK_ROOT"
echo "......................................................"
echo "Generating the code of $DOCUMENT into $CODE_GEN ....."

java -jar "$AREG_SDK_ROOT/tools/codegen.jar" \
    --root="$PROJECT_ROOT" \
    --doc="$DOCUMENT" \
    --target="$CODE_GEN"

if [ $? -ne 0 ]; then
    echo
    echo "The code generator failed. See the messages above."
    exit 1
fi

echo "......................................................"
echo
echo "End of code generating"

read -p "Press Enter to continue..."
