#! /bin/bash
#This script will download (and compile when necessary) the necessary libraries to compile nwjsmanager.
DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
source $DIR/setup-iup.sh
source $DIR/setup-curl-httponly.sh
