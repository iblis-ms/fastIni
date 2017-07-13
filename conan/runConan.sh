#!/bin/bash

currentDir=`pwd`

if [ ! -d "conan_gmock" ]
then
  git clone https://github.com/iblis-ms/conan_gmock.git
  cd "conan_gmock"
  git checkout -b GMock_GTest_ver_master origin/GMock_GTest_ver_master
  cd -
fi

if [ ! -d "conan_gbenchmark" ]
then
  git clone https://github.com/iblis-ms/conan_gbenchmark.git
  cd "conan_gbenchmark"
  git checkout -b Google_Benchmark_master origin/Google_Benchmark_master
  cd -
fi

./startConanServer.sh

cd $repoBaseDir/FastIni/tests/benchmark
conan install --build -s compiler=clang -s compiler.version=4.0 -s compiler.libcxx=libc++

cd $repoBaseDir/FastIni/tests/unit
conan install --build -s compiler=clang -s compiler.version=4.0 -s compiler.libcxx=libc++

cd $currentDir

