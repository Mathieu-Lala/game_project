#!/usr/bin/env bash

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
    *)
    shift
    ;;
esac
done

argument="-B build/$build_type -DCMAKE_BUILD_TYPE=$build_type"

case "$(uname)" in
"Linux" | "Darwin")
  ;;
*)
   argument="$argument -A $arch"
  ;;
esac

if [[ $(uname) =~ "Linux" ]]; then
  export PATH="$PATH:$HOME/.local/bin"
  export CC="gcc-10"
  export CXX="g++-10"
fi

cmake $argument .
