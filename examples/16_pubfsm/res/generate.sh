#!/usr/bin/env bash

ME=$(realpath "$0")
RES_DIR=$(dirname "$ME")
PROJ_DIR=$(dirname "$RES_DIR")
CMD=${PROJ_DIR}/../../tools/generate.sh

TARGET='generated/src'
$CMD --root "PROJ_DIR" --doc res/PowerManager.siml --target $TARGET
$CMD --root "PROJ_DIR" --doc res/TrafficController.siml --target $TARGET

find ${PROJ_DIR}/${TARGET} \( -name '*.cpp' -o -name '*.hpp' \) -exec dos2unix {} \;
