#!/bin/bash 

# Check OS
UNAME="$(uname -s)"
case "${UNAME}" in
    Linux*)     OS=Linux;;
    Darwin*)    OS=Mac;;
    CYGWIN*)    OS=Windows;;
    MINGW*)     OS=Windows;;
    *)          OS="UNKNOWN:${unameOut}"
esac

# Get in repository root directory
CURDIR=$(pwd)
SCRIPTDIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
cd $SCRIPTDIR/..

# Remove build directory
rm -rf build/
