#!/bin/bash

echo "------------------ start server conan ------------------"

callDir=`pwd`


conan_server&
echo "Conan server 1st run: $?"

./stopConanServer.sh
echo "Conan server 1st run was stopped: $?"

cp server.conf $HOME/.conan_server/

conan_server&
echo "Conan server 2nd run: $?"

remote=`conan remote list | grep "http://localhost:9300"`
if [ -z "$remote" ]; then
  conan remote add local http://localhost:9300
fi

conan user -p demo -r local  demo

cd conan_gmock
conan export iblis_ms/stable
conan upload GMock/master@iblis_ms/stable --all -r=local --force
cd ../conan_gbenchmark
conan export iblis_ms/stable
conan upload GBenchmark/master@iblis_ms/stable --all -r=local --force


cd "$callDir"
