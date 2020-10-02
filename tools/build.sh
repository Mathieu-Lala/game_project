#!/usr/bin/env bash

#
# This script will build the target from the cmake project
#

build_type=Debug

usage() {
    cat << EOF
Usage: $0 [-h|--help] [--build_type=Debug]

Options:
    -h|--help       Display this message.
    --build_type    Set the build type of the CMake project.
EOF
    exit 2
}

while [[ $# -gt 0 ]]
do
key="$1"

case $key in
    -h|--help)
    usage $#
    shift
    ;;
    --build_type)
    build_type="$2"
    shift
    shift
    ;;
    *)
    shift
    ;;
esac
done

echo "BUILD TYPE IS $build_type"

export PATH="$PATH:$HOME/.local/bin"

cmake --build build/$build_type --config $build_type -j $(nproc)
