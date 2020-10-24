#!/usr/bin/env bash

#
# This script will generate the cache for the cmake project
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

argument="-DCMAKE_BUILD_TYPE=$build_type"

case "$(uname)" in
"Linux")
  argument="$argument -B build/$build_type/$arch"
  ;;
*) # Windows
  argument="$argument -B build/$build_type -A $arch"
  ;;
esac

if [[ $(uname -a) =~ "Ubuntu" ]]; then
  export CC="gcc-10"
  export CXX="g++-10"
fi

export PATH="$PATH:$HOME/.local/bin"
export CONAN_SYSREQUIRES_MODE=enabled

cmake $argument -j $(nproc) -DENABLE_CACHE=ON .
