#!/usr/bin/env bash

#
# This script will start the application
#

build_type=Debug
arch=x64

usage() {
    cat << EOF
Usage: $0 [-h|--help] [--build_type=Debug] [--arch=x64]

Options:
    -h|--help       Display this message.
    --build_type    Set the build type of the CMake project.
    --arch          Set the architecture for the CMake project.
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
    --arch)
    arch="$2"
    shift
    shift
    ;;
    *)
    shift
    ;;
esac
done

# todo : handle msvc build
cd ./build/$build_type/$arch/out && ./app && cd -
