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

#export PATH="$PATH:$HOME/.local/bin"

python -m pip install --upgrade pip --user --verbose
if [ -f requirements.txt ]; then pip install -r requirements.txt --user --verbose; fi

#conan --version || exit 1
