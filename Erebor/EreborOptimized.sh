#!/usr/bin/env bash

LD_LIBRARY_PATH=$LD_LIBRARY_PATH:bin/DebugOptimized-linux-x86_64
export LD_LIBRARY_PATH
bin/DebugOptimized-linux-x86_64/Erebor $@