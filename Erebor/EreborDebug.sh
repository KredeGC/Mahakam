#!/usr/bin/env bash

LD_LIBRARY_PATH=$LD_LIBRARY_PATH:bin/Debug-linux-gcc-x86_64
export LD_LIBRARY_PATH
bin/Debug-linux-gcc-x86_64/Erebor $@