#!/usr/bin/env bash

ME=$(realpath "$0")
TARGET=generated

helpmsg()
{
cat << EOF
$(basename ${ME}) -r <dir> -d <path> [-t <dir>] [-h]
    -r, --root   <dir>    Project root directory.
    -d, --doc    <path>   Path to doc file (.siml), relatve to project directory.
    -t, --target <dir>    Directory for generated code, relatve to project directory.
                          Default is $TARGET.
EOF
exit 0
}

die ()
{
    echo $@
    exit 1
}

test -z "$1" && helpmsg

PROJECT_ROOT=
DOC_PATH=
while [ -n "$1" ]; do
    case $1 in
        -r|--root)
            PROJECT_ROOT=$2
            shift
            shift
            ;;
        -d|--doc)
            DOC_PATH=$2
            shift
            shift
            ;;
        -t|--target)
            TARGET=$2
            shift
            shift
            ;;
        -h|--help)
            helpmsg
            ;;
        *)
            die "Invalid option $1, use -h or --help to see the command usage."
            ;;
    esac
done

test -z "$PROJECT_ROOT" && die "Project root is missing, use -h or --help to see the command usage."
test -z "$DOC_PATH"     && die "Doc path is missing, use -h or --help to see the command usage."

TOOLS_PATH=$(dirname $ME)
export CLASSPATH=${TOOLS_PATH}:${TOOLS_PATH}/codegen.jar

echo ==============================================================================
echo Generating class defined in $(basename ${DOC_PATH}) to $TARGET ....
echo ==============================================================================

exec java -jar ${TOOLS_PATH}/codegen.jar --root=${PROJECT_ROOT} --doc=${DOC_PATH} --target=$TARGET
