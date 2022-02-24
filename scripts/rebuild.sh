#!/bin/bash 

# Get in repository root directory
CURDIR=$(pwd)
SCRIPTDIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
cd $SCRIPTDIR/..

CONFIG=$1

# Build
scripts/clean.sh
scripts/build.sh $CONFIG
