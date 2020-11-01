#!/usr/bin/env bash

#
# This script will start the application
#

build_type=Debug
arch=x64
extra_arg=""

usage() {
    cat << EOF
Usage: $0 [-h|--help] [--build_type=Debug] [--arch=x64] -- [APP_OPT]

Options:
    -h|--help       Display this message.
    --build_type    Set the build type of the CMake project.
    --arch          Set the architecture for the CMake project.
    APP_OPT         Application extra arguments.
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
    --)
    extra_arg="${*:2}"
    break
    ;;
    *)
    shift
    ;;
esac
done

if [[ $(uname -a) =~ "Linux" ]]; then
    cd ./build/$build_type/$arch/out && ./app $extra_arg && cd -
else
    cd ./build/$build_type/out/$build_type && app.exe && cd -
fi
