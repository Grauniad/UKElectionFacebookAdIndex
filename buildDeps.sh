#!/bin/bash


if [[ "$1" == "" ]]; then
    DEPS_ROOT="$PWD/deps"
else
    DEPS_ROOT="$1"
fi
if [[ -e $DEPS_ROOT/CMakeUtils ]]; then
    echo "Existing CMakeUtils directory, no need to clone"
else
    git clone https://github.com/Grauniad/CMakeUtils.git $DEPS_ROOT/CMakeUtils || exit 1
fi

declare -A depList

depList[JSON]=https://github.com/Grauniad/FixedJSONParserCPP.git

source $DEPS_ROOT/CMakeUtils/build_tools/buildDepsCommon.sh || exit 1
