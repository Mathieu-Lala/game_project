#!/usr/bin/env bash

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

if [[ $(uname) =~ "Linux" ]]; then
    sudo apt update & > /dev/null
    sudo apt install -y gcc-10 g++-10
fi

python -m pip install --upgrade pip --user --verbose
if [ -f requirements.txt ]; then pip install -r requirements.txt --user --verbose; fi
