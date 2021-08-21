#!/usr/bin/env bash

ME=$(realpath $0)
RES_DIR=$(dirname $ME)
CMD=${RES_DIR}/../../../tools/generate.sh

$CMD --root /home/biwiki/github/areg-sdk/examples/10_locsvchello --doc res/HelloWorld.siml --target _generated_
