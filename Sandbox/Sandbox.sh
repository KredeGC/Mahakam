#!/usr/bin/env bash

cd ../Erebor
LD_LIBRARY_PATH=$LD_LIBRARY_PATH:bin/Release-linux-gcc-x86_64
export LD_LIBRARY_PATH
../Sandbox/bin/Release-linux-gcc-x86_64/Sandbox