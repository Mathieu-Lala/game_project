#!/usr/bin/env bash

#
# This script will install the system requirements for this project
#

clone=True

usage() {
    cat << EOF
Usage: $0

Options:
    -h|--help       Display this message.
    --no-clone      Does not clone. Assume executed in the repo.
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
    --no-clone)
    clone=False
    shift
    ;;
    *)
    shift
    ;;
esac
done

if [[ "$clone" == "True" ]]; then
    git clone https://github.com/Mathieu-Lala/game_project.git
    cd game_project
fi

if [[ $(uname -a) =~ "Ubuntu" ]]; then
    sudo apt update & > /dev/null
    sudo apt install -y gcc-10 g++-10 pkg-config python3-pip ssh-askpass
fi

python3 -m pip install --upgrade pip setuptools --user

export PATH="$PATH:$HOME/.local/bin"

if [ -f requirements.txt ]; then pip3 install -r requirements.txt --user; fi

# conan profile new game_project --detect

if [[ $(uname -a) =~ "Ubuntu" ]]; then
    if [ ! "$CI" ]; then
        conan profile update settings.compiler.libcxx=libstdc++11 default
        conan profile update settings.compiler=gcc default
        conan profile update settings.compiler.version=10 default
    fi
fi
