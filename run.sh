#!/bin/bash

set -e

currentDir=`pwd`
export repoBaseDir=$currentDir


export CXX=/usr/bin/clang++
export CC=/usr/bin/clang

export repoBaseDir=$currentDir

cd conan

./runConan.sh

cd $currentDir


echo "----------------------------------------------------------------------"
echo "                             building                                 "
echo "----------------------------------------------------------------------"

mkdir output
cd output
cmake -DCMAKE_VERBOSE_MAKEFILE:BOOL=ON ../FastIni
cmake --build .

cd $currentDir/output/tests/unit/bin
#./FastIniUnitTest

cd $currentDir/output/tests/benchmark/bin
./FastIniBenchmark

cd $currentDir/output/example
./FastIniExample
