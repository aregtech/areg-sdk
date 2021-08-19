#!/usr/bin/env bash

ME=$(realpath $0)

die ()
{
    echo $@
    exit 1
}

test -z "$1" && die "$(basename ${ME}) <doc>"

TOOLS_PATH=$(dirname $ME)
export PROJECT_ROOT=$(dirname $TOOLS_PATH)
export CODE_GEN=share/generated
export CLASSPATH=${TOOLS_PATH}:${TOOLS_PATH}/codegen.jar

exec java com.aregtech.CMFMain --root=${PROJECT_ROOT} --doc=$1 --target=$CODE_GEN
