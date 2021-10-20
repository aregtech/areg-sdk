#!/usr/bin/env bash

ME=$(realpath "$0")
RES_DIR=$(dirname "$ME")
PROJ_DIR=$(dirname "$RES_DIR")
CMD=${PROJ_DIR}/../../tools/generate.sh

# NOTE: generated code is a bit different to the current code base in 'generated'
TARGET='generated'
$CMD --root "PROJ_DIR" --doc res/CentralMessager.siml --target $TARGET
$CMD --root "PROJ_DIR" --doc res/ConnectionManager.siml --target $TARGET
$CMD --root "PROJ_DIR" --doc res/DirectConnection.siml --target $TARGET
$CMD --root "PROJ_DIR" --doc res/DirectMessager.siml --target $TARGET

find ${PROJ_DIR}/${TARGET} \( -name '*.cpp' -o -name '*.hpp' \) -exec dos2unix {} \;
