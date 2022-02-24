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


if [ $# -eq 0 ]; then
    CONFIG="Release"
else
    CONFIG=$1
fi

# Check for valid config
if [ "$CONFIG" = "Debug" ] || [ "$CONFIG" = "Release" ]
then
	echo "Building $CONFIG config"
else
	echo "Unknown config"
	exit
fi

# Build
mkdir build
cd build
cmake -DCMAKE_BUILD_TYPE=$CONFIG ..
make
