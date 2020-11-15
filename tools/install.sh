#!/usr/bin/env bash

#
# This script will install the system requirements for this project
#

usage() {
    cat << EOF
Usage: $0

Options:
    -h|--help       Display this message.
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
    *)
    shift
    ;;
esac
done

if [[ $(uname -a) =~ "Ubuntu" ]]; then
    sudo apt update & > /dev/null
    sudo apt install -y gcc-10 g++-10 pkg-config python3-pip
fi

python3 -m pip install --upgrade pip --user
if [ -f requirements.txt ]; then pip install -r requirements.txt --user; fi

export PATH="$PATH:$HOME/.local/bin"

# conan profile new game_project --detect
# if [[ $(uname -a) =~ "Ubuntu" ]]; then
#     conan profile update settings.compiler.libcxx=libstdc++11 game_project
#     conan profile update settings.compiler=gcc game_project
#     conan profile update settings.compiler.version=10 game_project
# fi
