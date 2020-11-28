#!/usr/bin/env bash

#
# This script will build the target from the cmake project
#

build_type=Debug
arch=x64
test=false

usage() {
    cat << EOF
Usage: $0 [-h|--help] [--build_type=Debug] [--arch=x64] [--test]

Options:
    -h|--help       Display this message.
    --build_type    Set the build type of the CMake project.
    --arch          Set the architecture for the CMake project.
    --test          Build the test report (.xml)
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
    --test)
    test=true
    shift
    ;;
    *)
    shift
    ;;
esac
done

export PATH="$PATH:$HOME/.local/bin"

build_folder=build/$build_type

if [[ $(uname -a) =~ "Linux" ]]; then
    build_folder="$build_folder/$arch"
fi

cmake --build $build_folder --config $build_type # -j $(nproc --ignore=4)

if [[ "$test" == "true" ]]; then
    cd $build_folder && ctest # -j $(nproc --ignore=4)
fi
